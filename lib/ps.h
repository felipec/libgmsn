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

#ifndef MSN_PASSPORT_H
#define MSN_PASSPORT_H

typedef struct PsObject PsObject;
typedef struct PsObjectClass PsObjectClass;

#include "conn.h"

struct PsObject
{
    ConnObject parent_obj;

    gchar *ticket;
};

struct PsObjectClass
{
    ConnObjectClass parent_class;
};

#define PS_OBJECT_TYPE (ps_object_get_type ())
#define PS_OBJECT(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), PS_OBJECT_TYPE, PsObject))
#define PS_OBJECT_CLASS(c) (G_TYPE_CHECK_CLASS_CAST ((c), PS_OBJECT_TYPE, PsObjectClass))
#define PS_IS_OBJECT(obj) (G_TYPE_CHECK_TYPE ((obj), PS_OBJECT_TYPE))
#define PS_IS_OBJECT_CLASS(c) (G_TYPE_CHECK_CLASS_TYPE ((c), PS_OBJECT_TYPE))
#define PS_OBJECT_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), PS_OBJECT_TYPE, PsObjectClass))

GType ps_object_get_type ();
PsObject *ps_object_new (MsnCore *core);
void ps_object_free (PsObject *ns);

#endif /* MSN_PASSPORT_H */
