
#include<stdlib.h>
#include <zmq.h>

#include "config.h"
#include "client_con.h"

clientObject *make_client_object(void)
{
  clientObject *client_obj;

  if ((client_obj = (clientObject *)g_malloc(sizeof(clientObject))) == NULL) {
    //g_printerr("failed to malloc clientObject!");
    exit(EXIT_FAILURE);
  }

  client_obj->publish = TRUE;

  return client_obj;
}


void free_client_object(clientObject *client_obj)
{
  zmq_close (client_obj->subscriber);
  zmq_term (client_obj->context);
  g_free(client_obj);  
}
