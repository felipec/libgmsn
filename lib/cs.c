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

#include "cs.h"

#include "util.h"
#include "debug.h"

#include <libxml/tree.h>
#include <libxml/uri.h>
#include <string.h>

GType cs_object_get_type (void);
static GObjectClass *parent_class = NULL;

void cs_object_class_init (gpointer g_class, gpointer class_data);
void cs_object_instance_init (GTypeInstance *instance, gpointer g_class);

CsObject *
cs_object_new (MsnCore *core)
{
	CsObject *cs;
	ConnObject *conn;

	cs = CS_OBJECT (g_type_create_instance (CS_OBJECT_TYPE));

	conn = CONN_OBJECT (cs);

	conn->core = core;
	conn->name = g_strdup ("cs");
	conn->type = MSN_CONN_CS;

	return cs;
}

void
cs_object_free (CsObject *cs)
{
	g_object_unref (G_OBJECT (cs));
}

static void
soap_cb (ConnObject *conn, MsnBuffer *buffer)
{
	msn_print ("soap cb: [%b]\n", buffer);
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
				conn->payload_cb = soap_cb;
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
gen_header (xmlNode *root_node, xmlNs *ns)
{
	xmlNode *header_node;

	header_node = xmlNewChild (root_node, NULL, BAD_CAST "Header", NULL);

	{
		xmlNode *father_node;
		xmlNode *node;

		father_node = xmlNewChild (header_node, ns, BAD_CAST "ABApplicationHeader", NULL);
		node = xmlNewChild (father_node, NULL, BAD_CAST "ApplicationId", BAD_CAST "09607671-1C32-421F-A6A6-CBFAA51AB5F4");
		node = xmlNewChild (father_node, NULL, BAD_CAST "IsMigraton", BAD_CAST "false");
		node = xmlNewChild (father_node, NULL, BAD_CAST "PartnerScenario", BAD_CAST "Initial");
	}

	{
		xmlNode *father_node;
		xmlNode *node;

		father_node = xmlNewChild (header_node, ns, BAD_CAST "ABAuthHeader", NULL);
		node = xmlNewChild (father_node, NULL, BAD_CAST "ManagedGroupRequest", BAD_CAST "false");
	}
}

static void
soap_action (ConnObject *conn,
			 gchar *uri_str,
			 gchar *action,
			 xmlDoc *doc)
{
	gchar *head;
	gchar *request;

	xmlURI *uri;

	uri = xmlParseURI (uri_str);

	{
		gchar *tmp;
		xmlDocDumpFormatMemoryEnc (doc, (xmlChar **) &tmp, NULL, "UTF-8", 1);
		g_print ("request [%s]\n", tmp);
		g_free (tmp);
	}

	{
		char *body;
		int body_size;

		xmlDocDumpMemoryEnc (doc, (xmlChar **) &body, &body_size, "UTF-8");

		head = g_strdup_printf (
								"POST %s HTTP/1.1\r\n"
								"SOAPAction: %s\r\n"
								"Content-Type: text/xml; charset=utf-8\r\n"
								"Cookie: MSPAuth=%s\r\n"
								"Host: %s\r\n"
								"Content-Length: %d\r\n",
								uri->path, action, conn->core->cookie, uri->server, body_size);

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
request_membership_lists (CsObject *cs)
{
	xmlDoc *doc;

	/* * ABApplicationHeader (ApplicationId = "09607671-1C32-421F-A6A6-CBFAA51AB5F4",
	 *                        IsMigration = "false",
	 *                        PartnerScenario = "Initial") */
	/* * ABAuthHeader (ManagedGroupRequest = "false") */
	/* ->FindMembership (serviceFilter = Types = { Messenger, Invitation, SocialNetwork, Space, Profile }) */

	doc = xmlNewDoc (BAD_CAST "1.0");

	{
		xmlNode *root_node;
		xmlNs *soap;
		xmlNs *ab;

		root_node = xmlNewNode (NULL, BAD_CAST "Envelope");

		soap = xmlNewNs (root_node, BAD_CAST "http://schemas.xmlsoap.org/soap/envelope/", BAD_CAST "soap");
		ab = xmlNewNs (root_node, BAD_CAST "http://www.msn.com/webservices/AddressBook", BAD_CAST "ap");

		xmlSetNs (root_node, soap);
		xmlDocSetRootElement (doc, root_node);

		gen_header (root_node, ab);

		{
			xmlNode *body_node;

			body_node = xmlNewChild (root_node, NULL, BAD_CAST "Body", NULL);

			{
				xmlNode *father_node;
				xmlNode *node;

				father_node = xmlNewChild (body_node, ab, BAD_CAST "FindMembership", NULL);
				father_node = xmlNewChild (father_node, NULL, BAD_CAST "serviceFilter", NULL);
				father_node = xmlNewChild (father_node, NULL, BAD_CAST "Types", NULL);
				node = xmlNewChild (father_node, NULL, BAD_CAST "ServiceType", BAD_CAST "Messenger");
				node = xmlNewChild (father_node, NULL, BAD_CAST "ServiceType", BAD_CAST "Invitation");
				node = xmlNewChild (father_node, NULL, BAD_CAST "ServiceType", BAD_CAST "SocialNetwork");
				node = xmlNewChild (father_node, NULL, BAD_CAST "ServiceType", BAD_CAST "Space");
				node = xmlNewChild (father_node, NULL, BAD_CAST "ServiceType", BAD_CAST "Profile");
			}
		}
	}

	soap_action (CONN_OBJECT (cs),
				 "http://contacts.msn.com/abservice/SharingService.asmx",
				 "http://www.msn.com/webservices/AddressBook/FindMembership",
				 doc);

	xmlFreeDoc (doc);
}

static void
request_address_book (CsObject *cs)
{
	xmlDoc *doc;

	/* * ABApplicationHeader (ApplicationId = "09607671-1C32-421F-A6A6-CBFAA51AB5F4",
	 *                        IsMigration = "false",
	 *                        PartnerScenario = "Initial") */
	/* * ABAuthHeader (ManagedGroupRequest = "false") */
	/* -> ABFindAll (abId = "00000000-0000-0000-0000-000000000000",
	 *               abView = Full
	 *               deltasOnly = true
	 *               lastChange = "0001-01-01T00:00:00.0000000-08:00") */

	doc = xmlNewDoc (BAD_CAST "1.0");

	{
		xmlNode *root_node;
		xmlNs *soap;
		xmlNs *ab;

		root_node = xmlNewNode (NULL, BAD_CAST "Envelope");

		soap = xmlNewNs (root_node, BAD_CAST "http://schemas.xmlsoap.org/soap/envelope/", BAD_CAST "soap");
		ab = xmlNewNs (root_node, BAD_CAST "http://www.msn.com/webservices/AddressBook", BAD_CAST "ap");

		xmlSetNs (root_node, soap);
		xmlDocSetRootElement (doc, root_node);

		gen_header (root_node, ab);

		{
			xmlNode *body_node;

			body_node = xmlNewChild (root_node, NULL, BAD_CAST "Body", NULL);

			{
				xmlNode *father_node;
				xmlNode *node;

				father_node = xmlNewChild (body_node, ab, BAD_CAST "ABFindAll", NULL);
				node = xmlNewChild (father_node, NULL, BAD_CAST "abId", BAD_CAST "00000000-0000-0000-0000-000000000000");
				node = xmlNewChild (father_node, NULL, BAD_CAST "abView", BAD_CAST "Full");
				node = xmlNewChild (father_node, NULL, BAD_CAST "deltasOnly", BAD_CAST "false");
				/* node = xmlNewChild (father_node, NULL, BAD_CAST "lastChange", BAD_CAST "0001-01-01T00:00:00.0000000-08:00"); */
			}
		}
	}

	soap_action (CONN_OBJECT (cs),
				 "http://contacts.msn.com/abservice/abservice.asmx",
				 "http://www.msn.com/webservices/AddressBook/ABFindAll",
				 doc);

	xmlFreeDoc (doc);
}

static void
write_impl (ConnObject *conn)
{
	CsObject *cs;
	static guint state = 0;

	cs = CS_OBJECT (conn);

	if (state == 0)
	{
		request_membership_lists (cs);
		state++;
	}
	else if (state == 1)
	{
		request_address_book (cs);
		state++;
	}
}

static void
connect_impl (ConnObject *conn)
{
	CsObject *cs;

	cs = CS_OBJECT (conn);
}

static void
cs_object_dispose (GObject *obj)
{
	ConnObject *conn = (ConnObject *) obj;

	if (!conn->dispose_has_run)
	{
		conn->dispose_has_run = TRUE;
	}

	G_OBJECT_CLASS (parent_class)->dispose (obj);
}

static void
cs_object_finalize (GObject *obj)
{
	/* Chain up to the parent class */
	G_OBJECT_CLASS (parent_class)->finalize (obj);
}

void
cs_object_class_init (gpointer g_class, gpointer class_data)
{
	ConnObjectClass *conn_class = CONN_OBJECT_CLASS (g_class);
	GObjectClass *gobject_class = G_OBJECT_CLASS (g_class);

	conn_class->write = &write_impl;
	conn_class->connect = &connect_impl;
	conn_class->parse = &parse_impl;

	gobject_class->dispose = cs_object_dispose;
	gobject_class->finalize = cs_object_finalize;

	parent_class = g_type_class_peek_parent (g_class);
}

void
cs_object_instance_init (GTypeInstance *instance, gpointer g_class)
{
}

GType
cs_object_get_type (void)
{
	static GType type = 0;

	if (type == 0) 
	{
		static const GTypeInfo type_info = 
		{
			sizeof (CsObjectClass),
			NULL, /* base_init */
			NULL, /* base_finalize */
			cs_object_class_init, /* class_init */
			NULL, /* class_finalize */
			NULL, /* class_data */
			sizeof (CsObject),
			0, /* n_preallocs */
			cs_object_instance_init /* instance_init */
    	};

		type = g_type_register_static
			(CONN_OBJECT_TYPE, "CsObjectType", &type_info, 0);
	}

	return	type;
}
