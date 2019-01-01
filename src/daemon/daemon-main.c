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

#include <signal.h>
#include <errno.h>
#include <string.h>
#include <libdaemon/dfork.h>
#include <libdaemon/dsignal.h>
#include <libdaemon/dlog.h>
#include <libdaemon/dpid.h>
#include <libdaemon/dexec.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/unistd.h>
#include <sys/select.h>

#include "daemon.h"
#include "daemon-cond.h"

/**
 * @brief Start the daemon process
 * @return 0 on success, an errno value on error
 */
static int _daemon_fork_process(void)
{
  int ret = 0;

  if (daemon_check_process() == 1) {
    pid_t pid = daemon_fork();
    /* Do the fork */
    if (pid < 0) {
      daemon_retval_done();
      return errno;
    } else if (pid > 0) {
      /* Wait for 20 seconds for the return value passed from the daemon
       * process */
      ret = daemon_retval_wait(20);
      if (ret != 0) {
        daemon_log(LOG_ERR, "failed to recieve the daemon status: %s",
          strerror(ret));
        return ret;
      }
      daemon_log(LOG_INFO, "daemon returned value '%d'", ret);
      return ret;
    } else {
      return daemon_load_process();
    }
  }
  daemon_log(LOG_ERR, "process already started");
  return -EALREADY;
}

/**
 * @brief initialize the libdaemon layer
 * @return 0 on success, an errno value on error
 */
static int _daemon_initialize(const char *name)
{
  daemon_return_val_if_fail(name, -EINVAL);

  /* Reset signal handlers */
  if (daemon_reset_sigs(-1) < 0) {
    daemon_log(LOG_ERR, "failed to reset all signal handlers: %s",
      strerror(errno));
    return errno;
  }

  /* Unblock signals */
  if (daemon_unblock_sigs(-1) < 0) {
    daemon_log(LOG_ERR, "failed to unblock all signals: %s", strerror(errno));
    return errno;
  }

  /* Set indetification string for the daemon for both syslog and PID file */
  daemon_log_ident = daemon_ident_from_argv0((char *)name);
  daemon_pid_file_ident = daemon_log_ident;

  /* Prepare for return value passing from the initialization procedure of
   * the daemon process */
  if (daemon_retval_init() < 0) {
      daemon_log(LOG_ERR, "failed to create pipe.");
    return errno;
  }

  return 0;
}

int main(int argc, char *argv[])
{
  int ret = _daemon_initialize(argv[0]);

  if (ret == 0) {
    struct s_options *options = s_options_new(argc, argv);
#if DAEMON_SET_VERBOSITY_AVAILABLE
    daemon_set_verbosity(s_options_get_verbosity(options));
#endif /* !DAEMON_SET_VERBOSITY_AVAILABLE */
    switch (s_options_get_process_option(options)) {
    case e_process_option_check:
      ret = daemon_check_process();
      break;
    case e_process_option_kill:
      ret = daemon_kill_process();
      break;
    case e_process_option_start:
      ret = _daemon_fork_process();
      break;
    case e_process_option_reload: {
      ret = daemon_kill_process();
      ret |= _daemon_fork_process();
    }
    default:
      daemon_log(LOG_ERR, "an error occured...");
      ret = -EBADE;
      break;
    }
    s_options_free(options);
  }
  return ret;
}
