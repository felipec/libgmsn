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

#include "debug.h"

#include "buffer.h"
#include "conn.h"

void
msn_print (const char *format, ...)
{
	va_list list;
	const gchar *cur;

	va_start (list, format);

	for (cur = format; *cur; cur++)
	{
		if (*cur == '%')
		{
			cur++;

			if (*cur == 'b')
			{
				MsnBuffer *buffer;
				gchar *tmp;
				gchar *escaped;

				buffer = va_arg (list, MsnBuffer *);

				tmp = g_strndup (buffer->data, buffer->filled);
				escaped = g_strescape (tmp, NULL);

				g_print ("%d:{%s}", buffer->filled, escaped);

				g_free (escaped);
				g_free (tmp);
			}

			if (*cur == 'v')
			{
				MsnType type;

				type = va_arg (list, MsnType);

				switch (type)
				{
					case MSN_TYPE_CONN:
						{
							ConnObject *conn;
							gchar *tmp;

							conn = va_arg (list, ConnObject *);

							tmp = conn_object_to_string (conn);

							g_print (tmp);

							g_free (tmp);
							break;
						}
					case MSN_TYPE_STRING:
						{
							gchar *str;
							gchar *tmp;
							gchar *escaped;
							guint size;

							str = va_arg (list, gchar *);
							size = va_arg (list, guint);

							tmp = g_strndup (str, size);
							escaped = g_strescape (tmp, NULL);

							g_print (escaped);

							g_free (escaped);
							g_free (tmp);
							break;
						}
					default:
						break;
				}
			}
		}
		else
		{
			g_print ("%c", *cur);
		}
	}

	va_end (list);
}
