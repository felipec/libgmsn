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

#include "conn_end.h"

/* For read/write. */
#include <unistd.h>

static GObjectClass *parent_class = NULL;

/* ConnEndObject. */

ConnEndObject *
conn_end_object_new (ConnObject *conn)
{
	ConnEndObject *conn_end;

	conn_end = CONN_END_OBJECT (g_type_create_instance (CONN_END_OBJECT_TYPE));

	conn_end->conn = conn;

	return conn_end;
}

void
conn_end_object_free (ConnEndObject *conn)
{
	g_object_unref (G_OBJECT (conn));
}

void
conn_end_object_connect (ConnEndObject *conn_end)
{
	CONN_END_OBJECT_GET_CLASS (conn_end)->connect (conn_end);
}

void
conn_end_object_close (ConnEndObject *conn_end)
{
	CONN_END_OBJECT_GET_CLASS (conn_end)->close (conn_end);
}

gint
conn_end_object_read (ConnEndObject *conn_end, gchar *data, guint size)
{
	return CONN_END_OBJECT_GET_CLASS (conn_end)->read (conn_end, data, size);
}

gint
conn_end_object_write (ConnEndObject *conn_end, gchar *data, guint size)
{
	return CONN_END_OBJECT_GET_CLASS (conn_end)->write (conn_end, data, size);
}

/* ConnEndObject implementation. */

static void
connect_impl (ConnEndObject *conn_end)
{
}

static void
close_impl (ConnEndObject *conn_end)
{
}

static gint
read_impl (ConnEndObject *conn_end, gchar *data, guint size)
{
	return read (conn_end->conn->fd, data, size);
}

static gint
write_impl (ConnEndObject *conn_end, gchar *data, guint size)
{
	return write (conn_end->conn->fd, data, size);
}

/* GObject stuff. */

static void
conn_end_object_dispose (GObject *obj)
{
	ConnEndObject *conn_end = (ConnEndObject *) obj;

	if (!conn_end->dispose_has_run)
	{
		conn_end->dispose_has_run = TRUE;
	}

	G_OBJECT_CLASS (parent_class)->dispose (obj);
}

static void
conn_end_object_finalize (GObject *obj)
{
	/* Chain up to the parent class */
	G_OBJECT_CLASS (parent_class)->finalize (obj);
}

void
conn_end_object_class_init (gpointer g_class, gpointer class_data)
{
	ConnEndObjectClass *conn_end_class = CONN_END_OBJECT_CLASS (g_class);
	GObjectClass *gobject_class = G_OBJECT_CLASS (g_class);

	conn_end_class->connect = &connect_impl;
	conn_end_class->close = &close_impl;
	conn_end_class->read = &read_impl;
	conn_end_class->write = &write_impl;

	gobject_class->dispose = conn_end_object_dispose;
	gobject_class->finalize = conn_end_object_finalize;

	parent_class = g_type_class_peek_parent (g_class);
}

void
conn_end_object_instance_init (GTypeInstance *instance, gpointer g_class)
{
	ConnEndObject *conn_end = CONN_END_OBJECT (instance);

	conn_end->dispose_has_run = FALSE;
}

GType
conn_end_object_get_type (void)
{
	static GType type = 0;

	if (type == 0) 
	{
		static const GTypeInfo type_info =
		{
			sizeof (ConnEndObjectClass),
			NULL, /* base_init */
			NULL, /* base_finalize */
			conn_end_object_class_init, /* class_init */
			NULL, /* class_finalize */
			NULL, /* class_data */
			sizeof (ConnEndObject),
			0, /* n_preallocs */
			conn_end_object_instance_init /* instance_init */
    	};

		type = g_type_register_static
			(G_TYPE_OBJECT, "ConnEndObjectType", &type_info, 0);
	}

	return	type;
}
