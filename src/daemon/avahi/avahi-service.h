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

#ifndef _AVAHI_AVAHI_SERVICE_H_
# define _AVAHI_AVAHI_SERVICE_H_

# include <stdint.h>

struct s_service_data {
  char *data;
  char *domain;
  char *host;
  int interface;
  char *name;
  uint16_t port;
  int protocol;
  char *type;
};

/**
 * @brief Generate the service data to browse and publish process
 * @return a valid pointer on success, NULL on error
 */
struct s_service_data *s_service_generate(void);

/**
 * @brief Dellocate a specific service data
 * @param [in] data: data to delete
 */
void s_service_free(struct s_service_data *data);

/**
 * @brief Check the txt record of a service
 * @param [in] key: key to check
 * @return 0 if the key match, other value otherwise
 */
int s_service_check(const char *key);

#endif /* !_AVAHI_AVAHI_SERVICE_H_ */
