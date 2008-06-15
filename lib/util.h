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

#ifndef __MSN_UTIL_H__
#define __MSN_UTIL_H__

#include <glib.h>

gchar *url_decode (const gchar *url);
gchar *string_replace (const gchar *orig, gchar from, gchar to);
gchar *xml_encode (gchar *orig);
gchar *xml_decode (gchar *orig);

#endif /* __MSN_UTIL_H__ */
