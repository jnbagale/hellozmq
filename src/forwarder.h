

typedef struct {
  void *context;
  void *frontend;
  void *backend;
  int sub_port;
  int pub_port;
  char *host;
  char *group;
} brokerObject;

brokerObject *make_broker_object(void);
void free_broker_object(brokerObject *broker_obj);
void start_forwarder(brokerObject *broker_obj);
