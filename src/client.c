// A TCR client who acts as both sender and subscriber
// Connects SUB socket to tcp://localhost:5556
// Connects PUB socket to tc[://localhost:5566
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
#include "client_con.h"

clientObject *send_address(clientObject *client_obj)
{
  printf ("Connecting to broker to send IP address…\n");
  client_obj->context = zmq_init(1);
  client_obj->requester = zmq_socket (client_obj->context, ZMQ_REQ);
  zmq_connect (client_obj->requester, "tcp://localhost:5555");
  //Sending IP address to broker
  char address [20];
  sprintf (address, "%s","localhost");
  s_send (client_obj->requester, address);
  printf("Sent address: %s\n",address);
  //Receiving confirmation from broker
  char *message = s_recv (client_obj->requester);
  printf ("Received: %s :from broker\n",message);
  client_obj->publish = TRUE;
  zmq_close (client_obj->requester);
  zmq_term (client_obj->context);

  return client_obj;
}


clientObject *connect_broker(clientObject *client_obj)
{
  client_obj->context = zmq_init (1);

  gchar *forwarder_address =  g_strdup_printf("tcp://%s:%d",client_obj->server, client_obj->port);
  // Socket to talk to broker
  client_obj->subscriber = zmq_socket (client_obj->context, ZMQ_SUB);
  zmq_connect (client_obj->subscriber, forwarder_address);

  // Subscribe to default group :world
  char *filter =   g_strdup_printf("%s", client_obj->group_hash);
  zmq_setsockopt (client_obj->subscriber, ZMQ_SUBSCRIBE, filter  , strlen(filter));
  printf ("Collect data from broker for group %s …\n",filter);
  g_free(filter);
  g_free(forwarder_address);
  return client_obj;
}

void receive_data(clientObject *client_obj)
{
  while(1)
    {
      	char *string = s_recv (client_obj->subscriber);
	int covariance;
	int count;
	char group[40];
	sscanf (string, "%s %d %d", &group, &covariance, &count);
	printf("%d:->Covariance received %d from group %s\n",count, covariance, group);
	free (string);
	g_usleep(10);
    }
}

clientObject *start_publisher(clientObject *client_obj)
{

  // Prepare our context and publisher
  client_obj->context = zmq_init (1);
  client_obj->publisher = zmq_socket (client_obj->context, ZMQ_PUB);
  zmq_connect (client_obj->publisher, "tcp://*:5556");
  g_print("Now sending data from port 5556\n");

  return client_obj;
}

void send_data(clientObject *client_obj)
{
  int count = 0;
  while (1) {
    int covariance;
    covariance = randof (1000);
    // Initialize random covariance number generator
    srandom ((unsigned) time (NULL));

    // Send message to all subscribers of group: world
    char update [50];
    sprintf (update,"%s %d %d", client_obj->group_hash, covariance, count);
    //g_print("Sent %s\n",update);
    s_send (client_obj->publisher, update);
    count++;
    g_usleep(100000);
  }
}

int main (int argc, char *argv [])
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
  clientObject *client_obj = NULL;
  GOptionEntry entries[] = 
  {
    { "verbose", 'v', 0, G_OPTION_ARG_NONE, &verbose, "Verbose output", NULL },
    { "group", 'g', 0, G_OPTION_ARG_STRING, &group, "zeromq group", NULL },
    { "server", 's', 0, G_OPTION_ARG_STRING, &server, "zeromq server", NULL },
    { "port", 'p', 0, G_OPTION_ARG_INT, &port, "zeromq port", "N" },

    { NULL }
  };
 

  context = g_option_context_new ("- zero mq");
  g_option_context_add_main_entries (context, entries, PACKAGE_NAME);
  
  if (!g_option_context_parse (context, &argc, &argv, &error)) {
    g_printerr("option parsing failed: %s\n", error->message);
    exit (EXIT_FAILURE);
  }

  client_obj = make_client_object();
  client_obj->server =  g_strdup_printf("%s",server);
  client_obj->port = port;

  g_thread_init(NULL);
  
  uuid_generate_random(buf);
  uuid_unparse(buf, id);
  // generate a hash of a unique id
  user_hash = g_compute_checksum_for_string(G_CHECKSUM_MD5, id, strlen(id));

  // generate a hash of the group name
  group_hash = g_compute_checksum_for_string(G_CHECKSUM_MD5, group, strlen(group));
  // 32 len strings don't seem to work so this is a fix
  group_name_fix = g_strndup(group_hash, 31);
  
  client_obj->group_hash = g_strdup_printf("%s", group_hash);
  client_obj->user_hash =  g_strdup_printf("%s", user_hash);

  g_free(user_hash);
  g_free(group_hash);
  
  /* client_obj = send_address(client_obj); */
  client_obj = connect_broker(client_obj);


  mainloop = g_main_loop_new(NULL, FALSE);

  if (mainloop == NULL) {
    g_printerr("Couldn't create GMainLoop\n");
    exit(EXIT_FAILURE);
  }

  if( g_thread_create( (GThreadFunc) receive_data, (gpointer) client_obj, FALSE, &error) == NULL) {
      g_printerr("option parsing failed1: %s\n", error->message);
  exit (EXIT_FAILURE);
  }
 
  if(client_obj->publish) {
    client_obj = start_publisher(client_obj);
 
    if( g_thread_create( (GThreadFunc) send_data, (gpointer) client_obj, FALSE, &error) == NULL ) {
      g_printerr("option parsing failed 2: %s\n", error->message);
      exit (EXIT_FAILURE);
    }
  }

  g_main_loop_run(mainloop);
  
  return EXIT_FAILURE;
}
