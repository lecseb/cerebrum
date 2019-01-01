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
#include <unistd.h>
#include <sys/eventfd.h>
#include "daemon-alloc.h"
#include "daemon-cond.h"
#include "daemon-idle.h"

struct s_task_idle {
  int32_t fd;
  struct event *event;
};

/**
 * @brief Exit ordered by the user. Must quit the event loop
 */
static void _s_task_idle_cbk(daemon_unused evutil_socket_t fd,
  daemon_unused short e, struct s_loop *loop)
{
  daemon_return_if_fail(loop);

  event_base_loopexit(s_loop_tolibevent(loop), NULL);
}

struct s_task_idle *s_task_idle_new(struct s_loop *loop)
{
  daemon_return_val_if_fail(loop, NULL);

  struct s_task_idle *task = daemon_malloc(sizeof(struct s_task_idle));
  task->fd = eventfd(0, EFD_NONBLOCK);
  task->event = event_new(s_loop_tolibevent(loop), task->fd, EV_READ,
    (event_callback_fn)_s_task_idle_cbk, loop);

  if (task->fd < 0 || !task->event || event_add(task->event, NULL) < 0)
    goto error;

  return task;

error:
  daemon_log(LOG_ERR, "failed to create the idle task\n");
  s_task_idle_free(task);
  return NULL;
}

void s_task_idle_free(struct s_task_idle *task)
{
  daemon_return_if_fail(task);

  close(task->fd);
  event_del(task->event);
  event_free(task->event);
  daemon_free(task);
}

int s_task_idle_wakeup(struct s_task_idle *task)
{
  daemon_return_val_if_fail(task, -EINVAL);

  uint64_t u = 0xfffffffffffffffe;
  return !(write(task->fd, &u, sizeof(uint64_t)) != sizeof(uint64_t));
}
