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

#ifndef __MSN_CORE_H__
#define __MSN_CORE_H__

typedef struct MsnCore MsnCore;

#include "conn.h"

typedef void (*MsnHupCb) (MsnCore *core);

struct MsnCore
{
    ConnObject *ns;
    ConnObject *cs;
    ConnObject *ps;
    GKeyFile *config;

    gchar *username;
    gchar *password;

    gchar *cached_ns;
    gchar *config_file;
    gchar *cookie;

    MsnHupCb hup_cb;
};

MsnCore *msn_core_new ();
void msn_core_free (MsnCore *core);
void msn_core_init (MsnCore *core);
void msn_core_deinit (MsnCore *core);
void msn_core_start (MsnCore *core);
void msn_core_start_login (MsnCore *core, gchar *ticket);

void msn_core_got_ticket (MsnCore *core, gchar *ticket);
void msn_core_get_list (MsnCore *core);
void msn_core_save_conf (MsnCore *core);

#endif /* __MSN_CORE_H__ */
