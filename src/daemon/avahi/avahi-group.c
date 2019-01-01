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

#include <avahi-client/publish.h>
#include <avahi-common/alternative.h>
#include <avahi-common/error.h>
#include <libdaemon/dlog.h>

#include "daemon-alloc.h"
#include "daemon-cond.h"
#include "daemon-list.h"
#include "avahi/avahi-client.h"
#include "avahi/avahi-group.h"

struct s_group {
  AvahiEntryGroup *entry;
  struct s_group_funcs funcs;
  uint32_t nbr_services;
  struct s_list *services;
  void *userdata;
};

static void _s_group_cbk(AvahiEntryGroup *group,
  AvahiEntryGroupState state, struct s_group *mygroup)
{
  daemon_return_if_fail(group);
  daemon_return_if_fail(mygroup);

  int err = avahi_client_errno(avahi_entry_group_get_client(group));

  /* Called whenever the entry group state changes */
  switch (state) {
  case AVAHI_ENTRY_GROUP_ESTABLISHED:
    mygroup->funcs.running(mygroup->userdata);
    break;
  case AVAHI_ENTRY_GROUP_COLLISION: {
    avahi_entry_group_reset(mygroup->entry);
    mygroup->funcs.collision(mygroup->userdata);
    break;
  }
  case AVAHI_ENTRY_GROUP_FAILURE:
    mygroup->funcs.failure(mygroup->userdata, err);
    break;
  case AVAHI_ENTRY_GROUP_UNCOMMITED:
    daemon_log(LOG_WARNING, "group uncommited\n");
    break;
  case AVAHI_ENTRY_GROUP_REGISTERING:
    daemon_log(LOG_WARNING, "group registering\n");
    break;
  }
}


struct s_group *s_group_new(struct s_client *client, void *userdata,
  const struct s_group_funcs *funcs)
{
  daemon_return_val_if_fail(client, NULL);
  daemon_return_val_if_fail(funcs, NULL);

  AvahiClient *avahi_client = s_client_toavahi(client);
  daemon_return_val_if_fail(avahi_client, NULL);

  struct s_group *group = daemon_malloc(sizeof(struct s_group));
  group->entry = avahi_entry_group_new(avahi_client,
    (AvahiEntryGroupCallback)_s_group_cbk, group);
  group->funcs = *funcs;
  group->userdata = userdata;

  if (!group->entry)
    goto error;

  return group;

error:
  daemon_log(LOG_ERR, "failed to create a group\n");
  s_group_free(group);
  return NULL;
}

void s_group_free(struct s_group *group)
{
  daemon_return_if_fail(group);

  s_list_free_full(group->services, (s_destroy_cbk)s_service_free);
  daemon_free(group);
}

int s_group_add_service(struct s_group *group, struct s_service_data *data)
{
  daemon_return_val_if_fail(group, -EINVAL);
  daemon_return_val_if_fail(data, -EINVAL);

  int ret = avahi_entry_group_add_service(group->entry, data->interface,
    data->protocol, 0, data->name, data->type, data->domain, NULL, data->port,
    data->data, NULL);

  if (ret == 0) {
    uint32_t size = s_list_length(group->services);
    /* append the element in the queue */
    group->services = s_list_append(group->services, data);

    if (size + 1 != s_list_length(group->services)) {
      daemon_log(LOG_ERR, "failed to add into the list\n");
      return -EBADE;
    }
  } else if (ret == AVAHI_ERR_COLLISION) {
    avahi_entry_group_reset(group->entry);
    group->funcs.collision(group);
  }
  daemon_log(ret == 0 ? LOG_NOTICE : LOG_ERR,
    "%s", ret == 0 ? "service added successfully" :
    "failed to add the service");
  return ret;
}

int s_group_remove_service(struct s_group *group, struct s_service_data *data)
{
  daemon_return_val_if_fail(group, -EINVAL);
  daemon_return_val_if_fail(data, -EINVAL);

  struct s_list *element = s_list_find(group->services, data);
  if (element) {
    /* check if the element is well removed */
    uint32_t size = s_list_length(group->services);
    group->services = s_list_remove(group->services, data);
    return !(size == (s_list_length(group->services) - 1));
  }
  return -EAGAIN;
}

int s_group_commit(struct s_group *group)
{
  daemon_return_val_if_fail(group, -EINVAL);

  int ret = avahi_entry_group_commit(group->entry);
  daemon_log(ret == 0 ? LOG_NOTICE : LOG_ERR,
    "%s", ret == 0 ? "group commited successfully" :
    "failed to commit the group");
  return ret;
}
