/*
 * Copyright (C) 2006-2008 Felipe Contreras.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation version 2.1 of
 * the License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#ifndef MSN_SSL_H
#define MSN_SSL_H

#include <glib.h>

#include <openssl/ssl.h>
#include <openssl/conf.h>
#include <openssl/err.h>
#include <openssl/rand.h>

typedef struct MsnSsl MsnSsl;

struct MsnSsl
{
    SSL_CTX *ctx;
    SSL *ssl;
};

gboolean msn_ssl_init ();
void msn_ssl_deinit ();
MsnSsl *msn_ssl_new ();
void msn_ssl_connect (MsnSsl *ssl, int fd);
void msn_ssl_free (MsnSsl *ssl);
size_t msn_ssl_read (MsnSsl *ssl, void *data, size_t len);
size_t msn_ssl_write (MsnSsl *ssl, const void *data, size_t len);

#endif /* MSN_SSL_H */
