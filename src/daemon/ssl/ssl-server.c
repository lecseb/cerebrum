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

#include <string.h>
#include <event.h>
#include <arpa/inet.h>
#include <event2/bufferevent_ssl.h>
#include <event2/listener.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/socket.h>

#include "daemon-alloc.h"
#include "daemon-cond.h"
#include "daemon-list.h"
#include "ssl-connection.h"
#include "ssl-server.h"

struct s_ssl_server {
  struct s_ssl_funcs funcs;
  struct s_loop *loop;

  struct {
    SSL_CTX *context;
    struct evconnlistener *listener;
  } ssl;

  struct s_list *connections;
  void *userdata;
};

/**
 * @brief Any packet received in a communication structure arrive here
 * @param [in] connection: connection originated by the packet
 * @param [in] packet: packet received
 */
static void _s_ssl_server_communication_read(
  struct s_ssl_connection *connection, struct s_ssl_packet *packet)
{
  daemon_return_if_fail(connection);
  daemon_return_if_fail(packet);

  daemon_log(LOG_NOTICE, "incoming packet to process");
}

/**
 * @brief Any packet received in a communication structure arrive here
 * @param [in] connection: connection originated by the packet
 * @param [in] packet: packet received
 */
static void _s_ssl_server_communication_error(
  struct s_ssl_connection *connection, daemon_unused enum e_ssl_error type,
  daemon_unused int error, const struct s_ssl_packet *packet)
{
  daemon_return_if_fail(connection);
  daemon_return_if_fail(packet);

  daemon_log(LOG_ERR, "error transmiting a packet");
}

/**
 * @brief Connect event from the evconnect listener object
 */
static void _s_ssl_server_accept(struct evconnlistener *listener,
  daemon_unused int sockfd, struct sockaddr *sa, daemon_unused int sa_len,
  struct s_ssl_server *server)
{
  daemon_return_if_fail(listener);
  daemon_return_if_fail(sa);
  daemon_return_if_fail(server);

  daemon_log(LOG_NOTICE, "incoming connection");

  struct event_base *base = evconnlistener_get_base(listener);
  daemon_return_if_fail(base);
  SSL *context = SSL_new(server->ssl.context);
  daemon_return_if_fail(context);

  struct bufferevent *buffer = bufferevent_openssl_socket_new(base, sockfd,
    context, BUFFEREVENT_SSL_ACCEPTING, BEV_OPT_CLOSE_ON_FREE);

  /* the connection will be added automatically if it succeed
   * or delete if not */
  struct s_ssl_connection *connection = s_ssl_connection_new(server, buffer,
    (s_ssl_read_cbk)_s_ssl_server_communication_read,
    (s_ssl_error_cbk)_s_ssl_server_communication_error);
  if (!connection)
    daemon_log(LOG_ERR, "failed to create the connection");
}

struct s_ssl_server *s_ssl_server_new(struct s_loop *loop,
  const struct s_ssl_funcs *funcs, void *userdata)
{
  daemon_return_val_if_fail(loop, NULL);
  daemon_return_val_if_fail(funcs, NULL);
  daemon_return_val_if_fail(s_ssl_funcs_check(funcs) == 0, NULL);

  struct s_ssl_server *server = daemon_malloc(sizeof(struct s_ssl_server));
  server->funcs = *funcs;
  server->loop = loop;
  server->userdata = userdata;
  return server;
}

void s_ssl_server_free(struct s_ssl_server *server)
{
  daemon_return_if_fail(server);

  if (server->ssl.context) {
    s_ssl_context_deinit();
    if (server->ssl.listener)
      evconnlistener_free(server->ssl.listener);
    SSL_CTX_free(server->ssl.context);
  }
  s_list_free_full(server->connections, (s_destroy_cbk)s_ssl_connection_free);
  daemon_free(server);
}

int s_ssl_server_connect(struct s_ssl_server *server,
  const char *certificate, const char *private_key)
{
  daemon_return_val_if_fail(server, -EINVAL);
  daemon_return_val_if_fail(certificate, -EINVAL);
  daemon_return_val_if_fail(private_key, -EINVAL);

  server->ssl.context = s_ssl_context_server_new(certificate, private_key);
  daemon_return_val_if_fail(server->ssl.context, -EBADE);

  struct sockaddr_in sin;
  memset(&sin, 0, sizeof(sin));
  sin.sin_family = AF_INET;
  sin.sin_addr.s_addr = htonl(INADDR_ANY);
  sin.sin_port = htons(8000);
  /* Convert IPv4 and IPv6 addresses from text to binary form */
  if (inet_pton(AF_INET, "0.0.0.0", &sin.sin_addr) <= 0) {
    daemon_log(LOG_ERR, "inet_pton failed");
    goto error;
  }

  server->ssl.listener = evconnlistener_new_bind(
    s_loop_tolibevent(server->loop), (evconnlistener_cb)_s_ssl_server_accept,
    server, LEV_OPT_CLOSE_ON_FREE | LEV_OPT_REUSEABLE, 1024,
    (struct sockaddr *)&sin, sizeof(sin));

error:
  return server->ssl.listener ? 0 : -EBADE;
}

int s_ssl_server_write(struct s_ssl_server *server,
  const char *name, const struct s_ssl_packet *packet)
{
  daemon_return_val_if_fail(server, -EINVAL);
  daemon_return_val_if_fail(name, -EINVAL);
  daemon_return_val_if_fail(packet, -EINVAL);

  daemon_log(LOG_WARNING, "not yet implemented");
  return 0;
}

int s_ssl_server_add_connection(struct s_ssl_server *server,
  struct s_ssl_connection *connection)
{
  daemon_return_val_if_fail(server, -EINVAL);
  daemon_return_val_if_fail(connection, -EINVAL);

  server->connections = s_list_append(server->connections, connection);
  return 0;
}

int s_ssl_server_remove_connection(struct s_ssl_server *server,
  struct s_ssl_connection *connection)
{
  daemon_return_val_if_fail(server, -EINVAL);
  daemon_return_val_if_fail(connection, -EINVAL);

  struct s_list *current = s_list_first(server->connections);
  for (; current; current = s_list_next(current)) {
    struct s_ssl_connection *data = s_list_data(current);
    if (data == connection) {
      server->connections = s_list_remove(server->connections, data);
      return 0;
    }
  }
  return -EBADE;
}
