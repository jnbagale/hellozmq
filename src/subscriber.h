
typedef struct {
  void *context;
  void *subscriber;
  char *group_id;
  char *user_id;
  char *host;
  int port;
} subObject;

subObject *make_sub_object(void);
void free_sub_object(subObject *sub_obj);
subObject *subscribe_forwarder(subObject *sub_obj);
void *receive_data(void *sub_obj);
