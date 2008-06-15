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

#include "conn.h"

#include <netdb.h>
#include <string.h>

#include <sys/poll.h>

#include "debug.h"

/* #define FILE_TEST */

/* For open. */
#include <fcntl.h>
#include <unistd.h>

static GObjectClass *parent_class = NULL;

ConnObject *
conn_object_new (MsnCore *core, gchar *name, ConnObjectType type)
{
	ConnObject *conn;

	conn = CONN_OBJECT (g_type_create_instance (CONN_OBJECT_TYPE));

	conn->core = core;
	conn->name = g_strdup (name);
	conn->type = type;

	return conn;
}

void
conn_object_free (ConnObject *conn)
{
	g_object_unref (G_OBJECT (conn));
}

void
conn_object_send_cmd (ConnObject *conn, MsnCmd *cmd)
{
	gchar *buf;
	int r;

	buf = g_strdup_printf ("%s %d %s\r\n", cmd->id, cmd->trid, cmd->args);

#ifndef FILE_TEST
	r = conn_end_object_write (conn->end, buf, strlen (buf));
#endif

	msn_print (">> %v\n", MSN_TYPE_STRING, buf, r);

	g_free (buf);
}

gchar *
conn_object_to_string (ConnObject *conn)
{
	return g_strdup_printf ("%s (%s:%d)", conn->name, conn->host, conn->port);
}

void
conn_object_error (ConnObject *conn)
{
	CONN_OBJECT_GET_CLASS (conn)->error (conn);
}

void
conn_object_poll (ConnObject *conn)
{
	struct pollfd ufds[1];
	int r;

	ufds[0].fd = conn->fd;
	ufds[0].events = POLLIN | POLLOUT;

	r = poll (ufds, 1, 1000);

	if (ufds[0].revents & POLLIN)
	{
		CONN_OBJECT_GET_CLASS (conn)->read (conn);
	}

	if (ufds[0].revents & POLLOUT)
	{
		CONN_OBJECT_GET_CLASS (conn)->write (conn);
	}

	if (ufds[0].revents & POLLERR)
	{
		g_debug ("error\n");
	}

	if (ufds[0].revents & POLLHUP)
	{
		g_debug ("hup\n");
	}
}

void
conn_object_connect (ConnObject *conn,
					 gchar *hostname,
					 gint port)
{
#ifndef FILE_TEST
	int fd;
	struct hostent* host;
	struct sockaddr_in address;
	long host_address;

	conn->fd = fd = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (fd == SOCKET_ERROR)
	{
		g_error ("Could not make a socket\n");
		conn_object_error (conn);
		return;
	}

	conn->host = g_strdup (hostname);
	conn->port = port;

	host = gethostbyname (hostname);

	if (!host)
	{
		g_error ("Bad hostname: %s\n", hostname);
		conn_object_error (conn);
		return;
	}

	memcpy (&host_address, host->h_addr, host->h_length);

	address.sin_addr.s_addr = host_address;
	address.sin_port = htons (port);
	address.sin_family = AF_INET;

	g_message ("Connecting to %s on port %d\n", hostname, port);

	if (connect (fd, (struct sockaddr*) & address, sizeof (address)) == SOCKET_ERROR)
	{
		g_error ("Could not connect to host\n");
		conn_object_error (conn);
		return;
	}

	g_message ("Connected\n");

	conn_end_object_connect (conn->end);

	CONN_OBJECT_GET_CLASS (conn)->connect (conn);
#else
	conn->fd = open ("tests/test_0.txt", O_RDONLY);
#endif
}

void
conn_object_close (ConnObject *conn)
{
	msn_print ("connection closed: %v\n", MSN_TYPE_CONN, conn);

	close (conn->fd);
	conn->fd = -1;

	if (conn->type == MSN_CONN_NS)
	{
		if (conn->core->hup_cb)
		{
			conn->core->hup_cb (conn->core);
		}
	}
}

/* ConnObject stuff. */

