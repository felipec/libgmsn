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

#ifndef __MSN_BUFFER_H__
#define __MSN_BUFFER_H__

#include <glib.h>

typedef struct MsnBuffer MsnBuffer;

#define MSN_BUF_SIZE 0x1000

struct MsnBuffer
{
	gchar *data;
	gchar *alloc_data;
	guint size;
	guint filled;
};

MsnBuffer *msn_buffer_new ();
MsnBuffer *msn_buffer_new_and_alloc (guint size);
void msn_buffer_free (MsnBuffer *);
void msn_buffer_resize (MsnBuffer *buffer, guint new_size);
gchar *msn_buffer_to_string (MsnBuffer *buffer);
void msn_buffer_prepare (MsnBuffer *buffer, guint extra_size);

#endif /* __MSN_BUFFER_H__ */
