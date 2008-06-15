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

#ifndef __MSN_CS_H__
#define __MSN_CS_H__

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

#endif /* __MSN_CS_H__ */
