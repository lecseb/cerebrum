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

#include <libdaemon/dlog.h>
#include "avahi-client.h"
#include "daemon-alloc.h"
#include "daemon-cond.h"

struct s_client {
  AvahiClient *client;
  int32_t error;
  struct s_client_funcs funcs;
  const AvahiPoll *poll;
  void *userdata;
};

/**
 * @brief Avahi client callback
 * @param [in] cli: avahi client interface
 * @param [in] st: avahi client interface status
 * @param [in] data: smid client instance
 */
static void _s_client_cbk(AvahiClient *avahi_client, AvahiClientState state,
  struct s_client *client)
{
  daemon_return_if_fail(avahi_client);
  daemon_return_if_fail(client);

  int err = avahi_client_errno(avahi_client);
  switch (state) {
  case AVAHI_CLIENT_FAILURE:
    client->funcs.failure(client->userdata, err);
    break;
  case AVAHI_CLIENT_S_RUNNING:
    client->client = avahi_client;
    client->funcs.running(client->userdata);
    break;
  case AVAHI_CLIENT_S_COLLISION:
    client->funcs.collision(client->userdata);
    break;
  case AVAHI_CLIENT_S_REGISTERING:
  case AVAHI_CLIENT_CONNECTING:
    break;
  }
}

struct s_client *s_client_new(const AvahiPoll *poll, void *userdata,
  const struct s_client_funcs *funcs)
{
  daemon_return_val_if_fail(poll, NULL);
  daemon_return_val_if_fail(funcs, NULL);

  struct s_client *client = daemon_malloc(sizeof(struct s_client));
  client->poll = poll;
  client->funcs = *funcs;
  client->userdata = userdata;

  return client;
}

void s_client_free(struct s_client *client)
{
  daemon_return_if_fail(client);

  avahi_client_free(client->client);
  daemon_free(client);
}

int s_client_run(struct s_client *client)
{
  daemon_return_val_if_fail(client, -EINVAL);

  client->client = avahi_client_new(client->poll, 0,
    (AvahiClientCallback)_s_client_cbk, client, &client->error);

  return client->client ? 0 : -EBADE;
}

AvahiClient *s_client_toavahi(struct s_client *client)
{
  daemon_return_val_if_fail(client, NULL);

  return client->client;
}
