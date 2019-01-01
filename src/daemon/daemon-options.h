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

#ifndef _DAEMON_OPTIONS_H_
# define _DAEMON_OPTIONS_H_

enum e_process_option {
  e_process_option_check,
  e_process_option_reload,
  e_process_option_start,
  e_process_option_kill,
  e_process_option_error
};

struct s_options;

/**
 * @brief Allocate a new options structure
 * @param [in] argc: number of argument
 * @param [in] argv: list of argument
 * @return a valid pointer on success, NULL on error
 */
struct s_options *s_options_new(int argc, char *argv[]);

/**
 * @brief Deallocate a specific options instance
 * @param [in] options: structure to delete
 */
void s_options_free(struct s_options *options);

/**
 * @brief Get the process option
 * @param [in] options: options to browse
 * @return a value from @e_process_option
 */
enum e_process_option s_options_get_process_option(struct s_options *options);

/**
 * @brief Get the verbosity level given in parameter
 * @param [in] options: options to browse
 * @return a value >= 0 on success, an -errno value on error
 */
int32_t s_options_get_verbosity(struct s_options *options);

#endif /* !_DAEMON_OPTIONS_H_ */
