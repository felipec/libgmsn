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

#include <glib.h>

#include <glib-object.h>

#include "core.h"
#include "ns.h"

GMainLoop* loop;

void
hup_cb (MsnCore *core)
{
	g_main_loop_quit (loop);
}

gboolean
idle (gpointer data)
{
	MsnCore *msn;
	msn = (MsnCore *) data;

	msn_core_start (msn);

	return FALSE;
}

int
main (int argc,
	  char* argv[])
{
	MsnCore *msn;

	g_type_init ();

	g_thread_init (NULL);

	loop = g_main_loop_new (NULL, FALSE);

	msn = msn_core_new ();
	msn_core_init (msn);
	msn->hup_cb = hup_cb;
	msn->username = g_strdup (argv[1]);
	msn->password = g_strdup (argv[2]);

	g_idle_add (idle, msn);
	g_main_loop_run (loop);

	msn_core_deinit (msn);
	msn_core_free (msn);

	return 0;
}
