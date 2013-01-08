
/* ZeroMQ Forwarder which receives data from publishers and sends it back to subscribers */
/* Binds PUB socket to tcp://\*:5556 */
/* Binds SUB socket to given host address */
/* Publishes covariance data */


#include <zmq.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#include "config.h"
#include "forwarder.h"

static int verbose_flag;
static void print_usage(void);

static void print_usage(void)
{
  printf("usage: broker --group <group name> --host <host name> --subport <sub port number> --pubport <pub port number> \n");
  exit(EXIT_SUCCESS);
}

int main (int argc, char *argv[])
{
  int c;
  char *group = DEFAULT_GROUP;
  char *host = DEFAULT_HOST;
  int sub_port = DEFAULT_SUB_PORT;
  int pub_port = DEFAULT_PUB_PORT;
  brokerObject *broker_obj = NULL;

  /* creating a broker object structure and assigning broker's network address and port numbers */
  broker_obj = make_broker_object();

  while(1) {
    static struct option long_options[] =
      {
        {"verbose", no_argument, &verbose_flag, 1},
        {"help",  no_argument, 0, 'h'},
        {"group",  required_argument, 0, 'g'},
        {"broker",  required_argument, 0, 'b'},
        {"subport",  required_argument, 0, 's'},
        {"pubport",  required_argument, 0, 'p'},        
        {0, 0, 0, 0}
      };

    int option_index = 0;
    c = getopt_long (argc, argv, "hg:b:s:p:?", long_options, &option_index);
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

  /* assign values to various variables */
  broker_obj->sub_port = sub_port;
  broker_obj->pub_port = pub_port;
  broker_obj->host =  strdup(host);
  broker_obj->group =  strdup(group);

  start_forwarder(broker_obj);

  /* We should never reach here unless something goes wrong! */
  return EXIT_FAILURE;
}
