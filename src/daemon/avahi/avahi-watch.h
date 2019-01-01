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

#ifndef _AVAHI_AVAHI_WATCH_H_
# define _AVAHI_AVAHI_WATCH_H_

# include <avahi-common/watch.h>

struct s_avahi_watch;

/**
 * @brief Create a new watch for the specified file descriptor and for the
 * specified events. The API will call the callback function whenever any of
 * the events happens.
 */
typedef AvahiWatch *(*avahi_watch_new_cbk)(const AvahiPoll *api, int fd,
  AvahiWatchEvent event, AvahiWatchCallback callback, void *userdata);

/**
 * @brief Update the events to wait for. It is safe to call this function from
 * an AvahiWatchCallback
 */
typedef void (*avahi_watch_update_cbk)(AvahiWatch *w,
  AvahiWatchEvent event);

/**
 * @brief Create a new watch for the specified file descriptor and for the
 * specified events. The API will call the callback function whenever any of the
 * events happens.
 */
typedef AvahiWatchEvent (*avahi_watch_get_events_cbk)(AvahiWatch *w);

/**
 * @brief Free a watch. It is safe to call this function from an
 * AvahiWatchCallback
 */
typedef void (*avahi_watch_free_cbk)(AvahiWatch *w);

/**
 * @brief Allocate a new watch point
 * @param [in] api: avahi poll interface
 * @param [in] fd: file descriptor
 * @param [in] event: event description
 * @param [in] callback: function to call when watch is triggered
 * @param [in] data: not used
 * @return a valid pointer on success, NULL on error
 */
struct s_avahi_watch *s_avahi_watch_new(const AvahiPoll *api, int fd,
  AvahiWatchEvent evt, AvahiWatchCallback callback, void *data);

/**
 * @brief Update the event description of a watch point
 * @param [in] watch: watch to modify
 * @param [in] event: event description
 */
void s_avahi_watch_update(struct s_avahi_watch *watch, AvahiWatchEvent evt);

/**
 * @brief Get the event description of a watch point
 * @param [in] watch: watch to browse
 * @return the event description on success, 0 on error
 */
AvahiWatchEvent s_avahi_watch_get_events(struct s_avahi_watch *watch);

/**
 * @brief Deallocate a specific watch point
 * @param [in] watch: watch to delete
 */
void s_avahi_watch_free(struct s_avahi_watch *watch);

#endif /* !_AVAHI_AVAHI_WATCH_H_ */
