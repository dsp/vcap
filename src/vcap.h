#ifndef __VCAP_H__
#define __VCAP_H__ 1

#define IDENT_MAX 25

#include <pthread.h>
#include <glib.h>

// #define VCAP_DATA_CALLBACK(f) void (*f)(const struct vcap_data_entry* entry, void* userp)

struct vcap_data_entry
{
  struct vcap_data_entry *parent;
  struct vcap_data_entry **entries;
  unsigned int ecount;
  unsigned long amount;
  unsigned int level;
  char *ident;
};

typedef struct vcap_data_entry vcap_data_entry_t;

extern char errbuff[];
extern pthread_mutex_t vcap_data_mutex;

const struct vcap_data_entry *vcap_data_root (const struct vcap_data_entry *);
struct vcap_data_entry *vcap_data_lookup (const char *);
struct vcap_data_entry *vcap_data_create (const char *,
										  struct vcap_data_entry *);

void vcap_data_foreach (struct vcap_data_entry * n, void (*fp)(struct vcap_data_entry*, void*), void *userp);
void vcap_data_add (struct vcap_data_entry *, unsigned int);
GList *vcap_data_raw_list (void);

#define VCAP_DATA_INC(n) vcap_data_add(n,1)
#define VCAP_LOCK_DATA_TSAFE()  pthread_mutex_lock(&vcap_data_mutex)

#define VCAP_UNLOCK_DATA_TSAFE()  pthread_mutex_unlock(&vcap_data_mutex)

#define VCAP_DATA_CREATE(n,i,p)					\
  if (n == NULL) {								 \
	n = vcap_data_create(i,vcap_data_lookup(p)); \
  }

#endif /* __VCAP_H__ */
