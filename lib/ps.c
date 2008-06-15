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

#include "ps.h"
#include "ssl_end.h"
#include "util.h"

#include <libxml/tree.h>
#include <libxml/uri.h>
#include <string.h>

static GObjectClass *parent_class = NULL;

PsObject *
ps_object_new (MsnCore *core)
{
	PsObject *ps;
	ConnObject *conn;

	ps = PS_OBJECT (g_type_create_instance (PS_OBJECT_TYPE));

	conn = CONN_OBJECT (ps);

	conn->core = core;
	conn->name = g_strdup ("passport");
	conn->type = MSN_CONN_PASSPORT;

	return ps;
}

void
ps_object_free (PsObject *ps)
{
	g_object_unref (G_OBJECT (ps));
}

static void
payload_cb (ConnObject *conn, MsnBuffer *buf)
{
	gchar *start;
	gchar *end;
	start = strstr (buf->data, "<wsse:BinarySecurityToken Id=\"PPToken1\">");
	if (start)
	{
		gchar *tmp;
		gchar *tmp2;

		start += strlen ("<wsse:BinarySecurityToken Id=\"PPToken1\">");
		end = strstr (start, "</wsse:BinarySecurityToken>");
		tmp = g_strndup (start, end - start);
		tmp2 = xml_decode (tmp);
		g_free (tmp);
		msn_core_got_ticket (conn->core, tmp2);
		g_free (tmp2);
	}
}

static void
parse_impl (ConnObject *conn)
{
	gchar *cur;
	static guint last_br = 0;
	static guint content_length = 0;

	for (cur = conn->read_buffer->data;
		 conn->parse_pos < conn->buffer->filled;
		 cur++, conn->parse_pos++)
	{
		static guint st = 0;

		if (st == 0 && *cur == '\r')
		{
			st++;
		}
		else if (st == 1 && *cur == '\n')
		{
			guint pos;
			gchar *start;

			pos = cur - conn->buffer->data - 1;
			start = conn->buffer->data + last_br;

			/* msn_print ("{%v}\n", MSN_TYPE_STRING, start, pos - last_br); */
			if (strncmp (start, "Content-Length: ", strlen ("Content-Length: ")) == 0)
			{
				gchar *tmp;
				tmp = g_strndup (start + strlen ("Content-Length: "), pos);
				content_length = atoi (tmp);
				g_free (tmp);
			}

			last_br = cur + 1 - conn->buffer->data;
			st++;
		}
		else if (st == 2 && *cur == '\r')
		{
			st++;
		}
		else if (st == 3 && *cur == '\n')
		{
			st++;
			{
				conn->payload = msn_buffer_new ();
				conn->payload->size = content_length;
				conn->payload_cb = payload_cb;
				conn->parse_pos++;
				conn->parsed = TRUE;
				conn->last_parse_pos = conn->parse_pos;
				return;
			}
		}
		else
		{
			st = 0;
		}
	}
}

static void
action (ConnObject *conn,
		gchar *uri_str,
		xmlDoc *doc)
{
	gchar *head;
	gchar *request;

	xmlURI *uri;

	uri = xmlParseURI (uri_str);

	{
		char *body;
		int body_size;
		xmlDocDumpMemoryEnc (doc, (xmlChar **) &body, &body_size, "UTF-8");

		head = g_strdup_printf (
								"POST %s HTTP/1.1\r\n"
								"Accept: text/*\r\n"
								"User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1)\r\n"
								"Host: %s\r\n"
								"Content-Length: %d\r\n"
								"Connection: Keep-Alive\r\n"
								"Cache-Control: no-cache\r\n",
								uri->path, uri->server, body_size);

		request = g_strdup_printf ("%s\r\n%s", head, body);
		free (body);
	}

#if 0
	g_print ("request = {%s}\n", request);
#endif

	conn_end_object_write (conn->end, request, strlen (request));

	g_free (request);

	xmlFreeURI (uri);
}

