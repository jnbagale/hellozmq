
/* ZeroMQ Forwarder which receives data from publishers and sends it back to subscribers */
/* Binds PUB socket to tcp://\*:5556 */
/* Binds SUB socket to given host address */
/* Publishes covariance data */


#include <zmq.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include <glib/gthread.h>
#include <uuid/uuid.h>

#include "config.h"
#include "forwarder.h"

int main (int argc, char *argv[])
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
  serverObject *server_obj = NULL;
  GOptionEntry entries[] = 
  {
    { "verbose", 'v', 0, G_OPTION_ARG_NONE, &verbose, "Verbose output", NULL },
    { "group", 'g', 0, G_OPTION_ARG_STRING, &group, "zeromq group", NULL },
    { "server", 's', 0, G_OPTION_ARG_STRING, &server, "zeromq server", NULL },
    { "port", 'p', 0, G_OPTION_ARG_INT, &port, "zeromq port", "N" },

    { NULL }
  };
 

  context = g_option_context_new ("- hello zeromq");
  g_option_context_add_main_entries (context, entries, PACKAGE_NAME);
  
  if (!g_option_context_parse (context, &argc, &argv, &error)) {
    g_printerr("option parsing failed: %s\n", error->message);
    exit (EXIT_FAILURE);
  }

  /* creating a structure and assiging server and port addresses  */
  server_obj = make_server_object();
  server_obj->server =  g_strdup_printf("%s",server);
  server_obj->port = port;
  
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

  /* Store user and group hash to be sent to network */
  server_obj->user_hash = g_strdup_printf("%s",user_hash);
  server_obj->group_hash = g_strdup_printf("%s",group_hash);
  /* Clean up memory*/
  g_free(user_hash);
  g_free(group_hash);

  /* Initialising mainloop */
  mainloop = g_main_loop_new(NULL, FALSE);
  if (mainloop == NULL) {
    g_printerr("Couldn't create GMainLoop\n");
    exit(EXIT_FAILURE);
  }

  /* Run a thread to start the forwarder */
  if( g_thread_create( (GThreadFunc) start_forwarder, (gpointer) server_obj, FALSE, &error) == NULL ) {
       g_printerr("option parsing failed 2: %s\n", error->message);
   exit (EXIT_FAILURE);
  }

  g_main_loop_run(mainloop);

  /* We should never reach here unless something goes wrong! */
  return EXIT_FAILURE;
}