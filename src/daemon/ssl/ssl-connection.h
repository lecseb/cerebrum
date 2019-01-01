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

#ifndef _SSL_SSL_CONNECTION_H_
# define _SSL_SSL_CONNECTION_H_

# include <stdint.h>
# include <event2/event.h>
# include <event2/bufferevent.h>
# include <netinet/in.h>

# include "ssl.h"
# include "ssl-packet.h"
# include "ssl-server.h"

struct s_ssl_connection;

/**
 * @brief Allocate a new ssl connection
 * @param [in] server: server instance
 * @param [in] buffer: buffer event instance
 * @param [in] read: incoming packet callback
 * @param [in] error: error receiving/transmitting packet callback
 * @return a valid pointer on success, NULL on error
 */
struct s_ssl_connection *s_ssl_connection_new(struct s_ssl_server *server,
  struct bufferevent *buffer, s_ssl_read_cbk read, s_ssl_error_cbk error);

/**
 * @brief Deallocate a specific ssl connection
 * @param [in] connection: connection to delete
 */
void s_ssl_connection_free(struct s_ssl_connection *connection);

/**
 * @brief Write a packet in the connection
 * @param [in] connection: connection concerned by the packet
 * @param [in] packet: payload received
 * @return 0 on success, an -errno value on error
 */
int s_ssl_connection_write(struct s_ssl_connection *connection,
  const struct s_ssl_packet *packet);

#endif /* !_SSL_SSL_CONNECTION_H_ */
