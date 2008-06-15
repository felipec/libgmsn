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

#ifndef MSN_CONN_END_H
#define MSN_CONN_END_H

#include <glib.h>

#include "glib-object.h"

typedef struct ConnEndObject ConnEndObject;
typedef struct ConnEndObjectClass ConnEndObjectClass;

#include "conn.h"

struct ConnEndObject
{
    GObject parent;
    gboolean dispose_has_run;

    ConnObject *conn;
};

struct ConnEndObjectClass
{
    GObjectClass parent_class;

    void (*connect) (ConnEndObject *end);
    void (*close) (ConnEndObject *end);
    void (*free) (ConnEndObject *end);
    gint (*read) (ConnEndObject *end, gchar *data, guint size);
    gint (*write) (ConnEndObject *end, gchar *data, guint size);
};

#define CONN_END_OBJECT_TYPE (conn_end_object_get_type ())
#define CONN_END_OBJECT(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), CONN_END_OBJECT_TYPE, ConnEndObject))
#define CONN_END_OBJECT_CLASS(c) (G_TYPE_CHECK_CLASS_CAST ((c), CONN_END_OBJECT_TYPE, ConnEndObjectClass))
#define CONN_IS_END_OBJECT(obj) (G_TYPE_CHECK_TYPE ((obj), CONN_END_OBJECT_TYPE))
#define CONN_IS_END_OBJECT_CLASS(c) (G_TYPE_CHECK_CLASS_TYPE ((c), CONN_END_OBJECT_TYPE))
#define CONN_END_OBJECT_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), CONN_END_OBJECT_TYPE, ConnEndObjectClass))

GType conn_end_object_get_type ();
ConnEndObject *conn_end_object_new (ConnObject *conn_end);
void conn_end_object_free (ConnEndObject *conn_end);
void conn_end_object_connect (ConnEndObject *conn_end);
void conn_end_object_close (ConnEndObject *conn_end);
gint conn_end_object_read (ConnEndObject *conn_end, gchar *data, guint size);
gint conn_end_object_write (ConnEndObject *conn_end, gchar *data, guint size);

#endif /* MSN_CONN_END_H */
