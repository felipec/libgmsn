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

#include "core.h"

/* For open. */
#include <unistd.h>
#if 0
#include <sys/types.h>
#include <sys/stat.h>
#endif
#include <fcntl.h>

gpointer core_thread (gpointer data);

#define MSN_DEFAULT_NS "messenger.hotmail.com"
#define MSN_DEFAULT_NS_PORT 1863

#include "ps.h"
#include "ns.h"
#include "cs.h"

#include "ssl.h"

MsnCore *
msn_core_new ()
{
	MsnCore *core;

	core = g_new0 (MsnCore, 1);
	core->ns = CONN_OBJECT (ns_object_new (core));
	core->ps = CONN_OBJECT (ps_object_new (core));
	core->cs = CONN_OBJECT (cs_object_new (core));
	core->config = g_key_file_new ();

	return core;
}

void
msn_core_free (MsnCore *core)
{
	g_free (core->config_file);
	g_free (core->cached_ns);
	g_free (core->cookie);
	g_free (core->password);
	g_free (core->username);

	g_key_file_free (core->config);
	conn_object_free (core->cs);
	conn_object_free (core->ps);
	conn_object_free (core->ns);

	g_free (core);
}

void
msn_core_init (MsnCore *core)
{
	core->config_file = g_build_filename (g_get_user_config_dir (), "libmsn", NULL);
	g_key_file_load_from_file (core->config, core->config_file, G_KEY_FILE_NONE, NULL);
	core->cached_ns = g_key_file_get_string (core->config, "general", "ns", NULL);

	msn_ssl_init ();
}

void
msn_core_start (MsnCore *core)
{
	g_thread_create (core_thread, core, FALSE, NULL);
}

void
msn_core_deinit (MsnCore *core)
{
	msn_ssl_deinit ();
}

static void
error_cb (ConnObject *conn)
{
	if (strcmp (conn->host, MSN_DEFAULT_NS) == 0)
	{
		g_error ("Can't connect\n");
		conn->core->hup_cb (conn->core);
		return;
	}

	conn_object_connect (conn, MSN_DEFAULT_NS, MSN_DEFAULT_NS_PORT);
}

gpointer
core_thread (gpointer data)
{
	MsnCore *core;

	core = (MsnCore *) data;

	core->ns->error_cb = error_cb;

	conn_object_connect (core->ns, core->cached_ns ? core->cached_ns : MSN_DEFAULT_NS, MSN_DEFAULT_NS_PORT);

	while (TRUE)
	{
		conn_object_poll (core->ns);
		if (core->ns->fd == -1)
		{
			g_debug ("core thread finished\n");
			return FALSE;
		}
		g_usleep (3 * 1000);
	}

	return FALSE;
}

void
msn_core_save_conf (MsnCore *core)
{
	gchar *buf;
	guint len;
	gint fd;

	buf = g_key_file_to_data (core->config, &len, NULL);

	fd = open (core->config_file, O_WRONLY | O_CREAT);
	if (fd < 0)
	{
		g_free (buf);
		return;
	}

	write (fd, buf, len);

	g_free (buf);

	close (fd);
}

void
msn_core_got_ticket (MsnCore *core, gchar *ticket)
{
	MsnCmd *cmd;
	gchar *buf;

	buf = g_strdup_printf ("TWN S %s", ticket);

	cmd = msn_cmd_new_full ("USR", 4, buf);
	conn_object_send_cmd (core->ns, cmd);
	msn_cmd_free (cmd);

	g_free (buf);
}

gpointer
login_thread (gpointer data)
{
	MsnCore *core;

	core = (MsnCore *) data;

	conn_object_connect (core->ps, "loginnet.passport.com", 443);

	while (TRUE)
	{
		conn_object_poll (core->ps);
		g_usleep (3 * 1000);
	}

	return FALSE;
}

gpointer
cs_thread (gpointer data)
{
	MsnCore *core;

	core = (MsnCore *) data;

	conn_object_connect (core->cs, "contacts.msn.com", 80);

	while (TRUE)
	{
		conn_object_poll (core->cs);
		g_usleep (3 * 1000);
	}

	return FALSE;
}

void
msn_core_start_login (MsnCore *core, gchar *ticket)
{
	PS_OBJECT (core->ps)->ticket = g_strdup (ticket);
	g_thread_create (login_thread, core, TRUE, NULL);
}

void
msn_core_get_list (MsnCore *core)
{
	g_thread_create (cs_thread, core, TRUE, NULL);
}
