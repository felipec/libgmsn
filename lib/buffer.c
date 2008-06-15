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

#include "buffer.h"

MsnBuffer *
msn_buffer_new ()
{
	MsnBuffer *buffer;
	buffer = g_new (MsnBuffer, 1);
	buffer->data = NULL;
	buffer->alloc_data = NULL;
	buffer->size = 0;
	buffer->filled = 0;
	return buffer;
}

MsnBuffer *
msn_buffer_new_and_alloc (guint size)
{
	MsnBuffer *buffer;

	if (size <= 0)
		size = MSN_BUF_SIZE;

	buffer = g_new (MsnBuffer, 1);
	buffer->data = buffer->alloc_data = g_malloc (size);
	buffer->size = size;
	buffer->filled = 0;
	return buffer;
}

void
msn_buffer_free (MsnBuffer *buffer)
{
	if (buffer == NULL)
		return;

	g_free (buffer->alloc_data);
	g_free (buffer);
}

void
msn_buffer_resize (MsnBuffer *buffer, guint new_size)
{
	new_size = ((new_size / MSN_BUF_SIZE) + 1) * MSN_BUF_SIZE;
	buffer->data = buffer->alloc_data = g_realloc (buffer->data, new_size);
	buffer->size = new_size;
}

void
msn_buffer_prepare (MsnBuffer *buffer, guint extra_size)
{
	if (buffer->size - buffer->filled < extra_size)
	{
		msn_buffer_resize (buffer, buffer->filled + extra_size);
	}
}
