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
#include <libdaemon/dexec.h>
#include <libdaemon/dfork.h>
#include <libdaemon/dlog.h>
#include <libdaemon/dpid.h>
#include <libdaemon/dsignal.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/unistd.h>
#include <sys/select.h>

#include "daemon.h"
#include "daemon-ctx.h"
#include "daemon-loop.h"

static struct s_daemon_ctx *_g_ctx;

int daemon_check_process(void)
{
  pid_t pid = daemon_pid_file_is_running();

  /* Check that the daemon is not rung twice a the same time */
  if (pid >= 0) {
    daemon_log(LOG_INFO, "daemon is running on PID file %u", pid);
    return 0;
  }
  return 1;
}

int daemon_kill_process(void)
{
  if (daemon_check_process() == 0) {
    int ret = daemon_pid_file_kill_wait(SIGINT, 5);
    /* the cerebrum daemon is designed to quit properly on a sigint */
    if (ret == 0) {
      daemon_log(LOG_INFO, "daemon successfully killed");
      return 0;
    } else {
      daemon_log(LOG_ERR, "failed to kill daemon: %s", strerror(errno));
      return -errno;
    }
  }
  daemon_log(LOG_ERR, "failed to kill the process");
  return -EALREADY;
}

int daemon_load_process(void)
{
  if (daemon_close_all(-1) < 0) {
    daemon_log(LOG_ERR, "failed to close all file descriptors: %s",
      strerror(errno));
    goto finish;
  }

  if (daemon_pid_file_create() < 0) {
    daemon_log(LOG_ERR, "failed to create PID file (%s).", strerror(errno));
    goto finish;
  }

  if (daemon_signal_init(SIGINT, SIGTERM, SIGQUIT, SIGHUP, 0) < 0) {
    daemon_log(LOG_ERR, "failed to register signal handlers (%s).",
      strerror(errno));
    goto finish;
  }

  _g_ctx = s_daemon_ctx_new(daemon_signal_fd());
  daemon_retval_send(_g_ctx ? 0 : EBADE);

  s_daemon_ctx_run(_g_ctx);
  s_daemon_ctx_free(_g_ctx);

  return errno;

finish:
  daemon_retval_send(errno);
  daemon_log(LOG_INFO, "terminating...");
  daemon_retval_send(255);
  daemon_signal_done();
  daemon_pid_file_remove();
  return 0;
}
