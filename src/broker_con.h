
#include <glib.h>


typedef struct {

  void *context;
  void *publisher;
  void *responder;
  void *subscriber;
  void *frontend;
  void *backend;

  gchar *group_hash;
  gchar *user_hash;
  gchar *server;
  gint port;
  
} brokerObject;


brokerObject *make_broker_object(void);
void free_broker_object(brokerObject *broker_obj);
