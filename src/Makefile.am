
INCLUDES = -Wall -g  $(LIBZMQ_CFLAGS) $(GLIB_CFLAGS) $(GTHREAD_CFLAGS) $(UUID_CFLAGS)

bin_PROGRAMS =  server client

server_SOURCES = server.c forwarder.c forwarder.h config.h

server_LDADD =  $(LIBZMQ_LIBS) $(GLIB_LIBS) $(GTHREAD_LIBS) $(UUID_LIBS)


client_SOURCES = client.c subscriber.c subscriber.h publisher.c publisher.h zhelpers.h config.h

client_LDADD =  $(LIBZMQ_LIBS) $(GLIB_LIBS) $(GTHREAD_LIBS) $(UUID_LIBS)



