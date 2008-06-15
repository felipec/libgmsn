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

#ifndef MSN_CS_H
#define MSN_CS_H

typedef struct CsObject CsObject;
typedef struct CsObjectClass CsObjectClass;

#include "conn.h"

struct CsObject
{
    ConnObject parent_obj;
};

struct CsObjectClass
{
    ConnObjectClass parent_class;
};

#define CS_OBJECT_TYPE (cs_object_get_type ())
#define CS_OBJECT(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), CS_OBJECT_TYPE, CsObject))
#define CS_OBJECT_CLASS(c) (G_TYPE_CHECK_CLASS_CAST ((c), CS_OBJECT_TYPE, CsObjectClass))
#define CS_IS_OBJECT(obj) (G_TYPE_CHECK_TYPE ((obj), CS_OBJECT_TYPE))
#define CS_IS_OBJECT_CLASS(c) (G_TYPE_CHECK_CLASS_TYPE ((c), CS_OBJECT_TYPE))
#define CS_OBJECT_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), CS_OBJECT_TYPE, CsObjectClass))

CsObject *cs_object_new (MsnCore *core);
void cs_object_free (CsObject *ns);

void cs_object_deinit ();

#endif /* MSN_CS_H */
