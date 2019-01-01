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

#include <arpa/inet.h>
#include <libdaemon/dlog.h>
#include <sys/eventfd.h>

#include "daemon-alloc.h"
#include "daemon-cond.h"
#include "daemon-ctx.h"
#include "avahi/avahi-browser.h"
#include "ssl/ssl-client.h"

static const char *_g_cert_path = "/home/siroz/Project/mytank/certificate";

/**
 * @brief Call when an error occured.
 * @param [in] daemon: userdata passing through the allocation
 * @param [in] error: the errno value
 */
static void _s_daemon_ctx_failure(struct s_daemon_ctx *ctx, int error)
{
  daemon_return_if_fail(ctx);

  daemon_log(LOG_ERR, "an error occured '%s'\n", strerror(error));
}

/**
 * @brief Call when a service is found
 * @param [in] ctx: userdata passing through the allocation
 * @param [in] domain: domain associated to the service
 * @param [in] address: address of the service
 * @param [in] port: port associated to the service
 * @param [in] txt: text data of the service
 */
static void _s_daemon_ctx_find(struct s_daemon_ctx *ctx,
  struct s_browser_data *data)
{
  daemon_return_if_fail(ctx);
  daemon_return_if_fail(data);

  daemon_log(LOG_NOTICE, "cerebrum '%s' found\n", data->name);

  struct sockaddr_in sin = { 0, };
  memset(&sin, '0', sizeof(sin));
  sin.sin_family = AF_INET;
  sin.sin_port = htons(8000);
  /* Convert IPv4 and IPv6 addresses from text to binary form */
  if (inet_pton(AF_INET, data->address, &sin.sin_addr) <= 0) {
    daemon_log(LOG_ERR, "inet_pton failed\n");
    goto error;
  }

  ctx->connection = s_ssl_client_new(ctx->loop, s_daemon_ctx_ssl_get_funcs(),
    ctx);
  s_ssl_client_connect(ctx->connection, _g_cert_path, &sin);

error:
  s_browser_data_free(data);
  return;
}

/**
 * @brief Call when a service is removed
 * @param [in] ctx: userdata passing through the allocation
 * @param [in] data: service data to remove
 */
static void _s_daemon_ctx_remove(struct s_daemon_ctx *ctx,
  const struct s_browser_data *data)
{
  daemon_return_if_fail(ctx);
  daemon_return_if_fail(data);

  daemon_log(LOG_NOTICE, "service removed\n");
}

const struct s_browser_funcs *s_daemon_ctx_browser_get_funcs(void)
{
  static struct s_browser_funcs funcs = {
    .failure = (s_browser_failure_cbk)_s_daemon_ctx_failure,
    .find = (s_browser_find_cbk)_s_daemon_ctx_find,
    .remove = (s_browser_remove_cbk)_s_daemon_ctx_remove
  };
  return &funcs;
}
