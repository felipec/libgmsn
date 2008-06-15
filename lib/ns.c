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

#include "ns.h"

#include <string.h>

/* For atoi. */
#include <stdlib.h>

#include "debug.h"

GType ns_object_get_type (void);
static GObjectClass *parent_class = NULL;

NsObject *
ns_object_new (MsnCore *core)
{
	NsObject *ns;
	ConnObject *conn;

	ns = NS_OBJECT (g_type_create_instance (NS_OBJECT_TYPE));

	conn = CONN_OBJECT (ns);

	conn->core = core;

	return ns;
}

void
ns_object_free (NsObject *ns)
{
	g_object_unref (G_OBJECT (ns));
}

void
ns_object_handle (NsObject *ns, MsnCmd *cmd)
{
	MsnCmdHandler handler;

	handler = g_hash_table_lookup (ns->handlers, cmd->id);

	if (handler != NULL)
	{
		handler (CONN_OBJECT (ns), cmd);
	}
	else
	{
		g_warning ("Bad handler (%s)\n", cmd->id);
	}
}

/* Commands. */

static void
ver_cmd (ConnObject *conn, MsnCmd *in)
{
	MsnCmd *cmd;
	gchar *buf;

	buf = g_strdup_printf ("0x0409 winnt 5.1 i386 MSNMSGR 8.0.0566 msmsgs %s",
						   conn->core->username);

	cmd = msn_cmd_new_full ("CVR", 2, buf);
	conn_object_send_cmd (conn, cmd);
	msn_cmd_free (cmd);

	g_free (buf);
}

static void
cvr_cmd (ConnObject *conn, MsnCmd *in)
{
	MsnCmd *cmd;
	gchar *buf;

	buf = g_strdup_printf ("TWN I %s", conn->core->username);

	cmd = msn_cmd_new_full ("USR", 3, buf);
	conn_object_send_cmd (conn, cmd);
	msn_cmd_free (cmd);

	g_free (buf);
}

static void
msg_cmd_payload (ConnObject *conn, MsnBuffer *payload)
{
	gchar *start;
	start = g_strstr_len (payload->data, payload->size, "MSPAuth: ");
	if (start)
	{
		gchar *end;
		start += strlen ("MSPAuth: ");
		end = g_strstr_len (start, payload->size - (payload->data - start),  "\r\n");
		if (end)
		{
			/* g_print ("found it\n"); */
			conn->core->cookie = g_strndup (start, end - start);
			g_print ("{%s}\n", conn->core->cookie);
			msn_core_get_list (conn->core);
		}
	}
}

static void
msg_cmd (ConnObject *conn, MsnCmd *in)
{
	guint size;
	size = atoi (msn_cmd_get_param (in, 3));
	conn->payload = msn_buffer_new ();
	conn->payload->size = size;
	conn->payload_cb = msg_cmd_payload;
}

static void
xfr_cmd (ConnObject *conn, MsnCmd *in)
{
	gchar *copy;
	gchar *c;
	gchar *host;
	gchar *port_str;
	gint port;
	NsObject *ns = NS_OBJECT (conn);

	host = copy = g_strdup (msn_cmd_get_param (in, 3));
	c = strchr (copy, ':');
	*c = 0;
	port_str = c + 1;
	port = atoi (port_str);

	g_debug ("caching ns host: %s\n", host);
	g_key_file_set_string (conn->core->config, "general", "ns", host);
	msn_core_save_conf (conn->core);

	ns->first_write = FALSE;
	conn_object_connect (conn, host, port);

	g_free (copy);
}

static void
usr_cmd (ConnObject *conn, MsnCmd *in)
{
	gchar *r;

	r = msn_cmd_get_param (in, 2);

	if (strcmp (r, "OK") == 0)
	{
	}
	else if (strcmp (r, "TWN") == 0)
	{
		msn_core_start_login (conn->core, msn_cmd_get_param (in, 4));
	}
}

static void
gcf_cmd_payload (ConnObject *conn, MsnBuffer *payload)
{
	msn_print ("payload: [%b]\n", payload);
}

static void
gcf_cmd (ConnObject *conn, MsnCmd *in)
{
	guint size;
	size = atoi (msn_cmd_get_param (in, 2));
	conn->payload = msn_buffer_new ();
	conn->payload->size = size;
	conn->payload_cb = gcf_cmd_payload;
}

/* ConnObject implementation. */

