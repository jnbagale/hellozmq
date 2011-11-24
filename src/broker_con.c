
#include<stdlib.h>
#include <zmq.h>

#include "config.h"
#include "broker_con.h"

brokerObject *make_broker_object(void)
{
  brokerObject *broker_obj;


  if ((broker_obj = (brokerObject *)g_malloc(sizeof(brokerObject))) == NULL) {
    //g_printerr("failed to malloc brokerObject!");
    exit(EXIT_FAILURE);
  }

  return broker_obj;
}


void free_broker_object(brokerObject *broker_obj)
{
  zmq_close (broker_obj->publisher);
  zmq_term (broker_obj->context);
  g_free(broker_obj->group_hash);
  g_free(broker_obj->user_hash);
  g_free(broker_obj);  
}
