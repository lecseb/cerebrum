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

#include <sys/eventfd.h>
#include "daemon-alloc.h"
#include "daemon-cond.h"
#include "daemon-ctx.h"
#include "avahi/avahi-group.h"

/**
 * @brief Call after initialization when everything is done
 * @param [in] ctx: userdata passing through the allocation
 */
static void _s_daemon_ctx_client_running(struct s_daemon_ctx *ctx)
{
  daemon_return_if_fail(ctx);

  daemon_log(LOG_NOTICE, "cerebrum detection is running");

  ctx->group = s_group_new(ctx->client, ctx, s_daemon_ctx_group_get_funcs());
  if (ctx->group) {
    struct s_service_data *data = s_service_generate();
    if (s_group_add_service(ctx->group, data) == 0) {
      daemon_log(LOG_NOTICE, "service and group created\n");
      if (s_group_commit(ctx->group) == 0)
        return;
    }
  }
  daemon_log(LOG_ERR, "failed to create group or service");
}

/**
 * @brief Call when the group name is already used and an alternative name
 * is needed. Allow the plugin to notify the change and apply it internaly
 * @param [in] ctx: userdata passing through the allocation
 */
static void _s_daemon_ctx_client_collision(struct s_daemon_ctx *ctx)
{
  daemon_return_if_fail(ctx);

  daemon_log(LOG_NOTICE, "cerebrum detection detected a collision");
}

/**
 * @brief Call when an error occured.
 * @param [in] ctx: userdata passing through the allocation
 * @param [in] error: the errno value
 */
static void _s_daemon_ctx_client_failure(struct s_daemon_ctx *ctx, int error)
{
  daemon_return_if_fail(ctx);

  daemon_log(LOG_NOTICE, "cerebrum detection failed '%d'", error);
  s_daemon_ctx_quit(ctx);
}

const struct s_client_funcs *s_daemon_ctx_client_get_funcs(void)
{
  static const struct s_client_funcs funcs = {
    .collision = (s_client_collision_cbk)_s_daemon_ctx_client_collision,
    .failure = (s_client_failure_cbk)_s_daemon_ctx_client_failure,
    .running = (s_client_running_cbk)_s_daemon_ctx_client_running,
  };
  return &funcs;
};
