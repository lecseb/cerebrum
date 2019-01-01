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

#ifndef _AVAHI_AVAHI_GROUP_H_
# define _AVAHI_AVAHI_GROUP_H_

# include <avahi-client/client.h>
# include "avahi-client.h"
# include "avahi-service.h"

struct s_group;

/**
 * @brief Call after initialization when everything is done
 * @param [in] userdata: userdata passing through the allocation
 */
typedef void (*s_group_running_cbk)(void *userdata);

/**
 * @brief Call when the group name is already used and an alternative name
 * is needed. Allow the plugin to notify the change and apply it internaly
 * @param [in] userdata: userdata passing through the allocation
 */
typedef void (*s_group_collision_cbk)(void *userdata);

/**
 * @brief Call when an error occured.
 * @param [in] userdata: userdata passing through the allocation
 * @param [in] error: the errno value
 */
typedef void (*s_group_failure_cbk)(void *userdata, int error);

struct s_group_funcs {
  s_group_collision_cbk collision;
  s_group_failure_cbk failure;
  s_group_running_cbk running;
};

/**
 * @brief Allocate a new group
 * @param [in] client: client structure
 * @param [in] userdata: user pointer
 * @param [in] funcs: group behavior structure
 * @return a valid pointer on success, NULL on error
 */
struct s_group *s_group_new(struct s_client *client, void *userdata,
  const struct s_group_funcs *funcs);

/**
 * @brief Deallocate a specific group
 * @param [in] group: group to delete
 */
void s_group_free(struct s_group *group);

/**
 * @brief Add a service to a specific group
 * @param [in] group: group to modify
 * @param [in] data: service to store
 * @return 0 on success, an -errno value on error
 */
int s_group_add_service(struct s_group *group, struct s_service_data *data);

/**
 * @brief Remove a service from the group
 * @param [in] group: group to modify
 * @param [in] data: service to modify
 * @return 0 on success, an -errno value on error
 */
int s_group_remove_service(struct s_group *group, struct s_service_data *data);

/**
 * @brief Commit the group
 * @param [in] group: group to commit
 * @return 0 on success, an -errno value on error
 */
int s_group_commit(struct s_group *group);

#endif /* !_AVAHI_AVAHI_GROUP_H_ */
