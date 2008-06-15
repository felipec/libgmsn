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

#ifndef MSN_DEBUG_H
#define MSN_DEBUG_H

#include <glib.h>

typedef enum MsnType MsnType;

enum MsnType
{
    MSN_TYPE_CONN,
    MSN_TYPE_BUFFER,
    MSN_TYPE_STRING
};

void msn_print (const gchar *format, ...);

#endif /* MSN_DEBUG_H */
