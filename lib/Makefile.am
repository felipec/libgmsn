lib_LTLIBRARIES = libmsn.la
libmsn_la_SOURCES = cmd.c conn.c ssl.c util.c sem.c core.c ns.c buffer.c debug.c ps.c cs.c conn_end.c ssl_end.c
libmsn_la_CFLAGS = $(GTHREAD_CFLAGS) $(GOBJECT_CFLAGS) $(OPENSSL_CFLAGS) $(LIBXML_CFLAGS)
libmsn_la_LIBADD = $(GTHREAD_LIBS) $(GOBJECT_LIBS) $(OPENSSL_LIBS) $(LIBXML_LIBS)
libmsn_la_LDFLAGS = -version-info 0:0:0
