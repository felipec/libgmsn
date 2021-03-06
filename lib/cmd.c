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

#include "cmd.h"

#include <string.h>

MsnCmd *
msn_cmd_new (void)
{
    MsnCmd *cmd;
    cmd = g_new0 (MsnCmd, 1);
    return cmd;
}

MsnCmd *
msn_cmd_new_full (const gchar *id, gint trid, const gchar *args)
{
    MsnCmd *cmd;
    cmd = msn_cmd_new ();
    cmd->id = g_strdup (id);
    cmd->trid = trid;
    cmd->args = g_strdup (args);
    return cmd;
}

void
msn_cmd_free (MsnCmd *cmd)
{
    if (cmd == NULL)
        return;

    g_free (cmd->id);
    g_free (cmd->args);
    g_free (cmd);
}

void
msn_cmd_print (MsnCmd *cmd)
{
    g_print ("%s %d %s\n", cmd->id, cmd->trid, cmd->args);
}

/* not used */
#if 0
static gboolean
is_num (gchar *str)
{
    if (str[0] >= '0' && str[0] <= '9')
        return TRUE;

    return FALSE;
}
#endif

MsnCmd *
msn_cmd_from_string (gchar *string)
{
    MsnCmd *cmd;

    cmd = g_new0 (MsnCmd, 1);

    cmd->str = g_strdup (string);

    {
        gchar *cur;
        gchar *old;

        old = cur = cmd->str;

        for (; *cur && *cur != ' '; cur++);
        cmd->id = g_strndup (old, cur - old);
        cmd->buffer_cur = ++cur;
    }

    return cmd;
}

gchar *
msn_cmd_get_param (MsnCmd *cmd, gint num)
{
#if 0
    gchar *cur;
    gchar *last;
    gint count;

    last = cur = cmd->buffer_cur;
    count = 0;

    while (*cur && *cur != '\r' && *cur != '\n')
    {
        if (*cur == ' ')
        {
            count++;
            if (count > num - 1)
            {
                return g_strndup (last, cur - last);
            }
            last = cur + 1;
        }
        cur++;
    }

    if (count >= num - 1)
    {
        return g_strndup (last, cur - last);
    }
#else
    return cmd->argv[num];
#endif

    return NULL;
}
