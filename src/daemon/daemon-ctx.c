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

/**
 * @brief Event callback raised if a signal is received
 * @param [in] fd: file descriptor of the event
 * @param [in] evt: event received
 * @param [in] userdata: data passing through the event_new
 */
static void _s_daemon_ctx_signal_received(daemon_unused evutil_socket_t fd,
  daemon_unused short e, struct s_daemon_ctx *ctx)
{
  daemon_return_if_fail(ctx);

  s_daemon_ctx_quit(ctx);
}

struct s_daemon_ctx *s_daemon_ctx_new(int fd)
{
  struct s_daemon_ctx *ctx = daemon_malloc(sizeof(struct s_daemon_ctx));
  ctx->loop = s_loop_new();
  ctx->client = s_client_new(s_loop_toavahi(ctx->loop),
    ctx, s_daemon_ctx_client_get_funcs());
  ctx->event = event_new(s_loop_tolibevent(ctx->loop), fd, EV_READ,
    (event_callback_fn)_s_daemon_ctx_signal_received, ctx);

  if (!ctx->client || !ctx->event || !ctx->loop ||
      event_add(ctx->event, NULL) != 0) {
    errno = EBADE;
    goto error;
  }

  return ctx;

error:
  daemon_log(LOG_ERR, "failed to allocate a daemon context");
  s_daemon_ctx_free(ctx);
  return NULL;
}

void s_daemon_ctx_free(struct s_daemon_ctx *ctx)
{
  daemon_return_if_fail(ctx);

  event_del(ctx->event);
  event_free(ctx->event);

  if (ctx->connection)
    s_ssl_server_free(ctx->connection);
  s_client_free(ctx->client);
  s_loop_free(ctx->loop);
}

int s_daemon_ctx_run(struct s_daemon_ctx *ctx)
{
  daemon_return_val_if_fail(ctx, -EINVAL);

  int ret = s_client_run(ctx->client);
  ret |= s_loop_run(ctx->loop);
  return ret;
}

int s_daemon_ctx_quit(struct s_daemon_ctx *ctx)
{
  daemon_return_val_if_fail(ctx, -EINVAL);

  return s_loop_quit(ctx->loop);
}
