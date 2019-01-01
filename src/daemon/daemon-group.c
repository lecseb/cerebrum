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

#include "daemon-alloc.h"
#include "daemon-cond.h"
#include "daemon-ctx.h"
#include "avahi/avahi-group.h"

static const char *_g_cert_path = "/home/siroz/Project/mytank/certificate";
static const char *_g_priv_path = "/home/siroz/Project/mytank/private.pem";

/**
 * @brief Call after initialization when everything is done
 * @param [in] ctx: userdata passing through the allocation
 */
static void _s_daemon_ctx_group_running(struct s_daemon_ctx *ctx)
{
  daemon_return_if_fail(ctx);

  daemon_log(LOG_NOTICE, "cerebrum group is running");

  s_ssl_server_connect(ctx->connection, _g_cert_path, _g_priv_path);

  daemon_log(LOG_NOTICE, "everything is ready");
}

/**
 * @brief Call when the group name is already used and an alternative name
 * is needed. Allow the plugin to notify the change and apply it internaly
 * @param [in] ctx: userdata passing through the allocation
 */
static void _s_daemon_ctx_group_collision(struct s_daemon_ctx *ctx)
{
  daemon_return_if_fail(ctx);

  daemon_log(LOG_WARNING, "cerebrum group collision detected");
}

/**
 * @brief Call when an error occured.
 * @param [in] ctx: userdata passing through the allocation
 * @param [in] error: the errno value
 */
static void _s_daemon_ctx_group_failure(struct s_daemon_ctx *ctx,
  daemon_unused int error)
{
  daemon_return_if_fail(ctx);

  daemon_log(LOG_ERR, "cerebrum group failed");
}

const struct s_group_funcs *s_daemon_ctx_group_get_funcs(void)
{
  static const struct s_group_funcs funcs = {
    .collision = (s_group_collision_cbk)_s_daemon_ctx_group_collision,
    .failure = (s_group_failure_cbk)_s_daemon_ctx_group_failure,
    .running = (s_group_running_cbk)_s_daemon_ctx_group_running
  };
  return &funcs;
}
