//
// Broker which receives data from clients and sends it back to them
// Binds PUB socket to tcp://*:5556
// Publishes covariance data
//

#include <zmq.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include <glib/gthread.h>
#include <uuid/uuid.h>

#include "zhelpers.h"
#include "config.h"
#include "broker_con.h"

brokerObject *connect_client_pub(brokerObject *broker_obj)
{
  broker_obj->context = zmq_init (1);

  // Socket to talk to broker
  broker_obj->subscriber = zmq_socket (broker_obj->context, ZMQ_SUB);
  zmq_connect (broker_obj->subscriber, "tcp://localhost:5566");

  // Subscribe to default group :world
  char *filter =   g_strdup_printf("%s", broker_obj->group_hash);
  zmq_setsockopt (broker_obj->subscriber, ZMQ_SUBSCRIBE, filter  , strlen(filter));
  printf ("Collect data from broker for group %s …\n",filter);
  g_free(filter);
  return broker_obj;
}

void receive_data_pub(brokerObject *broker_obj)
{
  while(1)
    {
      	char *string = s_recv (broker_obj->subscriber);
	int covariance;
	char group[40];
	sscanf (string, "%s %d", &group, &covariance);
	printf("Covariance received %d from client\n", covariance);
	free (string);
	g_usleep(10);
    }
}


brokerObject *start_responder(brokerObject *broker_obj)
{
   broker_obj->context = zmq_init (1);
  // Socket to talk to clients
  broker_obj->responder = zmq_socket (broker_obj->context, ZMQ_REP);
  zmq_bind (broker_obj->responder, "tcp://*:5555");
  g_print("The broker is now listening client's IP addresses to subscribe to them...\n\n");

  return broker_obj;
}

void retrieve_client_address(brokerObject *broker_obj)
{
  while (1) {
    // Wait for next request from client
     char *address = s_recv(broker_obj->responder);
     g_print("Received %s from client\n", address);

     
     // Do some 'work'
     sleep (1);

    // Send reply back to client
    char response[20];
    sprintf (response, "%s","confirmation");
    s_send (broker_obj->responder, response);
    g_print("Sent %s\n",response);
    //Start a subscriber which subscribes to different clients
  }

}


brokerObject *start_publisher(brokerObject *broker_obj)
{

  // Prepare our context and publisher
  broker_obj->context = zmq_init (1);
  broker_obj->publisher = zmq_socket (broker_obj->context, ZMQ_PUB);
  zmq_bind (broker_obj->publisher, "tcp://*:5556");
  g_print("Now sending data from port 5556\n");

  return broker_obj;
}

void send_data(brokerObject *broker_obj)
{
  while (1) {

    int covariance;
    covariance = randof (1000);
    // Initialize random covariance number generator
    srandom ((unsigned) time (NULL));

    // Send message to all subscribers of group: world
    char update [50];
    sprintf (update,"%s %d", broker_obj->group_hash, covariance);
    //g_print("Sent %s\n",update);
    s_send (broker_obj->publisher, update);
    g_usleep(100000);
  }
}

void start_forwarder(brokerObject *broker_obj)
{
  // void *context;          //  ØMQ context for our process
  void *frontend;          //  Socket facing outside
  void *backend;         //  Socket facing frontend

  //  This is where the weather update server sits
  char *frontend_endpoint = "tcp://*:5556";
  
  
  //  This is our public IP address and port
  char *backend_endpoint =  g_strdup_printf("tcp://%s:%d",broker_obj->server, broker_obj->port);

  //  Prepare our context and sockets
  broker_obj->context  = zmq_init (1);
  broker_obj->frontend  = zmq_socket (broker_obj->context, ZMQ_SUB);
  broker_obj->backend = zmq_socket (broker_obj->context, ZMQ_PUB);

  zmq_bind (broker_obj->frontend,  frontend_endpoint);
  zmq_bind (broker_obj->backend, backend_endpoint);

  //  Subscribe on everything
  zmq_setsockopt (broker_obj->frontend, ZMQ_SUBSCRIBE, "", 0);
  g_print("Forwarder device is started\n");
  //  Start the forwarder device
  zmq_device (ZMQ_FORWARDER, broker_obj->frontend, broker_obj->backend);
  

}



