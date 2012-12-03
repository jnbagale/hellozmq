
#include <glib.h>
#include <string.h>
#include <stdlib.h>
#include <zmq.h>

#include "config.h"
#include "subscriber.h"
#include "zhelpers.h"

subObject *make_sub_object(void)
{
  subObject *sub_obj;

  if ((sub_obj = (subObject *)g_malloc(sizeof(subObject))) == NULL) {
    //g_printerr("failed to malloc subObject!");
    exit(EXIT_FAILURE);
  }

  return sub_obj;
}

subObject *subscribe_forwarder(subObject *sub_obj)
{
  uint64_t hwm = 100; 
  sub_obj->context = zmq_init (1);

  gchar *forwarder_address =  g_strdup_printf("tcp://%s:%d",sub_obj->host, sub_obj->port);

   /* Socket to subscribe to forwarder */
  sub_obj->subscriber = zmq_socket (sub_obj->context, ZMQ_SUB);
  zmq_connect (sub_obj->subscriber, forwarder_address);

  /* Subscribe to default group: world */
  gchar *filter =   g_strdup_printf("%s", sub_obj->group_hash);
  zmq_setsockopt (sub_obj->subscriber, ZMQ_SUBSCRIBE, filter  , strlen(filter));
  /* Set high water mark to control number of messages buffered for subscribers */
  rc = zmq_setsockopt (sub_obj->subscriber, ZMQ_HWM, &hwm, sizeof (hwm));

  g_print("Receiving data from forwarder %s for group %s \n",forwarder_address, filter);

  g_free(filter);
  g_free(forwarder_address);
  return sub_obj;
}

void receive_data(subObject *sub_obj)
{
  while(1)
    {
      /* Receive data from forwarder using magical s_recv fn from z_helpers.h */
      gchar *string = s_recv (sub_obj->subscriber);
      gint covariance;
      gint count;
      char group[40];
      sscanf (string, "%s %d %d", group, &count, &covariance);
      printf("Received: group %s user %d covariance %d\n", sub_obj->group_hash, count, covariance);
      g_free (string);
      g_usleep(10);
    }
}

void free_sub_object(subObject *sub_obj)
{
  zmq_close (sub_obj->subscriber);
  zmq_term (sub_obj->context);
  g_free(sub_obj->host);
  g_free(sub_obj);  
}
