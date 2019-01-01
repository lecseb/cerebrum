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

#ifndef _SSL_SSL_PACKET_H_
# define _SSL_SSL_PACKET_H_

# include <stdint.h>
# include "daemon-alloc.h"
# include "daemon-cond.h"

struct s_ssl_packet {
  uint8_t *payload;
  uint32_t size;
};

/**
 * @brief Allocate a ssl packet instance
 * @param [in] payload: data payload to store
 * @param [in] size: data's size to store
 * @return a valid pointer on success, NULL on error
 */
static inline struct s_ssl_packet *s_ssl_packet_new(const uint8_t *payload,
  uint32_t size)
{
  daemon_return_val_if_fail(payload, NULL);

  struct s_ssl_packet *packet = daemon_malloc(sizeof(struct s_ssl_packet));
  packet->payload = daemon_malloc(sizeof(uint8_t) * size);
  memcpy(packet->payload, payload, size);
  packet->size = size;
  return packet;
}

/**
 * @brief Deallocate a specific packet instance
 * @param [in] packet: packet to delete
 */
static inline void s_ssl_packet_free(struct s_ssl_packet *packet)
{
  daemon_return_if_fail(packet);

  daemon_free(packet->payload);
  daemon_free(packet);
}

#endif /* !_SSL_SSL_PACKET_H_ */
