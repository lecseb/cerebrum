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

#ifndef _DAEMON_COND_H_
# define _DAEMON_COND_H_

# include <assert.h>
# include <errno.h>
# include <stdio.h>
# include <string.h>
# include <libdaemon/dlog.h>

/**
 * @brief assert handler
 * @param [in] cond : condition to check, if false -> assert is called
 * @param [in] str : string to print in case of an assert occured
 * @param [in] ... : possible string parameter
 */
# define daemon_assert(cond, str, ...) { \
  do { \
    if (!(cond)) { \
      daemon_log(LOG_CRIT, "assert: " str, ## __VA_ARGS__); \
      assert(0); \
    } \
  } while (0); \
}

/**
 * @brief checking function parameter (should be set at the begining of the
 * function)
 * @param [in] cond : condition to check, if false -> value is returned
 * @param [in] value : value to return
 * @return no return on success, return value on error (failed)
 */
# define daemon_return_val_if_fail(cond, value) { \
  do { \
    if (!(cond)) { \
      daemon_log(LOG_ERR, "condition failed '%s'", #cond); \
      return value; \
    } \
  } while (0); \
}
/* codecheck_ignore[SPACING] */
# define daemon_return_if_fail(cond) daemon_return_val_if_fail(cond, )

/**
 * @brief Remove unused warning from compilation
 */
# define daemon_unused __attribute__((unused))

#endif /* !_DAEMON_COND_H_ */
