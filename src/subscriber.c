// License: GPLv3
// Copyright 2012 The Clashing Rocks
// team@theclashingrocks.org

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <inttypes.h>
#include <zmq.h>

#include "config.h"
#include "message.h"
#include "subscriber.h"

subObject *make_sub_object(void)
{
  subObject *sub_obj;

  if ((sub_obj = (subObject *)malloc(sizeof(subObject))) == NULL) {
    //printf("failed to malloc subObject!");
    exit(EXIT_FAILURE);
  }

  return sub_obj;
}

subObject *subscribe_forwarder(subObject *sub_obj)
{
  uint64_t hwm = 100; 
  sub_obj->context = zmq_init (1);

  char *forwarder_address = malloc(1000);
  sprintf(forwarder_address, "tcp://%s:%d", sub_obj->host, sub_obj->port);

   /* Socket to subscribe to forwarder */
  sub_obj->subscriber = zmq_socket (sub_obj->context, ZMQ_SUB);
  zmq_connect (sub_obj->subscriber, forwarder_address);

  /* Subscribe to default group: world */
  char *filter =  strdup(sub_obj->group_id);
  zmq_setsockopt (sub_obj->subscriber, ZMQ_SUBSCRIBE, filter, strlen(filter));
  /* Set high water mark to control number of messages buffered for subscribers */
  zmq_setsockopt (sub_obj->subscriber, ZMQ_HWM, &hwm, sizeof (hwm));

  printf("Receiving data from forwarder: %s for group: %s \n",forwarder_address, filter);

  free(filter);
  free(forwarder_address);
  return sub_obj;
}

void *receive_data(void *sub_obj)
{  
  int size;
  int count;
  char user[40];
  char group[40];
  subObject *sub_obj1 = (subObject *)sub_obj;
  
  while(1)
    {
      /* Receive data sent by publishers via forwarder */
      char *string = receive_message (sub_obj1->subscriber, &size);
  
      sscanf (string, "%s %s %d", group, user, &count);
      printf("Received: group:- %s user:- %s count:- %d\n", group, user, count);
      free (string);
      usleep(10);
    }
}

void free_sub_object(subObject *sub_obj)
{
  zmq_close (sub_obj->subscriber);
  zmq_term (sub_obj->context);
  free(sub_obj->group_id);
  free(sub_obj->user_id);
  free(sub_obj->host);
  free(sub_obj);  
}
