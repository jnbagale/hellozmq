/* A ZeroMQ client who acts as both publisher and subscriber */
/* Connects SUB socket to forwarder's out socket address */
/* Connects PUB socket to forwarder's in socket address */

#include <zmq.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <getopt.h>
#include <uuid/uuid.h>

#include "config.h"
#include "publisher.h"
#include "subscriber.h"

static int verbose_flag;
static void print_usage(void);

static void print_usage(void)
{
  printf("usage: node --group <group name> --broker <broker host name> --type <pub or sub or both> --subport <sub port number> --pubport <pub port number> \n");
  exit(EXIT_SUCCESS);
}

int main (int argc, char *argv [])
{
  int c;
  uuid_t buf;
  char id[36];
  char *group = DEFAULT_GROUP;
  char *host = DEFAULT_HOST;
  char *type = DEFAULT_TYPE;
  int sub_port = DEFAULT_SUB_PORT;
  int pub_port = DEFAULT_PUB_PORT;
  subObject *sub_obj = NULL;  
  pubObject *pub_obj = NULL;
  pthread_t thread_sub;
  pthread_t thread_pub;

  /* creating a subscriber and publisher object structure and assigning broker's network address and port numbers */
  sub_obj = make_sub_object();
  pub_obj = make_pub_object();

  while(1) {
    static struct option long_options[] =
      {
        {"verbose", no_argument, &verbose_flag, 1},
        {"help",  no_argument, 0, 'h'},
        {"group",  required_argument, 0, 'g'},
        {"broker",  required_argument, 0, 'b'},
	{"type",  required_argument, 0, 't'},
        {"subport",  required_argument, 0, 's'},
        {"pubport",  required_argument, 0, 'p'},        
        {0, 0, 0, 0}
      };

    int option_index = 0;
    c = getopt_long (argc, argv, "hg:b:t:s:p:?", long_options, &option_index);
    if (c == -1) break;
    switch (c)
      {
      case 0:
        if (long_options[option_index].flag != 0) break;
        printf ("option %s", long_options[option_index].name);
        if (optarg) printf (" with arg %s", optarg);
        printf ("\n");
        break;
      case 'h':
        print_usage();
        break;  
      case 'g':
	group =  optarg;
        break;
      case 'b':
        host =  optarg;
        break;
      case 't':
	type =  optarg;
        break;
      case 's':
	sub_port = atoi(optarg);
        break;   
      case 'p':
	pub_port = atoi(optarg);
        break;
      case '?':
        print_usage();
        break;
      default:
        abort ();
      }
  }

  /* assigning default values */
  sub_obj->port = sub_port;
  pub_obj->port = pub_port;
  sub_obj->host = strdup(host);
  pub_obj->host = strdup(host);

  /* generate and assign unique user id */
  uuid_generate_random(buf);
  uuid_unparse(buf, id);

  sub_obj->group_id = strdup(group);
  sub_obj->user_id =  strdup(id);
  pub_obj->group_id = strdup(group);
  pub_obj->user_id =  strdup(id);
  
  if( (strcmp(type,"both") == 0) || (strcmp(type,"sub") == 0) ) {
    sub_obj = subscribe_forwarder(sub_obj);

    if (pthread_create( &thread_sub, NULL, receive_data,(void *) sub_obj)) {
      fprintf(stderr, "Error creating searcher thread \n");
      exit(EXIT_FAILURE);
    }
  }

  if( (strcmp(type,"both") == 0) || (strcmp(type,"pub") == 0) ) {
    pub_obj = publish_forwarder(pub_obj);

    if (pthread_create( &thread_pub, NULL, send_data,(void *) pub_obj)) {
      fprintf(stderr, "Error creating receiver thread \n");
      exit(EXIT_FAILURE);
    }
  }

  if( (strcmp(type,"both") == 0) || (strcmp(type,"sub") == 0) ) {
    /* Join the thread to subscribe  */
    if(pthread_join( thread_sub, NULL)) {
      fprintf(stderr, "Error joining subscriber thread \n");
      exit(EXIT_FAILURE);
    }
  }

  if( (strcmp(type,"both") == 0) || (strcmp(type,"pub") == 0) ) {
    /* Join the thread to publish */
    if(pthread_join( thread_pub, NULL)) {
      fprintf(stderr, "Error joining publisher thread \n");
      exit(EXIT_FAILURE);
    }
  }

  /* We should never reach here unless something goes wrong! */
  return EXIT_FAILURE;
}