static void
parse_impl (ConnObject *conn)
{
	NsObject *ns;

	ns = NS_OBJECT (conn);

	if (ns->cmd == NULL)
	{
		ns->cmd = msn_cmd_new ();
	}

	{
		static guint br = 0;
		static guint st = 0;
		static guint last_arg_pos = 0;

		gchar *cur;
		for (cur = conn->buffer->data + conn->parse_pos;
			 conn->parse_pos < conn->buffer->filled;
			 cur++, conn->parse_pos++)
		{
			if (st == 0)
			{
				switch (*cur)
				{
					case '\r':
					case '\n':
					case ' ':
						{
							ns->cmd->id = g_strndup (conn->buffer->data + conn->last_parse_pos,
													 conn->parse_pos - conn->last_parse_pos);
							last_arg_pos = conn->parse_pos + 1;
#if 0
							/** @todo This isn't good. */
							ns->cmd->buffer_cur = cur + 1;
#endif

							st = 1;
							break;
						}
					default:
						break;
				}
			}
			else
			{
				switch (*cur)
				{
					case '\r':
					case ' ':
						{
							ns->cmd->argv[st] = g_strndup (conn->buffer->data + last_arg_pos,
														   conn->parse_pos - last_arg_pos);
							/* g_print ("got arg [%s]\n", ns->cmd->argv[st]); */
							last_arg_pos = conn->parse_pos + 1;
							st++;
							break;
						}
					default:
						break;
				}
			}
			if (br == 0 && *cur == '\r')
			{
				br = 1;
			}
			if (br == 1 && *cur == '\n')
			{
				/* Reset. */
				br = 0;
				st = 0;

				conn->parse_pos++;

				msn_print ("<< %v\n",
						   MSN_TYPE_STRING,
						   conn->buffer->data + conn->last_parse_pos,
						   conn->parse_pos - conn->last_parse_pos);

				ns_object_handle (ns, ns->cmd);
				msn_cmd_free (ns->cmd);
				ns->cmd = NULL;

				conn->parsed = TRUE;
				conn->last_parse_pos = conn->parse_pos;

				return;
			}
		}
	}
}

static void
write_impl (ConnObject *conn)
{
	NsObject *ns = NS_OBJECT (conn);

	if (!ns->first_write)
	{
		MsnCmd *cmd;
		cmd = msn_cmd_new_full ("VER", 1, "MSNP13");
		conn_object_send_cmd (conn, cmd);
		msn_cmd_free (cmd);
		ns->first_write = TRUE;
	}
}

/* GObject stuff. */

static void
ns_object_dispose (GObject *obj)
{
	ConnObject *conn = (ConnObject *) obj;

	if (conn->dispose_has_run)
	{
		/* If dispose did already run, return. */
		return;
	}

	/* Make sure dispose does not run twice. */
	conn->dispose_has_run = TRUE;

	G_OBJECT_CLASS (parent_class)->dispose (obj);
}

static void
ns_object_finalize (GObject *obj)
{
	NsObject *ns = NS_OBJECT (obj);

	msn_cmd_free (ns->cmd);

	g_hash_table_destroy (ns->handlers);

	/* Chain up to the parent class */
	G_OBJECT_CLASS (parent_class)->finalize (obj);
}

void
ns_object_class_init (gpointer g_class, gpointer class_data)
{
	ConnObjectClass *conn_class = CONN_OBJECT_CLASS (g_class);
	GObjectClass *gobject_class = G_OBJECT_CLASS (g_class);

	conn_class->parse = &parse_impl;
	conn_class->write = &write_impl;

	gobject_class->dispose = ns_object_dispose;
	gobject_class->finalize = ns_object_finalize;

	parent_class = g_type_class_peek_parent (g_class);
}

void
ns_object_instance_init (GTypeInstance *instance, gpointer g_class)
{
	NsObject *ns = NS_OBJECT (instance);
	ConnObject *conn = CONN_OBJECT (instance);

	ns->handlers = g_hash_table_new (g_str_hash, g_str_equal);
	ns->cmd = NULL;

	conn->name = g_strdup ("ns");
	conn->type = MSN_CONN_NS;

	g_hash_table_insert (ns->handlers, "VER", ver_cmd);
	g_hash_table_insert (ns->handlers, "CVR", cvr_cmd);
	g_hash_table_insert (ns->handlers, "XFR", xfr_cmd);
	g_hash_table_insert (ns->handlers, "GCF", gcf_cmd);
	g_hash_table_insert (ns->handlers, "USR", usr_cmd);
	g_hash_table_insert (ns->handlers, "MSG", msg_cmd);
}

GType
ns_object_get_type (void)
{
	static GType type = 0;

	if (type == 0) 
	{
		static const GTypeInfo type_info =
		{
			sizeof (NsObjectClass),
			NULL, /* base_init */
			NULL, /* base_finalize */
			ns_object_class_init, /* class_init */
			NULL, /* class_finalize */
			NULL, /* class_data */
			sizeof (NsObject),
			0, /* n_preallocs */
			ns_object_instance_init /* instance_init */
    	};

		type = g_type_register_static
			(CONN_OBJECT_TYPE, "NsObjectType", &type_info, 0);
	}

	return	type;
}
