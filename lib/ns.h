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
