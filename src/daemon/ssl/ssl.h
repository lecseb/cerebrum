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

#ifndef _SSL_SSL_H_
# define _SSL_SSL_H_

# include <libdaemon/dlog.h>
# include <openssl/conf.h>
# include <openssl/engine.h>
# include <openssl/err.h>
# include <openssl/rand.h>
# include <openssl/ssl.h>
# include <stdint.h>

# include "ssl-packet.h"

enum e_ssl_connection {
  e_ssl_connection_close,
  e_ssl_connection_connected,
  e_ssl_connection_timeout
};

enum e_ssl_error {
  e_ssl_error_connection,
  e_ssl_error_eof,
  e_ssl_error_read,
  e_ssl_error_write
};

/**
 * @brief Connection status callback
 * @param [in] userdata: userdata passing through the allocator
 * @param [in] state: current connection status
 */
typedef void (*s_ssl_connection_cbk)(void *userdata,
  enum e_ssl_connection state);

/**
 * @brief Error status callback, called whenever a read / write operation
 * failed
 * @param [in] userdata: userdata passing through the allocator
 * @param [in] type: error type definition
 * @param [in] packet: payload to send
 * @param [in] error: error received from ssl
 */
typedef void (*s_ssl_error_cbk)(void *userdata, enum e_ssl_error type,
  int error, const struct s_ssl_packet *packet);

/**
 * @brief Read callback, called whenever a packet is received
 * @param [in] userdata: userdata passing through the allocator
 * @param [in] packet: payload received
 */
typedef void (*s_ssl_read_cbk)(void *userdata,
  const struct s_ssl_packet *packet);

/**
 * @brief Ssl socket behavior callback
 */
struct s_ssl_funcs {
  s_ssl_connection_cbk connection;
  s_ssl_error_cbk error;
  s_ssl_read_cbk read;
};

/**
 * @brief Check the content of funcs
 * @param [in] funcs: behavior functions callback
 * @return 0 on success, an -errno value on error
 */
static inline int s_ssl_funcs_check(const struct s_ssl_funcs *funcs)
{
  daemon_return_val_if_fail(funcs, -EINVAL);
  daemon_return_val_if_fail(funcs->connection, -EBADE);
  daemon_return_val_if_fail(funcs->error, -EBADE);
  daemon_return_val_if_fail(funcs->read, -EBADE);
  return 0;
}

/**
 * @brief Create a client openssl context
 * @param certificate: certificate path file
 * @param private_key: private key path file
 * @return a valid pointer on success, NULL on error
 */
static inline SSL_CTX *s_ssl_context_server_new(const char *certificate,
  const char *private_key)
{
  daemon_return_val_if_fail(certificate, NULL);
  daemon_return_val_if_fail(private_key, NULL);

  SSL_load_error_strings();
  SSL_library_init();
  daemon_return_val_if_fail(RAND_poll(), NULL);

  SSL_CTX *context = SSL_CTX_new(SSLv23_server_method());
  daemon_return_val_if_fail(context, NULL);

  if (!SSL_CTX_use_certificate_chain_file(context, certificate) ||
      !SSL_CTX_use_PrivateKey_file(context, private_key, SSL_FILETYPE_PEM)) {
    daemon_log(LOG_ERR, "failed to initialize the ssl layer\n");
    SSL_CTX_free(context);
    return NULL;
  }
  SSL_CTX_set_options(context, SSL_OP_NO_SSLv2);
  return context;
}

/**
 * @brief Create a client openssl context
 * @param certificate: certificate path file
 * @return a valid pointer on success, NULL on error
 */
static inline SSL_CTX *s_ssl_context_client_new(const char *certificate)
{
  daemon_return_val_if_fail(certificate, NULL);

  SSL_load_error_strings();
  SSL_library_init();
  daemon_return_val_if_fail(RAND_poll(), NULL);

  SSL_CTX *context = SSL_CTX_new(SSLv23_client_method());
  daemon_return_val_if_fail(context, NULL);

  if (!SSL_CTX_use_certificate_chain_file(context, certificate)) {
    daemon_log(LOG_ERR, "failed to initialize the ssl layer\n");
    SSL_CTX_free(context);
    return NULL;
  }
  SSL_CTX_set_options(context, SSL_OP_NO_SSLv2);
  return context;
}

/**
 * @brief Deinit openssl/ssl/crypto properly to avoid leaks
 */
static inline void s_ssl_context_deinit(void)
{
  ERR_remove_state(0);
  FIPS_mode_set(0);
  CRYPTO_set_locking_callback(NULL);
  CRYPTO_set_id_callback(NULL);
  ENGINE_cleanup();
  CONF_modules_unload(1);
  ERR_free_strings();
  EVP_cleanup();
  CRYPTO_cleanup_all_ex_data();
}

#endif /* !_SSL_SSL_H_ */
