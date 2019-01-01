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
#include "avahi-watch.h"
#include "daemon-alloc.h"
#include "daemon-cond.h"
#include "daemon-loop.h"

struct s_avahi_watch {
  struct event_base *base;
  AvahiWatchCallback callback;
  struct event *event;
  int fd;
  void *userdata;
};

static void _s_avahi_watch_cbk(daemon_unused evutil_socket_t fd,
  daemon_unused short what, struct s_avahi_watch *watch)
{
  daemon_return_if_fail(watch);

  AvahiWatchEvent events = 0;
  if (what & EV_READ)
    events |= AVAHI_WATCH_IN;
  if (what & EV_WRITE)
    events |= AVAHI_WATCH_OUT;

  watch->callback((AvahiWatch *)watch, fd, events, watch->userdata);
}

struct s_avahi_watch *s_avahi_watch_new(const AvahiPoll *api, int fd,
  AvahiWatchEvent events, AvahiWatchCallback callback, void *data)
{
  daemon_return_val_if_fail(api, NULL);
  daemon_return_val_if_fail(callback, NULL);

  struct s_avahi_watch *watch = daemon_malloc(sizeof(struct s_avahi_watch));
  watch->base = s_loop_tolibevent(api->userdata);

  short ev_events = EV_PERSIST;
  if (events & AVAHI_WATCH_IN)
    ev_events |= EV_READ;
  if (events & AVAHI_WATCH_OUT)
    ev_events |= EV_WRITE;

  watch->callback = callback;
  watch->event = event_new(watch->base, fd, ev_events,
    (event_callback_fn)_s_avahi_watch_cbk, watch);
  watch->fd = fd;
  watch->userdata = data;

  if (!watch->event || event_add(watch->event, NULL) != 0)
    goto error;

  return watch;

error:
  daemon_log(LOG_ERR, "failed to allocate avahi watch instance");
  s_avahi_watch_free(watch);
  return NULL;
}

void s_avahi_watch_update(struct s_avahi_watch *watch, AvahiWatchEvent events)
{
  daemon_return_if_fail(watch);

  event_del(watch->event);
  event_free(watch->event);

  short ev_events = EV_PERSIST;
  if (events & AVAHI_WATCH_IN)
    ev_events |= EV_READ;
  if (events & AVAHI_WATCH_OUT)
    ev_events |= EV_WRITE;

  watch->event = event_new(watch->base, watch->fd, ev_events,
    (event_callback_fn)_s_avahi_watch_cbk, watch);

  if (!watch->event || event_add(watch->event, NULL))
    daemon_log(LOG_ERR, "failed to update an event");
}

AvahiWatchEvent s_avahi_watch_get_events(struct s_avahi_watch *watch)
{
  daemon_return_val_if_fail(watch, 0);

  AvahiWatchEvent events = 0;
  if (event_pending(watch->event, EV_READ, NULL))
    events |= AVAHI_WATCH_IN;
  if (event_pending(watch->event, EV_WRITE, NULL))
    events |= AVAHI_WATCH_OUT;

  return events;
}

void s_avahi_watch_free(struct s_avahi_watch *watch)
{
  daemon_return_if_fail(watch);

  event_del(watch->event);
  event_free(watch->event);
  daemon_free(watch);
}
