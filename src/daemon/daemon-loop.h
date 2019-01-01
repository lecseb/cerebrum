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

#ifndef _DAEMON_LOOP_H_
# define _DAEMON_LOOP_H_

# include <avahi-common/watch.h>
# include <event2/event.h>

struct s_loop;

/**
 * @brief Allocate a new module loop
 * @return a valid pointer on success, NULL on error
 */
struct s_loop *s_loop_new(void);

/**
 * @brief Deallocate a specific module loop
 * @param [in] loop: loop to delete
 */
void s_loop_free(struct s_loop *loop);

/**
 * @brief Start the module loop process
 * @param [in] loop: loop to start
 * @return 0 on success, an -errno value on error
 */
int s_loop_run(struct s_loop *loop);

/**
 * @brief Stop the module loop
 * @param [in] loop: loop to stop
 * @return 0 on success, an -errno value on error
 */
int s_loop_quit(struct s_loop *loop);

/**
 * @brief Convert the module loop into libevent loop
 * @param [in] loop: loop to convert
 * @return a valid pointer on success, NULL on error
 */
struct event_base *s_loop_tolibevent(struct s_loop *loop);

/**
 * @brief Convert the module loop into avahi poll instance
 * @param [in] loop: loop to convert
 * @return a valid pointer on success, NULL on error
 */
const AvahiPoll *s_loop_toavahi(struct s_loop *loop);

#endif /* !_DAEMON_LOOP_H_ */