int main (int argc, char *argv[])
{
  gint ret;
  GMainLoop *mainloop;
  GError *error;
  uuid_t buf;
  gchar id[36];
  gchar *user_hash;
  gchar *group_hash;
  gchar *group_name_fix;
  gchar *group = DEFAULT_GROUP;
  gchar *server = DEFAULT_SERVER;
  gint port = DEFAULT_PORT;
  gboolean verbose = FALSE;
  GOptionContext *context;
  brokerObject *broker_obj = NULL;
  GOptionEntry entries[] = 
  {
    { "verbose", 'v', 0, G_OPTION_ARG_NONE, &verbose, "Verbose output", NULL },
    { "group", 'g', 0, G_OPTION_ARG_STRING, &group, "zeromq group", NULL },
    { "server", 's', 0, G_OPTION_ARG_STRING, &server, "zeromq server", NULL },
    { "port", 'p', 0, G_OPTION_ARG_INT, &port, "zeromq port", "N" },

    { NULL }
  };
 

  context = g_option_context_new ("- the clashing rocks");
  g_option_context_add_main_entries (context, entries, PACKAGE_NAME);
  
  if (!g_option_context_parse (context, &argc, &argv, &error)) {
    g_printerr("option parsing failed: %s\n", error->message);
    exit (EXIT_FAILURE);
  }

  broker_obj = make_broker_object();

  broker_obj->server =  g_strdup_printf("%s",server);
  broker_obj->port = port;
  g_thread_init(NULL);
  
  uuid_generate_random(buf);
  uuid_unparse(buf, id);
  // generate a hash of a unique id
  user_hash = g_compute_checksum_for_string(G_CHECKSUM_MD5, id, strlen(id));

  // generate a hash of the group name
  group_hash = g_compute_checksum_for_string(G_CHECKSUM_MD5, group, strlen(group));
  // 32 len strings don't seem to work so this is a fix
  group_name_fix = g_strndup(group_hash, 31);
  
  mainloop = g_main_loop_new(NULL, FALSE);
  if (mainloop == NULL) {
    g_printerr("Couldn't create GMainLoop\n");
    exit(EXIT_FAILURE);
  }
  broker_obj->user_hash = g_strdup_printf("%s",user_hash);
  broker_obj->group_hash = g_strdup_printf("%s",group_hash);
  g_free(user_hash);
  g_free(group_hash);

  start_forwarder(broker_obj);

 /*  broker_obj = start_responder (broker_obj); */
 /*  broker_obj = start_publisher (broker_obj); */
 /*  broker_obj = connect_client_pub(broker_obj); */


 /*  if( g_thread_create( (GThreadFunc) send_data, (gpointer) broker_obj, FALSE, &error) == NULL ) { */
 /*       g_printerr("option parsing failed 2: %s\n", error->message); */
 /*   exit (EXIT_FAILURE); */
 /*  } */
 
 /* if( g_thread_create( (GThreadFunc) retrieve_client_address, (gpointer) broker_obj, FALSE, &error) == NULL) { */
 /*    g_printerr("option parsing failed1: %s\n", error->message); */
 /*    exit (EXIT_FAILURE); */
 /*  } */

 /*  if( g_thread_create( (GThreadFunc) receive_data_pub, (gpointer) broker_obj, FALSE, &error) == NULL) { */
 /*    g_printerr("option parsing failed1: %s\n", error->message); */
 /*    exit (EXIT_FAILURE); */
 /*  } */

  g_main_loop_run(mainloop);
  
  return EXIT_FAILURE;
}
