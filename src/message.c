// License: GPLv3
// Copyright 2012 The Clashing Rocks
// team@theclashingrocks.org

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zmq.h>

#include "config.h"
#include "message.h"

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
