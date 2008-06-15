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

#ifndef __MSN_UTIL_H__
#define __MSN_UTIL_H__

#include <glib.h>

gchar *url_decode (const gchar *url);
gchar *string_replace (const gchar *orig, gchar from, gchar to);
gchar *xml_encode (gchar *orig);
gchar *xml_decode (gchar *orig);

#endif /* __MSN_UTIL_H__ */
