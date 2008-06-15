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

#include "util.h"

#include <string.h>

inline static int
hexvalue (gchar c)
{
#if 0
    static const gchar *list = "0123456789ABCDEFabcdef";
    gchar *p;

    if (p = strchr (list, c))
    {
        int r;
        r = p - list;
        return (r < 16) ? r : r - 6;
    }

    return -1;
#else
    switch (c)
    {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            return c - '0'; break;
        case 'a':
        case 'A':
            return 10; break;
        case 'b':
        case 'B':
            return 11; break;
        case 'c':
        case 'C':
            return 12; break;
        case 'd':
        case 'D':
            return 13; break;
        case 'e':
        case 'E':
            return 14; break;
        case 'f':
        case 'F':
            return 15; break;
        default:
            return -1;
    }
#endif
}

gchar *
string_replace (const gchar *orig, gchar from, gchar to)
{
    gchar *new;
    gchar *cur;

    cur = new = g_strdup (orig);

    for (; *cur; cur++)
    {
        if (*cur == from)
        {
            *cur = to;
        }
    }

    return new;
}

gchar *
url_decode (const gchar *url)
{
    gchar *new;
    const gchar *src;
    gchar *dest;

    /*
       Thus, only alphanumerics, the special characters "$-_.+!*'(),", and
       reserved characters used for their reserved purposes may be used
       unencoded within a URL.
       */

    dest = new = g_malloc (strlen (url));
    src = url;

    while (*src != '\0')
    {
        if (*src == '%')
        {
            gint v1;
            gint v2;

#if 0
            v1 = hexvalue (src[1]);
            v2 = hexvalue (src[2]);
#else
            v1 = g_ascii_xdigit_value (src[1]);
            v2 = g_ascii_xdigit_value (src[2]);
#endif

            if ((v1 >= 0) && (v2 >= 0))
            {
                *dest = (v1 * 16) + v2;
            }
            else
            {
                /* Malformed */
                g_free (new);
                return NULL;
            }
            src += 3;
        }
        else
        {
            *dest = *src;
            src++;
        }
        dest++;
    }

    *dest = '\0';

    return new;
}

gchar *
xml_encode (gchar *orig)
{
    gchar *src;
    gchar *dest;
    gchar *new;
    gint extra = 0;

    /*
     * &amp; (&)
     * &lt; (<)
     * &gt; (>)
     * &apos; (')
     * &quot; (")
     */

    for (src = orig; *src; src++)
    {
        switch (*src)
        {
            case '&':
                extra += strlen ("&amp;") - 1; break;
            case '<':
                extra += strlen ("&lt;") - 1; break;
            case '>':
                extra += strlen ("&gt;") - 1; break;
            case '\'':
                extra += strlen ("&apos;") - 1; break;
            case '"':
                extra += strlen ("&quot;") - 1; break;
        }
    }

    g_print ("allocation %d bytes\n", src - orig + extra);
    dest = new = g_malloc (src - orig + extra);

    for (src = orig; *src; src++)
    {
        switch (*src)
        {
            case '&':
                memcpy (dest, "&amp;", strlen ("&amp;"));
                dest += strlen ("&amp;");
                break;
            case '<':
                memcpy (dest, "&lt;", strlen ("&lt;"));
                dest += strlen ("&lt;");
                break;
            case '>':
                memcpy (dest, "&gt;", strlen ("&gt;"));
                dest += strlen ("&gt;");
                break;
            case '\'':
                memcpy (dest, "&apos;", strlen ("&apos;"));
                dest += strlen ("&apos;");
                break;
            case '"':
                memcpy (dest, "&quot;", strlen ("&quot;"));
                dest += strlen ("&quot;");
                break;
            default:
                *dest = *src;
                dest++;
                break;
        }
    }

    g_print ("total %d bytes\n", dest - new);
    *dest = '\0';

    return new;
}

gchar *
xml_decode (gchar *orig)
{
    gchar *src;
    gchar *dest;
    gchar *new;

    /*
     * &amp; (&)
     * &lt; (<)
     * &gt; (>)
     * &apos; (')
     * &quot; (")
     */

    dest = new = g_strdup (orig);

    for (src = orig; *src;)
    {
        if (*src == '&')
        {
            if (strncmp (src + 1, "amp;", strlen ("amp;")) == 0)
            {
                *dest = '&';
                src += strlen ("&amp;");
                dest++;
            }
            else if (strncmp (src + 1, "lt;", strlen ("lt;")) == 0)
            {
                *dest = '<';
                src += strlen ("&lt;");
                dest++;
            }
            else if (strncmp (src + 1, "gt;", strlen ("gt;")) == 0)
            {
                *dest = '>';
                src += strlen ("&gt;");
                dest++;
            }
            else if (strncmp (src + 1, "apos;", strlen ("apos;")) == 0)
            {
                *dest = '\'';
                src += strlen ("&apos;");
                dest++;
            }
            else if (strncmp (src + 1, "quot;", strlen ("quot;")) == 0)
            {
                *dest = '"';
                src += strlen ("&quot;");
                dest++;
            }
            else
            {
                g_free (new);
                return NULL;
            }
        }
        else
        {
            *dest = *src;
            src++;
            dest++;
        }
    }

    *dest = '\0';

    return new;
}

#if 0
MsnString *
escape (MsnString *string)
{
    gchar *cur;
    gchar *new;
    gchar *dest;

    new = msn_string_new ();
    new = dest = g_strdup (string);

    for (cur = string->data; cur < (string->data + string->size); cur++, dest++)
    {
        switch (*cur)
        {
            case '\r':
            case '\n':
                {
                    *dest = 'X';
                    break;
                }
            default:
                {
                    *dest = *cur;
                    break;
                }
        }
    }

    *dest = '\0';

    return new;
}
#endif
