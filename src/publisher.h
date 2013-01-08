
typedef struct {
  void *context;
  void *publisher;
  char *group_id;
  char *user_id;
  char *host;
  int port;
  int publish;  
} pubObject;

pubObject *make_pub_object(void);
void free_pub_object(pubObject *pub_obj);
pubObject *publish_forwarder(pubObject *pub_obj);
void *send_data(void *pub_obj);
