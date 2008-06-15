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

#include "debug.h"

#include "buffer.h"
#include "conn.h"

void
msn_print (const gchar *format, ...)
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

                            g_print ("%s", tmp);

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

                            g_print ("%s", escaped);

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