static void
read_impl (ConnObject *conn)
{
	MsnBuffer *read_buffer;
	int r;

	read_buffer = conn->read_buffer;

	read_buffer->size = MSN_BUF_SIZE;

	if (conn->payload)
	{
		msn_buffer_prepare (conn->buffer, conn->payload->size);
	}
	else
	{
		msn_buffer_prepare (conn->buffer, read_buffer->size);
	}

	read_buffer->data = conn->buffer->data + conn->buffer->filled;

	r = conn_end_object_read (conn->end, read_buffer->data, read_buffer->size);

	if (r == 0)
	{
		/* connection closed */
		conn_object_close (conn);
		return;
	}

	if (r < 0)
	{
		/* connection error */
		conn_object_close (conn);
		return;
	}

	read_buffer->filled = r;
	/* msn_print ("read [%b]\n", read_buffer); */

	conn->buffer->filled += read_buffer->filled;

	while (conn->parse_pos < conn->buffer->filled)
	{
		if (conn->payload)
		{
			guint size;
			size = MIN (conn->payload->size - conn->payload->filled,
						conn->buffer->filled - conn->parse_pos);

			conn->payload->filled += size;
			conn->parse_pos += size;

			if (conn->payload->filled == conn->payload->size)
			{
				if (conn->payload_cb)
				{
					conn->payload->data = conn->buffer->data + conn->last_parse_pos;
					conn->payload_cb (conn, conn->payload);
				}
				msn_buffer_free (conn->payload);
				conn->payload = NULL;
				conn->parsed = TRUE;
				conn->last_parse_pos = conn->parse_pos;
			}
		}
		else
		{
			CONN_OBJECT_GET_CLASS (conn)->parse (conn);
		}

		/** @todo only if parsed? yes indeed! */
		if (conn->parsed)
		{
			if (conn->parse_pos == conn->buffer->filled)
			{
				/* g_debug ("reset\n"); */
				conn->buffer->filled = 0;
				conn->parse_pos = 0;
				conn->last_parse_pos = 0;
			}

			conn->parsed = FALSE;
		}
	}
}

static void
connect_impl (ConnObject *conn)
{
}

static void
error_impl (ConnObject *conn)
{
}

/* GObject stuff. */

static void
conn_object_dispose (GObject *obj)
{
	ConnObject *conn = (ConnObject *) obj;

	if (!conn->dispose_has_run)
	{
		conn->dispose_has_run = TRUE;
	}

	G_OBJECT_CLASS (parent_class)->dispose (obj);
}

static void
conn_object_finalize (GObject *obj)
{
	ConnObject *conn = (ConnObject *) obj;

	conn_end_object_free (conn->end);

	msn_buffer_free (conn->read_buffer);
	msn_buffer_free (conn->buffer);

	g_free (conn->name);
	g_free (conn->host);

	/* Chain up to the parent class */
	G_OBJECT_CLASS (parent_class)->finalize (obj);
}

void
conn_object_class_init (gpointer g_class, gpointer class_data)
{
	ConnObjectClass *conn_class = CONN_OBJECT_CLASS (g_class);
	GObjectClass *gobject_class = G_OBJECT_CLASS (g_class);

	conn_class->connect = &connect_impl;
	conn_class->error = &error_impl;
	conn_class->read = &read_impl;

	gobject_class->dispose = conn_object_dispose;
	gobject_class->finalize = conn_object_finalize;

	parent_class = g_type_class_peek_parent (g_class);
}

void
conn_object_instance_init (GTypeInstance *instance, gpointer g_class)
{
	ConnObject *conn = CONN_OBJECT (instance);

	conn->end = conn_end_object_new (conn);
	conn->dispose_has_run = FALSE;
	conn->buffer = msn_buffer_new_and_alloc (0);
	conn->read_buffer = msn_buffer_new ();
}

GType
conn_object_get_type (void)
{
	static GType type = 0;

	if (type == 0) 
	{
		static const GTypeInfo type_info =
		{
			sizeof (ConnObjectClass),
			NULL, /* base_init */
			NULL, /* base_finalize */
			conn_object_class_init, /* class_init */
			NULL, /* class_finalize */
			NULL, /* class_data */
			sizeof (ConnObject),
			0, /* n_preallocs */
			conn_object_instance_init /* instance_init */
    	};

		type = g_type_register_static
			(G_TYPE_OBJECT, "ConnObjectType", &type_info, 0);
	}

	return	type;
}
