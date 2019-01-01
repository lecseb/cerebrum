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

#ifndef _AVAHI_AVAHI_CLIENT_H_
# define _AVAHI_AVAHI_CLIENT_H_

# include <avahi-client/client.h>
# include "avahi-service.h"

struct s_client;

/**
 * @brief Call after initialization when everything is done
 * @param [in] userdata: userdata passing through the allocation
 */
typedef void (*s_client_running_cbk)(void *userdata);

/**
 * @brief Call when the group name is already used and an alternative name
 * is needed. Allow the plugin to notify the change and apply it internaly
 * @param [in] userdata: userdata passing through the allocation
 */
typedef void (*s_client_collision_cbk)(void *userdata);

/**
 * @brief Call when an error occured.
 * @param [in] userdata: userdata passing through the allocation
 * @param [in] error: the errno value
 */
typedef void (*s_client_failure_cbk)(void *userdata, int error);

struct s_client_funcs {
  s_client_collision_cbk collision;
  s_client_failure_cbk failure;
  s_client_running_cbk running;
};

/**
 * @brief Allocate a new client
 * @param [in] poll: poll avahi instance
 * @param [in] userdata: user pointer
 * @param [in] funcs: behavior function
 * @return a valid pointer on success, NULL on error
 */
struct s_client *s_client_new(const AvahiPoll *poll, void *userdata,
  const struct s_client_funcs *funcs);

/**
 * @brief Deallocate a specific client
 * @param [in] client: client to delete
 */
void s_client_free(struct s_client *client);

/**
 * @brief Start the client process
 * @param [in] client: client to start
 * @return 0 on success, an -errno value on error
 */
int s_client_run(struct s_client *client);

/**
 * @brief Get the avahi client interface
 * @param [in] client: client structure to browse
 * @return a valid pointer on success, NULL on error
 */
AvahiClient *s_client_toavahi(struct s_client *client);

#endif /* !_AVAHI_AVAHI_CLIENT_H_ */
