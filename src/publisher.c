#include <glib.h>
#include <stdlib.h>
#include <string.h>
#include <zmq.h>

#include "config.h"
#include "zhelpers.h"
#include "publisher.h"

pubObject *make_pub_object(void)
{
  pubObject *pub_obj;

  if ((pub_obj = (pubObject *)g_malloc(sizeof(pubObject))) == NULL) {
    //g_printerr("failed to malloc pubObject!");
    exit(EXIT_FAILURE);
  }

  pub_obj->publish = TRUE;
  return pub_obj;
}

pubObject *publish_forwarder(pubObject *pub_obj)
{
  gchar *forwarder_address =  g_strdup_printf("tcp://%s:%d",pub_obj->server, 5566);
  /* Prepare our context and publisher */
  pub_obj->context = zmq_init (1);
  pub_obj->publisher = zmq_socket (pub_obj->context, ZMQ_PUB);
  zmq_connect (pub_obj->publisher, forwarder_address);
  g_print("Now sending data to forwarder %s\n",forwarder_address);
  g_free(forwarder_address);
  return pub_obj;
}

void send_data(pubObject *pub_obj)
{
  int count = 0;
  while (1) {
    int covariance;
    covariance = randof (1000);
    // Initialize random covariance number generator
    srandom ((unsigned) time (NULL));

    // Send message to all subscribers of group: world
    char update [50];
    sprintf (update,"%s %d %d", pub_obj->group_hash, covariance, count);
    g_print("Sent :%s\n",update);
    s_send (pub_obj->publisher, update);
    count++;
    g_usleep(100000);
  }
}

void free_pub_object(pubObject *pub_obj)
{
  zmq_close (pub_obj->publisher);
  zmq_term (pub_obj->context);
  g_free(pub_obj);  
}
