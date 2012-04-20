

/* ZeroMQ Forwarder which receives data from publishers and sends it back to subscribers */
/* Binds PUB socket to tcp://\*:5556 */
/* Binds SUB socket to given host address */
/* Publishes covariance data */



#include <glib.h>
#include <stdlib.h>
#include <zmq.h>

#include "config.h"
#include "forwarder.h"

serverObject *make_server_object(void)
{
  serverObject *server_obj;


  if ((server_obj = (serverObject *)g_malloc(sizeof(serverObject))) == NULL) {
    //g_printerr("failed to malloc serverObject!");
    exit(EXIT_FAILURE);
  }

  return server_obj;
}


void start_forwarder(serverObject *server_obj)
{
  // To subscriber to all the publishers
  gchar *frontend_endpoint = "tcp://*:5556";
  
  //  This is our public IP address and port
  gchar *backend_endpoint =  g_strdup_printf("tcp://%s:%d",server_obj->host, server_obj->port);

  //  Prepare context and sockets
  server_obj->context  = zmq_init (1);
  server_obj->frontend  = zmq_socket (server_obj->context, ZMQ_SUB);
  server_obj->backend = zmq_socket (server_obj->context, ZMQ_PUB);

  zmq_bind (server_obj->frontend,  frontend_endpoint);
  zmq_bind (server_obj->backend, backend_endpoint);

  //  Subscribe for everything
  zmq_setsockopt (server_obj->frontend, ZMQ_SUBSCRIBE, "", 0); 
  g_print("\nForwarder device is receiving at %s\n",frontend_endpoint);
  g_print("\nForwarder device is sending from %s\n",backend_endpoint);
  //  Start the forwarder device
  zmq_device (ZMQ_FORWARDER, server_obj->frontend, server_obj->backend);
}

void free_server_object(serverObject *server_obj)
{
  zmq_term (server_obj->context);
  g_free(server_obj->group_hash);
  g_free(server_obj->user_hash);
  g_free(server_obj->host);
  g_free(server_obj);  
}
