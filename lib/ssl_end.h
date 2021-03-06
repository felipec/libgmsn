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

#ifndef MSN_SSL_END_H
#define MSN_SSL_END_H

#include <glib.h>

#include "glib-object.h"

typedef struct SslEndObject SslEndObject;
typedef struct SslEndObjectClass SslEndObjectClass;

#include "ssl.h"
#include "conn.h"

struct SslEndObject
{
    ConnEndObject parent;
    gboolean dispose_has_run;

    MsnSsl *ssl;
};

struct SslEndObjectClass
{
    ConnEndObjectClass parent_class;
};

#define SSL_END_OBJECT_TYPE (ssl_end_object_get_type ())
#define SSL_END_OBJECT(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), SSL_END_OBJECT_TYPE, SslEndObject))
#define SSL_END_OBJECT_CLASS(c) (G_TYPE_CHECK_CLASS_CAST ((c), SSL_END_OBJECT_TYPE, SslEndObjectClass))
#define SSL_IS_END_OBJECT(obj) (G_TYPE_CHECK_TYPE ((obj), SSL_END_OBJECT_TYPE))
#define SSL_IS_END_OBJECT_CLASS(c) (G_TYPE_CHECK_CLASS_TYPE ((c), SSL_END_OBJECT_TYPE))
#define SSL_END_OBJECT_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), SSL_END_OBJECT_TYPE, SslEndObjectClass))

SslEndObject *ssl_end_object_new (ConnObject *ssl_end);
void ssl_end_object_free (SslEndObject *ssl_end);

#endif /* MSN_SSL_END_H */
