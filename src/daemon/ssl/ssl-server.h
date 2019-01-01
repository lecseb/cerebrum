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

#ifndef _SSL_SSL_SERVER_H_
# define _SSL_SSL_SERVER_H_

# include "ssl.h"
# include "daemon-loop.h"

struct s_ssl_server;
struct s_ssl_connection;

/**
 * @brief Allocate a new ssl server
 * @param [in] loop: event loop base instance
 * @param [in] funcs: behavior callback functions
 * @param [in] userdata: userdata to use for @s_ssl_server_funcs callback
 * @return a valid pointer on success, NULL on error
 */
struct s_ssl_server *s_ssl_server_new(struct s_loop *loop,
  const struct s_ssl_funcs *funcs, void *userdata);

/**
 * @brief Deallocate a specific ssl communication server
 * @param [in] server: server to delete
 */
void s_ssl_server_free(struct s_ssl_server *server);

/**
 * @brief Attempt a client connection on the address given in parameter
 * @param [in] server: server to connect
 * @param [in] certificate: certificate to authenticate
 * @param [in] private_key: crypto private key used to autenticate
 * @return 0 on success, an -errno value on error
 */
int s_ssl_server_connect(struct s_ssl_server *server,
  const char *certificate, const char *private_key);

/**
 * @brief Write a packet in the socket
 * @param [in] server: server concerned by the packet
 * @param [in] name: name of the client which will receive the packet
 * @param [in] packet: payload received
 * @return 0 on success, an -errno value on error
 */
int s_ssl_server_write(struct s_ssl_server *server,
  const char *name, const struct s_ssl_packet *packet);

/**
 * @brief Add a connection to the server
 * @param [in] server: server to modify
 * @param [in] connection: connection to add
 * @return 0 on success, an -errno value on error
 */
int s_ssl_server_add_connection(struct s_ssl_server *server,
  struct s_ssl_connection *connection);

/**
 * @brief Remove a connection from the server
 * @param [in] server: server to modify
 * @param [in] connection: connection to remove
 * @return 0 on success, an -errno value on error
 */
int s_ssl_server_remove_connection(struct s_ssl_server *server,
  struct s_ssl_connection *connection);

#endif /* !_SSL_SSL_SERVER_H_ */
