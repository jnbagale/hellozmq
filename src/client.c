/* A ZeroMQ client who acts as both publisher and subscriber */
/* Connects SUB socket to forwarder's out socket address */
/* Connects PUB socket to forwarder's in socket address */

#include <zmq.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include <glib/gthread.h>
#include <uuid/uuid.h>

#include "zhelpers.h"
#include "config.h"
#include "subscriber.h"
#include "publisher.h"


int main (int argc, char *argv [])
{
  GMainLoop *mainloop;
  GError *error;
  uuid_t buf;
  gchar id[36];
  gchar *user_hash;
  gchar *group_hash;
  gchar *group_name_fix;
  gchar *group = DEFAULT_GROUP;
  gchar *server = DEFAULT_SERVER;
  gint port = DEFAULT_PORT;
  gboolean verbose = FALSE;
  GOptionContext *context;
  subObject *sub_obj = NULL;  
  pubObject *pub_obj = NULL;


  GOptionEntry entries[] = 
  {
    { "verbose", 'v', 0, G_OPTION_ARG_NONE, &verbose, "Verbose output", NULL },
    { "group", 'g', 0, G_OPTION_ARG_STRING, &group, "zeromq group", NULL },
    { "server", 's', 0, G_OPTION_ARG_STRING, &server, "zeromq server", NULL },
    { "port", 'p', 0, G_OPTION_ARG_INT, &port, "zeromq port", "N" },

    { NULL }
  };
 

  context = g_option_context_new ("- hello zero mq");
  g_option_context_add_main_entries (context, entries, PACKAGE_NAME);
  
  if (!g_option_context_parse (context, &argc, &argv, &error)) {
    g_printerr("option parsing failed: %s\n", error->message);
    exit (EXIT_FAILURE);
  }

  sub_obj = make_sub_object();
  pub_obj = make_pub_object();

  sub_obj->server =  g_strdup_printf("%s",server);
  sub_obj->port = port;

  /* Initialising thread */
  g_thread_init(NULL);
  
  uuid_generate_random(buf);
  uuid_unparse(buf, id);
  /* generate a hash of a unique id */
  user_hash = g_compute_checksum_for_string(G_CHECKSUM_MD5, id, strlen(id));

  /* generate a hash of the group name */
  group_hash = g_compute_checksum_for_string(G_CHECKSUM_MD5, group, strlen(group));
  /* 32 len strings don't seem to work so this is a fix */
  group_name_fix = g_strndup(group_hash, 31);
  
  sub_obj->group_hash = g_strdup_printf("%s", group_hash);
  sub_obj->user_hash =  g_strdup_printf("%s", user_hash);

  g_free(user_hash);
  g_free(group_hash);
  
  sub_obj = subscribe_forwarder(sub_obj);

  /* Initialise mainloop */
  mainloop = g_main_loop_new(NULL, FALSE);

  if (mainloop == NULL) {
    g_printerr("Couldn't create GMainLoop\n");
    exit(EXIT_FAILURE);
  }

  if( g_thread_create( (GThreadFunc) receive_data, (gpointer) sub_obj, FALSE, &error) == NULL) {
      g_printerr("option parsing failed1: %s\n", error->message);
  exit (EXIT_FAILURE);
  }
 
  if(pub_obj->publish) {
    pub_obj = publish_forwarder(pub_obj);
 
    if( g_thread_create( (GThreadFunc) send_data, (gpointer) pub_obj, FALSE, &error) == NULL ) {
      g_printerr("option parsing failed 2: %s\n", error->message);
      exit (EXIT_FAILURE);
    }
  }

  g_main_loop_run(mainloop);
  
  return EXIT_FAILURE;
}