static void
request_security_info (PsObject *ps)
{
	ConnObject *conn;
	gchar *guid;
	gchar *user;
	gchar *pwd;
	gchar *ticket;

	xmlDocPtr doc;
	xmlNodePtr root_node;
	xmlNodePtr header_node;
	xmlNodePtr body_node;
	xmlNodePtr father_node;
	xmlNodePtr tokens_node;
	xmlNodePtr token_node;
	xmlNodePtr node;
	xmlNsPtr soap;
	xmlNsPtr wsse;
	xmlNsPtr wst;
	xmlNsPtr wsp;
	xmlNsPtr wsa;
	xmlNsPtr ps_ns;

	conn = CONN_OBJECT(ps);
	guid = "7108E71A-9926-4FCB-BCC9-9A9D3F32E423";
	user = conn->core->username;
	pwd = conn->core->password;
	ticket = ps->ticket;

	doc = xmlNewDoc (BAD_CAST "1.0");

	root_node = xmlNewNode (NULL, BAD_CAST "Envelope");

	soap = xmlNewNs (root_node, BAD_CAST "http://schemas.xmlsoap.org/soap/envelope/", BAD_CAST "soap");
	wsse = xmlNewNs (root_node, BAD_CAST "http://schemas.xmlsoap.org/ws/2003/06/secext", BAD_CAST "wsse");
	wsp = xmlNewNs (root_node, BAD_CAST "http://schemas.xmlsoap.org/ws/2002/12/policy", BAD_CAST "wsp");
	wsa = xmlNewNs (root_node, BAD_CAST "http://schemas.xmlsoap.org/ws/2004/03/addressing", BAD_CAST "wsa");
	wst = xmlNewNs (root_node, BAD_CAST "http://schemas.xmlsoap.org/ws/2004/04/trust", BAD_CAST "wst");

	ps_ns = xmlNewNs (root_node, BAD_CAST "http://schemas.microsoft.com/Passport/SoapServices/PPCRL", BAD_CAST "ps");

	xmlSetNs (root_node, soap);
	xmlDocSetRootElement (doc, root_node);

	header_node = xmlNewChild (root_node, NULL, BAD_CAST "Header", NULL);

	father_node = xmlNewChild (header_node, ps_ns, BAD_CAST "AuthInfo", NULL);
	xmlNewProp (father_node, BAD_CAST "Id", BAD_CAST "PPAuthInfo");

	{
		gchar *tmp;
		tmp = g_strdup_printf ("{%s}", guid);
		node = xmlNewChild (father_node, NULL, BAD_CAST "HostingApp", BAD_CAST tmp); /* ? */
		g_free (tmp);
	}
	node = xmlNewChild (father_node, NULL, BAD_CAST "BinaryVersion", BAD_CAST "4");
	node = xmlNewChild (father_node, NULL, BAD_CAST "UIVersion", BAD_CAST "1");
	node = xmlNewChild (father_node, NULL, BAD_CAST "Cookies", NULL);
	node = xmlNewChild (father_node, NULL, BAD_CAST "RequestParams", BAD_CAST "AQAAAAIAAABsYwQAAAAzMDg0");

	father_node = xmlNewChild (header_node, wsse, BAD_CAST "Security", NULL);
	father_node = xmlNewChild (father_node, NULL, BAD_CAST "UsernameToken", NULL);
	xmlNewProp (father_node, BAD_CAST "Id", BAD_CAST "user");
	node = xmlNewChild (father_node, NULL, BAD_CAST "Username", BAD_CAST user); /* ? */
	node = xmlNewChild (father_node, NULL, BAD_CAST "Password", BAD_CAST pwd); /* ? */

	body_node = xmlNewChild (root_node, NULL, BAD_CAST "Body", NULL);
	tokens_node = xmlNewChild (body_node, ps_ns, BAD_CAST "RequestMultipleSecurityTokens", NULL);
	xmlNewProp (tokens_node, BAD_CAST "Id", BAD_CAST "RSTS");

	token_node = xmlNewChild (tokens_node, wst, BAD_CAST "RequestSecurityToken", NULL);
	xmlNewProp (token_node, BAD_CAST "Id", BAD_CAST "RST0");
	father_node = xmlNewChild (token_node, NULL, BAD_CAST "RequestType", BAD_CAST "http://schemas.xmlsoap.org/ws/2004/04/security/trust/Issue");
	father_node = xmlNewChild (token_node, wsp, BAD_CAST "AppliesTo", NULL);
	father_node = xmlNewChild (father_node, wsa, BAD_CAST "EndpointReference", NULL);
	father_node = xmlNewChild (father_node, NULL, BAD_CAST "Address", BAD_CAST "http://Passport.NET/tb");

	token_node = xmlNewChild (tokens_node, wst, BAD_CAST "RequestSecurityToken", NULL);
	xmlNewProp (token_node, BAD_CAST "Id", BAD_CAST "RST1");
	father_node = xmlNewChild (token_node, NULL, BAD_CAST "RequestType", BAD_CAST "http://schemas.xmlsoap.org/ws/2004/04/security/trust/Issue");
	father_node = xmlNewChild (token_node, wsp, BAD_CAST "AppliesTo", NULL);
	father_node = xmlNewChild (father_node, wsa, BAD_CAST "EndpointReference", NULL);
	father_node = xmlNewChild (father_node, NULL, BAD_CAST "Address", BAD_CAST "messenger.msn.com");
	father_node = xmlNewChild (token_node, wsse, BAD_CAST "PolicyReference", NULL);

	{
		gchar *tmp1;
		gchar *tmp2;
		gchar *tmp3;

		tmp1 = url_decode (ticket);
		tmp2 = string_replace (tmp1, ',', '&');
		tmp3 = g_strdup_printf ("?%s", tmp2);

		xmlNewProp (father_node, BAD_CAST "URI", BAD_CAST tmp3); /* ? */

		g_free (tmp1);
		g_free (tmp2);
		g_free (tmp3);
	}

	action (CONN_OBJECT (ps),
			"http://loginnet.passport.com/RST.srf",
			doc);

	xmlFreeDoc (doc);
}

