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

#include <libdaemon/dlog.h>

#include "daemon-alloc.h"
#include "daemon-cond.h"
#include "daemon-ctx.h"
#include "ssl/ssl.h"

/**
 * @brief Connection status callback
 * @param [in] ctx: userdata passing through the allocation
 * @param [in] state: current connection status
 */
static void _s_daemon_ctx_ssl_connection(struct s_daemon_ctx *ctx,
  enum e_ssl_connection state)
{
  daemon_return_if_fail(ctx);

  switch (state) {
  case e_ssl_connection_close:
    daemon_log(LOG_NOTICE, "ssl connection closed\n");
    break;
  case e_ssl_connection_connected:
    daemon_log(LOG_NOTICE, "ssl connection connected\n");
    break;
  case e_ssl_connection_timeout:
    daemon_log(LOG_NOTICE, "ssl connection timeout\n");
    break;
  }
}

/**
 * @brief Error status callback, called whenever a read / write operation
 * failed
 * @param [in] ctx: userdata passing through the allocation
 * @param [in] type: error type definition
 * @param [in] packet: payload to send
 * @param [in] error: error received from ssl
 */
static void _s_daemon_ctx_ssl_error(struct s_daemon_ctx *ctx,
  enum e_ssl_error type, daemon_unused int error,
  const struct s_ssl_packet *packet)
{
  daemon_return_if_fail(ctx);

  switch (type) {
  case e_ssl_error_connection:
    daemon_log(LOG_ERR, "failed ssl connection\n");
    s_daemon_ctx_quit(ctx);
    break;
  case e_ssl_error_read:
    daemon_log(LOG_ERR, "failed ssl read\n");
    daemon_return_if_fail(packet);
    break;
  case e_ssl_error_write:
    daemon_log(LOG_ERR, "failed ssl write\n");
    daemon_return_if_fail(packet);
    break;
  default:
    daemon_log(LOG_ERR, "strange state... better to assert\n");
    daemon_assert(0, "swith case not handle");
  }
}

/**
 * @brief Read callback, called whenever a packet is received
 * @param [in] ctx: userdata passing through the allocation
 * @param [in] packet: payload received
 */
static void _s_daemon_ctx_ssl_read(struct s_daemon_ctx *ctx,
  const struct s_ssl_packet *packet)
{
  daemon_return_if_fail(ctx);
  daemon_return_if_fail(packet);

  daemon_log(LOG_NOTICE, "a packet is received");
}

const struct s_ssl_funcs *s_daemon_ctx_ssl_get_funcs(void)
{
  static const struct s_ssl_funcs funcs = {
    .connection = (s_ssl_connection_cbk)_s_daemon_ctx_ssl_connection,
    .error = (s_ssl_error_cbk)_s_daemon_ctx_ssl_error,
    .read = (s_ssl_read_cbk)_s_daemon_ctx_ssl_read,
  };
  return &funcs;
}
