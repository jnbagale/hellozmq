
#include <glib.h>


typedef struct {

  void *context;
  void *subscriber;
  void *requester;
  void *publisher;
  gchar *group_hash;
  gchar *user_hash;
  gboolean publish;

  gchar *server;
  gint port;
  
} clientObject;


clientObject *make_client_object(void);
void free_client_object(clientObject *client_obj);
