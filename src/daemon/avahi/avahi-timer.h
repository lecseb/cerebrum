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

#ifndef _AVAHI_AVAHI_TIMER_H_
# define _AVAHI_AVAHI_TIMER_H_

# include <avahi-common/watch.h>

struct s_avahi_timer;

/**
 * @brief Set a wakeup time for the polling loop.
 * The API will call the callback function when the absolute time *tv is
 * reached. If tv is NULL, the timeout is disabled. After the timeout expired
 * the callback function will be called and the timeout is disabled. You can
 * reenable it by calling timeout_update()
*/
typedef AvahiTimeout* (*avahi_timeout_new_cbk)(const AvahiPoll *api,
  const struct timeval *tv, AvahiTimeoutCallback callback, void *userdata);

/**
 * @brief Update the absolute expiration time for a timeout, If tv is NULL, the
 * timeout is disabled. It is safe to call this function from an
 * AvahiTimeoutCallback
*/
typedef void (*avahi_timeout_update_cbk)(AvahiTimeout *timer,
  const struct timeval *tv);

/**
 * @brief Free a timeout. It is safe to call this function from an
 * AvahiTimeoutCallback
 */
typedef void (*avahi_timeout_free_cbk)(AvahiTimeout *timer);

/**
 * @brief Allocate a new timer
 * @param [in] api: avahi poll interface
 * @param [in] tv: timeval structure (timeout definition)
 * @param [in] callback: function to call when the timer raised
 * @param [in] data: not used
 * @return a valid pointer on success, NULL on error
 */
struct s_avahi_timer *s_avahi_timer_new(const AvahiPoll *api,
  const struct timeval *tv, AvahiTimeoutCallback callback, void *data);

/**
 * @brief Update the timeval associated to a specific timer
 * @param [in] timer: timer to modify
 * @param [in] tv: timeval to store
 */
void s_avahi_timer_update(struct s_avahi_timer *timer,
  const struct timeval *tv);

/**
 * @brief Deallocate a specific timer
 * @param [in] watch: timer to delete
 */
void s_avahi_timer_free(struct s_avahi_timer *timer);

#endif /* !_AVAHI_AVAHI_TIMER_H_ */
