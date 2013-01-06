// License: GPLv3
// Copyright 2012 The Clashing Rocks
// team@theclashingrocks.org

/* ZeroMQ Forwarder which receives data from publishers and sends it back to subscribers */
/* Binds PUB socket to tcp://\*:5556 */
/* Binds SUB socket to given host address */
/* Publishes covariance data */



#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <zmq.h>

#include "config.h"
#include "forwarder.h"

static int zmq_custom_broker(brokerObject *broker_obj);

int send_message(void *socket, char *message, int message_length, int send_more) 
{
  int rc;
  zmq_msg_t z_message;

  if((rc = zmq_msg_init_size (&z_message, message_length)) == -1){
    printf("Error occurred during zmq_msg_init_size(): %s", zmq_strerror (errno));
    return rc;
  }

  memcpy (zmq_msg_data (&z_message), message, message_length);
  if(send_more) {
    if((rc = zmq_send (socket, &z_message, ZMQ_SNDMORE)) == -1){
      printf("Error occurred during zmq_send(): %s", zmq_strerror (errno));
    }
  }
  else {
    if((rc = zmq_send (socket, &z_message, 0)) == -1){
      printf("Error occurred during zmq_send(): %s", zmq_strerror (errno));
    }
  }

  zmq_msg_close (&z_message);
  printf("size of message sent: %d bytes\n",message_length);
  return rc;
}

char *receive_message(void *socket, int *message_length) 
{
  int rc;
  int size;
  *message_length = -1;
  char *message = NULL;
  zmq_msg_t z_message;

  if((rc = zmq_msg_init (&z_message)) == -1){
    printf("Error occurred during zmq_msg_init_size(): %s", zmq_strerror (errno));
    return NULL;
  }

  if((rc = zmq_recv(socket, &z_message, 0)) == -1){
    printf("Error occurred during zmq_recv(): %s", zmq_strerror (errno));
    return NULL;
  }

  size = zmq_msg_size (&z_message);
  if(size > 0) {
    if((message = malloc(size + 1)) == NULL){
      printf("Failed to allocated message");
      return NULL;
    }
    memcpy (message, zmq_msg_data (&z_message), size);
    zmq_msg_close (&z_message);
    message [size] = '\0';   
    *message_length = size;
  }
  printf("size of message received: %d bytes\n",size);
  return message;
}

static int zmq_custom_broker(brokerObject *broker_obj)
{
  //  Subscribe for everything
  zmq_setsockopt (broker_obj->frontend, ZMQ_SUBSCRIBE, "", 0); 
  
  while (1)
    {
      int hasMore = 1;
 
      while (hasMore)
	{
	  int size;
	  int64_t more;
	  char *message = NULL;
	  size_t more_size = sizeof (more);

	  /* receive message from publishers */
	  message = receive_message(broker_obj->frontend, &size);

	  /* check if more message is present on the socket */
	  zmq_getsockopt (broker_obj->frontend, ZMQ_RCVMORE, &more, &more_size);

	  /* forward the message to the subscribers */
	  send_message(broker_obj->backend, message, size, more ? 1 : 0);
	  hasMore = more;

	  free(message);
	}
    }

  return -1;
} 

brokerObject *make_broker_object(void)
{
  brokerObject *broker_obj;


  if ((broker_obj = (brokerObject *)g_malloc(sizeof(brokerObject))) == NULL) {
    //g_printerr("failed to malloc brokerObject!");
    exit(EXIT_FAILURE);
  }

  return broker_obj;
}


void start_forwarder(brokerObject *broker_obj)
{
  // To subscriber to all the publishers
  gchar *frontend_endpoint = g_strdup_printf("tcp://*:%d",broker_obj->pub_port);

  //  This is our public IP address and port
  gchar *backend_endpoint =  g_strdup_printf("tcp://%s:%d",broker_obj->host, broker_obj->sub_port);

  //  Prepare context and sockets
  broker_obj->context  = zmq_init (1);
  broker_obj->frontend  = zmq_socket (broker_obj->context, ZMQ_SUB);
  broker_obj->backend = zmq_socket (broker_obj->context, ZMQ_PUB);

  zmq_bind (broker_obj->frontend,  frontend_endpoint);
  zmq_bind (broker_obj->backend, backend_endpoint);

  g_print("\nForwarder device is receiving at %s\n", frontend_endpoint);
  g_print("\nForwarder device is sending from %s\n", backend_endpoint);

  //  Start the forwarder device
  //  zmq_device (ZMQ_FORWARDER, broker_obj->frontend, broker_obj->backend);
  zmq_custom_broker(broker_obj);
}

void free_broker_object(brokerObject *broker_obj)
{
  zmq_term (broker_obj->context);
  g_free(broker_obj->group);
  g_free(broker_obj->host);
  g_free(broker_obj);  
}
/* End of forwarder.c */
