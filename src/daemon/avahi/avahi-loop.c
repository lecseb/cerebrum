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
#include "avahi-timer.h"
#include "avahi-watch.h"
#include "daemon-alloc.h"
#include "daemon-cond.h"
#include "daemon-loop.h"

const AvahiPoll *s_loop_toavahi(struct s_loop *loop)
{
  daemon_return_val_if_fail(loop, NULL);

  static AvahiPoll api = {
    .watch_new = (avahi_watch_new_cbk)s_avahi_watch_new,
    .watch_free = (avahi_watch_free_cbk)s_avahi_watch_free,
    .watch_update = (avahi_watch_update_cbk)s_avahi_watch_update,
    .watch_get_events = (avahi_watch_get_events_cbk)s_avahi_watch_get_events,
    .timeout_new = (avahi_timeout_new_cbk)s_avahi_timer_new,
    .timeout_free = (avahi_timeout_free_cbk)s_avahi_timer_free,
    .timeout_update = (avahi_timeout_update_cbk)s_avahi_timer_update,
    .userdata = NULL
  };
  api.userdata = loop;

  return &api;
}
