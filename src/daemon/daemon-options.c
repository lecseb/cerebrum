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

#include <getopt.h>
#include <libdaemon/dlog.h>
#include "daemon-alloc.h"
#include "daemon-cond.h"
#include "daemon-options.h"

struct s_options {
  enum e_process_option process;
  int32_t verbosity;
};

struct s_options *s_options_new(int argc, char *argv[])
{
  struct s_options *options = daemon_malloc(sizeof(struct s_options));

  options->verbosity = LOG_WARNING;
  if (argc > 1) {
    static const struct option _g_daemon_options[] = {
      { "check", no_argument, 0, 'c' },
      { "kill", no_argument, 0, 'k' },
      { "reload", no_argument, 0, 'r' },
      { "verbose", required_argument, 0, 'v' },
      {0, 0, 0, 0 }
    };
    int option_index = 0;
    int option = getopt_long(argc, argv, "crs", _g_daemon_options,
      &option_index);
    switch (option) {
    case 'c':
      options->process = e_process_option_check;
      break;
    case 'k':
      options->process = e_process_option_kill;
      break;
    case 'r':
      options->process = e_process_option_reload;
      break;
    case 'v':
    default:
      daemon_log(LOG_ERR, "unrecognized option '%c'", option);
      options->process = e_process_option_error;
      break;
    }
  } else {
    options->process = e_process_option_start;
  }
  return options;
}

void s_options_free(struct s_options *options)
{
  daemon_return_if_fail(options);

  daemon_free(options);
}

enum e_process_option s_options_get_process_option(struct s_options *options)
{
  daemon_return_val_if_fail(options, e_process_option_error);

  return options->process;
}

int32_t s_options_get_verbosity(struct s_options *options)
{
  daemon_return_val_if_fail(options, -EINVAL);

  return options->verbosity;
}
