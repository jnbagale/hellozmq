

#include <glib.h>


typedef struct {

  void *context;
  void *frontend;
  void *backend;
  gchar *group_hash;
  gchar *user_hash;
  gchar *host;
  gint port;
} serverObject;

serverObject *make_server_object(void);
void free_server_object(serverObject *server_obj);
void start_forwarder(serverObject *server_obj);
