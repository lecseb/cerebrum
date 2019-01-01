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

#include <string.h>
#include <avahi-common/address.h>
#include <libdaemon/dlog.h>
#include "avahi-service.h"
#include "daemon-alloc.h"
#include "daemon-cond.h"

/**
 * @brief Key token used to identified cerebrum service description from the
 * rest
 */
const char *_g_service_key = "id=d6c4e9bcccdb8b16083036b45048dd52";

struct s_service_data *s_service_generate(void)
{
  struct s_service_data *data = daemon_malloc(sizeof(struct s_service_data));
  data->data = strdup(_g_service_key);
  data->domain = NULL;
  data->host = NULL;
  data->interface = AVAHI_IF_UNSPEC;
  data->name = strdup("cerebrum");
  data->port = 651;
  data->protocol = AVAHI_PROTO_INET;
  data->type = strdup("_http._tcp");
  return data;
}

void s_service_free(struct s_service_data *data)
{
  daemon_return_if_fail(data);

  if (data->name)
    daemon_free(data->name);
  if (data->type)
    daemon_free(data->type);
  if (data->domain)
    daemon_free(data->domain);
  if (data->host)
    daemon_free(data->host);
  if (data->data)
    daemon_free(data->data);
  daemon_free(data);
}

int s_service_check(const char *key)
{
  daemon_return_val_if_fail(key, -EINVAL);

  uint8_t size = strlen(key);
  uint8_t key_size = strlen(_g_service_key);

  return (size != key_size) ? 1 : strncmp(key, _g_service_key, size);
}
