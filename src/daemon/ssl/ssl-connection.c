/*
 * This file is part of cerebrum.
 *
 * cerebrum is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * cerebrum is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with cerebrum.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <event.h>

#include "daemon-alloc.h"
#include "daemon-cond.h"
#include "ssl-connection.h"
#include "ssl-server.h"

struct s_ssl_connection {
  struct bufferevent *buffer;
  s_ssl_error_cbk error;
  s_ssl_read_cbk read;
  struct s_ssl_server *server;
};

/**
 * @brief Generate a packet instance from the data gather inside the buffer
 * @param [in] buffer: buffer concerned by that process
 * @return a valid pointer on success, NULL on error
 */
static struct s_ssl_packet *_s_ssl_packet_generate(struct bufferevent *buffer)
{
  daemon_return_val_if_fail(buffer, NULL);

  struct evbuffer *event = evbuffer_new();
  daemon_return_val_if_fail(bufferevent_read_buffer(buffer, event) == 0, NULL);

  struct s_ssl_packet *packet = s_ssl_packet_new(evbuffer_pullup(event, -1),
    evbuffer_get_length(event));

  evbuffer_free(event);
  return packet;
}

/**
 * @brief Read callback for a bufferevent.
 * The read callback is triggered when new data arrives in the input buffer and
 * the amount of readable data exceed the low watermark which is 0 by default.
 * @param [in] buffer: buffer to read
 * @param [in] connection: ssl client representation
 */
static void _s_ssl_connection_read(struct bufferevent *buffer,
  struct s_ssl_connection *connection)
{
  daemon_return_if_fail(buffer);
  daemon_return_if_fail(connection);

  struct s_ssl_packet *packet = _s_ssl_packet_generate(buffer);
  connection->read(connection->server, packet);
  s_ssl_packet_free(packet);
}

/**
 * @brief An event/error callback for a bufferevent.
 * The event callback is triggered if either an EOF condition or another
 * unrecoverable error was encountered.
 * @param [in] buffer: the bufferevent for which the error condition was reached
 * @param [in] what: a conjunction of flags: BEV_EVENT_READING or
 * BEV_EVENT_WRITING to indicate if the error was encountered on the read or
 * write path, and one of the following flags: BEV_EVENT_EOF, BEV_EVENT_ERROR,
 * BEV_EVENT_TIMEOUT, BEV_EVENT_CONNECTED.
 * @param [in] connection: ssl client representation
 */
static void _s_ssl_connection_event(struct bufferevent *buffer, short what,
  struct s_ssl_connection *connection)
{
  daemon_return_if_fail(buffer);
  daemon_return_if_fail(connection);

  if ((what & BEV_EVENT_EOF) == BEV_EVENT_EOF) {
    daemon_log(LOG_WARNING, "a communication is terminated\n");
    goto terminated;
  } else if ((what & BEV_EVENT_TIMEOUT) == BEV_EVENT_TIMEOUT) {
    daemon_log(LOG_WARNING, "a communication timeout\n");
    goto terminated;
  } else if ((what & BEV_EVENT_CONNECTED) == BEV_EVENT_CONNECTED) {
    daemon_log(LOG_NOTICE, "a communication succeed\n");
    s_ssl_server_add_connection(connection->server, connection);
    return;
  }
  enum e_ssl_error error = ((what & BEV_EVENT_WRITING) != BEV_EVENT_WRITING) ?
    e_ssl_error_read : ((what & BEV_EVENT_READING) != BEV_EVENT_READING) ?
    e_ssl_error_write : e_ssl_error_eof;

  if (error == e_ssl_error_eof) {
    daemon_log(LOG_NOTICE, "a communication ended\n");
    goto terminated;
  }
  struct s_ssl_packet *packet = _s_ssl_packet_generate(buffer);
  /* TODO: get the ssl error code value directly */
  connection->error(connection->server, error, 0, packet);
  return;

terminated:
  s_ssl_server_remove_connection(connection->server, connection);
  s_ssl_connection_free(connection);
}

struct s_ssl_connection *s_ssl_connection_new(struct s_ssl_server *server,
  struct bufferevent *buffer, s_ssl_read_cbk read, s_ssl_error_cbk error)
{
  daemon_return_val_if_fail(server, NULL);
  daemon_return_val_if_fail(buffer, NULL);
  daemon_return_val_if_fail(read, NULL);
  daemon_return_val_if_fail(error, NULL);

  struct s_ssl_connection *connection =
    daemon_malloc(sizeof(struct s_ssl_connection));
  connection->buffer = buffer;
  connection->error = error;
  connection->read = read;
  connection->server = server;

  bufferevent_setcb(buffer,
    (bufferevent_data_cb)_s_ssl_connection_read, NULL,
    (bufferevent_event_cb)_s_ssl_connection_event, connection);
  bufferevent_enable(buffer, EV_READ);

  return connection;
}

void s_ssl_connection_free(struct s_ssl_connection *connection)
{
  daemon_return_if_fail(connection);

  bufferevent_free(connection->buffer);
  daemon_free(connection);
}

int s_ssl_connection_write(struct s_ssl_connection *connection,
  const struct s_ssl_packet *packet)
{
  daemon_return_val_if_fail(connection, -EINVAL);
  daemon_return_val_if_fail(packet, -EINVAL);

  return bufferevent_write(connection->buffer, packet->payload,
    packet->size);
}
