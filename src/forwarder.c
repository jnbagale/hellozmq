// License: GPLv3
// Copyright 2012 The Clashing Rocks
// team@theclashingrocks.org

/* ZeroMQ Forwarder which receives data from publishers and sends it back to subscribers */
/* Binds PUB socket to given host address or default tcp://127.0.0.1:8100 */
/* Binds SUB socket to given host address or default tcp://127.0.0.1:5556*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <zmq.h>

#include "config.h"
#include "forwarder.h"

brokerObject *make_broker_object(void)
{
  brokerObject *broker_obj;


  if ((broker_obj = (brokerObject *)malloc(sizeof(brokerObject))) == NULL) {
    //printf("failed to malloc brokerObject!");
    exit(EXIT_FAILURE);
  }

  return broker_obj;
}

void start_forwarder(brokerObject *broker_obj)
{
  // To subscriber to all the publishers
  char *frontend_endpoint;
  char *backend_endpoint;

  frontend_endpoint = malloc(1000);
  backend_endpoint = malloc(1000);

  sprintf(frontend_endpoint, "tcp://%s:%d", broker_obj->host, broker_obj->pub_port);
  sprintf(backend_endpoint, "tcp://%s:%d", broker_obj->host, broker_obj->sub_port);

  //  Prepare ZeroMQ context and sockets
  broker_obj->context = zmq_init (1);
  broker_obj->frontend = zmq_socket (broker_obj->context, ZMQ_SUB);
  broker_obj->backend = zmq_socket (broker_obj->context, ZMQ_PUB);

  //  Subscribe for everything
  zmq_setsockopt (broker_obj->frontend, ZMQ_SUBSCRIBE, "", 0); 
  
  zmq_bind (broker_obj->frontend,  frontend_endpoint);
  zmq_bind (broker_obj->backend, backend_endpoint);

  printf("\nForwarder device is receiving at %s\n", frontend_endpoint);
  printf("\nForwarder device is sending from %s\n", backend_endpoint);
 
  //  Start the forwarder device
  zmq_device (ZMQ_FORWARDER, broker_obj->frontend, broker_obj->backend);
}

void free_broker_object(brokerObject *broker_obj)
{
  zmq_close(broker_obj->backend);
  zmq_close(broker_obj->frontend);
  zmq_term (broker_obj->context);

  free(broker_obj->group);
  free(broker_obj->host);
  free(broker_obj);  
}
/* End of forwarder.c */
