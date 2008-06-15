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

#include "ssl_end.h"

GType ssl_end_object_get_type (void);
static GObjectClass *parent_class = NULL;

void ssl_end_object_class_init (gpointer g_class, gpointer class_data);
void ssl_end_object_instance_init (GTypeInstance *instance, gpointer g_class);

static void
connect_impl (ConnEndObject *conn_end)
{
    SslEndObject *ssl_end;
    ssl_end = SSL_END_OBJECT (conn_end);
    ssl_end->ssl = msn_ssl_new ();
    msn_ssl_connect (ssl_end->ssl, conn_end->conn->fd);
}

static void
close_impl (ConnEndObject *conn_end)
{
}

static gint
read_impl (ConnEndObject *conn_end, gchar *data, guint size)
{
    SslEndObject *ssl_end;
    ssl_end = SSL_END_OBJECT (conn_end);
    return msn_ssl_read (ssl_end->ssl, data, size);
}

static gint
write_impl (ConnEndObject *conn_end, gchar *data, guint size)
{
    SslEndObject *ssl_end;
    ssl_end = SSL_END_OBJECT (conn_end);
    return msn_ssl_write (ssl_end->ssl, data, size);
}

SslEndObject *
ssl_end_object_new (ConnObject *conn)
{
    SslEndObject *ssl_end;
    ConnEndObject *conn_end;

    ssl_end = SSL_END_OBJECT (g_type_create_instance (SSL_END_OBJECT_TYPE));
    conn_end = CONN_END_OBJECT (ssl_end);

    conn_end->conn = conn;

    return ssl_end;
}

void
ssl_end_object_free (SslEndObject *conn)
{
    g_object_unref (G_OBJECT (conn));
}

static void
ssl_end_object_dispose (GObject *obj)
{
    SslEndObject *ssl_end = (SslEndObject *) obj;

    if (!ssl_end->dispose_has_run)
    {
        ssl_end->dispose_has_run = TRUE;
    }

    G_OBJECT_CLASS (parent_class)->dispose (obj);
}

static void
ssl_end_object_finalize (GObject *obj)
{
    /* Chain up to the parent class */
    G_OBJECT_CLASS (parent_class)->finalize (obj);
}

void
ssl_end_object_class_init (gpointer g_class, gpointer class_data)
{
    ConnEndObjectClass *conn_end_class = CONN_END_OBJECT_CLASS (g_class);
    GObjectClass *gobject_class = G_OBJECT_CLASS (g_class);

    conn_end_class->connect = &connect_impl;
    conn_end_class->close = &close_impl;
    conn_end_class->read = &read_impl;
    conn_end_class->write = &write_impl;

    gobject_class->dispose = ssl_end_object_dispose;
    gobject_class->finalize = ssl_end_object_finalize;

    parent_class = g_type_class_peek_parent (g_class);
}

void
ssl_end_object_instance_init (GTypeInstance *instance, gpointer g_class)
{
    SslEndObject *ssl_end = SSL_END_OBJECT (instance);

    ssl_end->dispose_has_run = FALSE;
}

GType
ssl_end_object_get_type (void)
{
    static GType type = 0;

    if (type == 0) 
    {
        static const GTypeInfo type_info = 
        {
            sizeof (SslEndObjectClass),
            NULL, /* base_init */
            NULL, /* base_finalize */
            ssl_end_object_class_init, /* class_init */
            NULL, /* class_finalize */
            NULL, /* class_data */
            sizeof (SslEndObject),
            0, /* n_preallocs */
            ssl_end_object_instance_init /* instance_init */
        };

        type = g_type_register_static
            (CONN_END_OBJECT_TYPE, "SslEndObjectType", &type_info, 0);
    }

    return	type;
}
