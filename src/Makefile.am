

INCLUDES = -Wall -g  $(LIBZMQ_CFLAGS) $(GLIB_CFLAGS) $(GTHREAD_CFLAGS) $(UUID_CFLAGS)

bin_PROGRAMS =  broker node

broker_SOURCES = broker.c forwarder.c forwarder.h config.h

broker_LDADD =  $(LIBZMQ_LIBS) $(GLIB_LIBS) $(GTHREAD_LIBS)

node_SOURCES = node.c subscriber.c subscriber.h publisher.c publisher.h zhelpers.h config.h

node_LDADD =  $(LIBZMQ_LIBS) $(GLIB_LIBS) $(GTHREAD_LIBS) $(UUID_LIBS)