static void
write_impl (ConnObject *conn)
{
	PsObject *ps;
	static guint state = 0;

	ps = PS_OBJECT (conn);

	if (state == 0)
	{
		request_security_info (ps);
		state++;
	}
}

/* GObject stuff. */

static void
ps_object_dispose (GObject *obj)
{
	ConnObject *conn = CONN_OBJECT (obj);

	if (!conn->dispose_has_run)
	{
		conn->dispose_has_run = TRUE;
	}

	G_OBJECT_CLASS (parent_class)->dispose (obj);
}

static void
ps_object_finalize (GObject *obj)
{
	PsObject *ps = PS_OBJECT (obj);

	g_free (ps->ticket);

	/* Chain up to the parent class */
	G_OBJECT_CLASS (parent_class)->finalize (obj);
}

void
ps_object_class_init (gpointer g_class, gpointer class_data)
{
	ConnObjectClass *conn_class = CONN_OBJECT_CLASS (g_class);
	GObjectClass *gobject_class = G_OBJECT_CLASS (g_class);

	conn_class->parse = &parse_impl;
	conn_class->write = &write_impl;

	gobject_class->dispose = ps_object_dispose;
	gobject_class->finalize = ps_object_finalize;

	parent_class = g_type_class_peek_parent (g_class);
}

void
ps_object_instance_init (GTypeInstance *instance, gpointer g_class)
{
	ConnObject *conn = CONN_OBJECT (instance);
	conn->end = CONN_END_OBJECT (ssl_end_object_new (conn));
}

GType
ps_object_get_type (void)
{
	static GType type = 0;

	if (type == 0) 
	{
		static const GTypeInfo type_info = 
		{
			sizeof (PsObjectClass),
			NULL, /* base_init */
			NULL, /* base_finalize */
			ps_object_class_init, /* class_init */
			NULL, /* class_finalize */
			NULL, /* class_data */
			sizeof (PsObject),
			0, /* n_preallocs */
			ps_object_instance_init /* instance_init */
    	};

		type = g_type_register_static
			(CONN_OBJECT_TYPE, "PsObjectType", &type_info, 0);
	}

	return	type;
}
