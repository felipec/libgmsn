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

#include "ssl.h"

gboolean
msn_ssl_init ()
{
	char seed_data[1024];
	FILE *ram_dev;
	int read;

	/* ERR_load_crypto_strings (); */
	SSL_load_error_strings ();
	SSL_library_init ();

	ram_dev = fopen ("/dev/urandom", "rb");
	read = fread (seed_data, 1, 1024, ram_dev);
	fclose (ram_dev);
	RAND_seed (seed_data, read);

	return TRUE;
}

void
msn_ssl_deinit ()
{
	RAND_cleanup ();
	CONF_modules_unload (1);
	EVP_cleanup ();
	CRYPTO_cleanup_all_ex_data ();
	ERR_remove_state (0);
	ERR_free_strings ();
}

MsnSsl *
msn_ssl_new ()
{
	MsnSsl *ssl;
	ssl = g_new0 (MsnSsl, 1);

	ssl->ctx = SSL_CTX_new (SSLv23_client_method ());
	if (ssl->ctx == NULL)
	{
		char errmsg[128] = {0};
		ERR_error_string_n (ERR_get_error (), errmsg, 128);
		g_print ("error: %s\n", errmsg);
		msn_ssl_free (ssl);
		return NULL;
	}

	/* turn on all SSL implementation bug workarounds. */
	SSL_CTX_set_options (ssl->ctx, SSL_OP_ALL);

	/* don't do the verification. */
	SSL_CTX_set_verify (ssl->ctx, SSL_VERIFY_NONE, NULL);

	ssl->ssl = SSL_new (ssl->ctx);

	return ssl;
}

void
msn_ssl_connect (MsnSsl *ssl, int fd)
{
	int ret;

	SSL_set_fd (ssl->ssl, fd);
	SSL_set_connect_state (ssl->ssl);

	g_print ("ssl: Handshaking\n");
	ret = SSL_connect (ssl->ssl);

	if (ret < 0)
	{
		g_print ("ssl: Handshake failed\n");
		msn_ssl_free (ssl);
	}

	g_print ("ssl: Handshake done\n");
}

void
msn_ssl_free (MsnSsl *ssl)
{
	if (ssl->ssl)
	{
		SSL_shutdown (ssl->ssl);
		SSL_free (ssl->ssl);
	}

	if (ssl->ctx)
	{
		SSL_CTX_free (ssl->ctx);
	}

	g_free (ssl);
}

size_t
msn_ssl_read (MsnSsl *ssl,
			  void *data,
			  size_t len)
{
	int s;

	s = SSL_read (ssl->ssl, data, len);

	return s;
}

size_t
msn_ssl_write (MsnSsl *ssl,
			   const void *data,
			   size_t len)
{
	int s;

	s = SSL_write (ssl->ssl, data, len);

	return s;
}
