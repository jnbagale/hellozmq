
INCLUDES = -Wall -g  $(LIBZMQ_CFLAGS) $(GLIB_CFLAGS) $(GTHREAD_CFLAGS) $(UUID_CFLAGS)

bin_PROGRAMS =  broker client

broker_SOURCES = broker.c broker_con.c broker_con.h zhelpers.h config.h

broker_LDADD =  $(LIBZMQ_LIBS) $(GLIB_LIBS) $(GTHREAD_LIBS) $(UUID_LIBS)

client_SOURCES = client.c client_con.c client_con.h zhelpers.h config.h

client_LDADD =  $(LIBZMQ_LIBS) $(GLIB_LIBS) $(GTHREAD_LIBS) $(UUID_LIBS)



