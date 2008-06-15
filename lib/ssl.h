/*
 * Copyright © 2006 Felipe Contreras.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef __MSN_SSL_H__
#define __MSN_SSL_H__

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

#endif /* __MSN_SSL_H__ */
