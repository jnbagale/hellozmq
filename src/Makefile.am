

INCLUDES = -Wall -g  $(LIBZMQ_CFLAGS) $(PTHREAD_CFLAGS) $(UUID_CFLAGS)

bin_PROGRAMS =  broker node

broker_SOURCES = broker.c forwarder.c forwarder.h config.h

broker_LDADD =  $(LIBZMQ_LIBS)

node_SOURCES = node.c subscriber.c subscriber.h publisher.c publisher.h message.c message.h config.h

node_LDADD =  $(LIBZMQ_LIBS) $(PTHREAD_LIBS) $(UUID_LIBS)



