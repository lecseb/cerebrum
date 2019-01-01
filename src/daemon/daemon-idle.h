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

#ifndef _DAEMON_IDLE_H_
# define _DAEMON_IDLE_H_

# include "daemon-loop.h"

/**
 * @brief Use to create an idle event and wait for further treatment to do
 */
struct s_task_idle;

/**
 * @brief Allocate an idle task
 * @param [in] loop: loop assicated with the event
 * @return a valid pointer on success, NULL on error
 */
struct s_task_idle *s_task_idle_new(struct s_loop *loop);

/**
 * @brief Deallocate a specific idle task
 * @param [in] task: task to delete
 */
void s_task_idle_free(struct s_task_idle *idle);

/**
 * @brief Wakeup the task
 * @param [in] idle: task to wakeup
 * @return 0 on success, an errno value on error
 */
int s_task_idle_wakeup(struct s_task_idle *idle);

#endif /* !_DAEMON_IDLE_H_ */
