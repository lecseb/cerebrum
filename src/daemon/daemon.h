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

#ifndef _DAEMON_H_
# define _DAEMON_H_

# include "daemon-options.h"

/**
 * @brief Check if the daemon is already started
 * @return 0 if the daemon isn't started yet, an another value otherwise
 */
int daemon_check_process(void);

/**
 * @brief Daemon kill handler
 * @return 0 on success, an errno value on error
 */
int daemon_kill_process(void);

/**
 * @brief Start the daemon process
 * @return a valid pointer on success, an errno value on error
 */
int daemon_load_process(void);

#endif /* !_DAEMON_H_ */
