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

#ifndef __MSN_NS_H__
#define __MSN_NS_H__

typedef struct NsObject NsObject;
typedef struct NsObjectClass NsObjectClass;

#include "conn.h"

typedef void (*MsnCmdHandler) (ConnObject *conn, MsnCmd *in);

struct NsObject
{
    ConnObject parent_obj;

    MsnCmd *cmd;
    GHashTable *handlers;
    gboolean first_write;
};

struct NsObjectClass
{
    ConnObjectClass parent_class;
};

#define NS_OBJECT_TYPE (ns_object_get_type ())
#define NS_OBJECT(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), NS_OBJECT_TYPE, NsObject))
#define NS_OBJECT_CLASS(c) (G_TYPE_CHECK_CLASS_CAST ((c), NS_OBJECT_TYPE, NsObjectClass))
#define NS_IS_OBJECT(obj) (G_TYPE_CHECK_TYPE ((obj), NS_OBJECT_TYPE))
#define NS_IS_OBJECT_CLASS(c) (G_TYPE_CHECK_CLASS_TYPE ((c), NS_OBJECT_TYPE))
#define NS_OBJECT_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), NS_OBJECT_TYPE, NsObjectClass))

NsObject *ns_object_new (MsnCore *core);
void ns_object_free (NsObject *ns);

#endif /* __MSN_NS_H__ */
