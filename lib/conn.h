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

#ifndef __MSN_CONN_H__
#define __MSN_CONN_H__

#include <glib.h>

#include "glib-object.h"

typedef struct MsnBuff MsnBuff;
typedef enum ConnObjectType ConnObjectType;

typedef struct ConnObject ConnObject;
typedef struct ConnObjectClass ConnObjectClass;

#include "core.h"
#include "cmd.h"
#include "buffer.h"
#include "conn_end.h"

#define SOCKET_ERROR -1

enum ConnObjectType
{
    MSN_CONN_NS,
    MSN_CONN_PASSPORT,
    MSN_CONN_CS
};

struct ConnObject
{
    GObject parent;
    MsnCore *core;
    gboolean dispose_has_run;
    ConnEndObject *end;

    ConnObjectType type;

    gchar *name;
    gchar *host;
    gint port;
    int fd;

    void (*error_cb) (ConnObject *conn);

    MsnBuffer *read_buffer;
    MsnBuffer *buffer;
    MsnBuffer *payload;
    void (*payload_cb) (ConnObject *conn, MsnBuffer *payload);
    gboolean parsed;
    guint parse_pos;
    guint last_parse_pos;
};

struct ConnObjectClass
{
    GObjectClass parent_class;

    void (*read) (ConnObject *conn);
    void (*parse) (ConnObject *conn);
    void (*write) (ConnObject *conn);
    void (*error) (ConnObject *conn);
    void (*connect) (ConnObject *conn);
};

#define CONN_OBJECT_TYPE (conn_object_get_type ())
#define CONN_OBJECT(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), CONN_OBJECT_TYPE, ConnObject))
#define CONN_OBJECT_CLASS(c) (G_TYPE_CHECK_CLASS_CAST ((c), CONN_OBJECT_TYPE, ConnObjectClass))
#define CONN_IS_OBJECT(obj) (G_TYPE_CHECK_TYPE ((obj), CONN_OBJECT_TYPE))
#define CONN_IS_OBJECT_CLASS(c) (G_TYPE_CHECK_CLASS_TYPE ((c), CONN_OBJECT_TYPE))
#define CONN_OBJECT_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), CONN_OBJECT_TYPE, ConnObjectClass))

GType conn_object_get_type ();
ConnObject *conn_object_new (MsnCore *core, gchar *name, ConnObjectType type);
void conn_object_free (ConnObject *conn);
void conn_object_connect (ConnObject *conn, gchar *hostname, gint port);
void conn_object_send_cmd (ConnObject *conn, MsnCmd *cmd);
void conn_object_close (ConnObject *conn);
void conn_object_handle (ConnObject *conn, MsnCmd *cmd);
gchar *conn_object_to_string (ConnObject *conn);
void conn_object_poll (ConnObject *conn);

#endif /* __MSN_CONN_H__ */
