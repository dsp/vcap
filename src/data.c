#include "config.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>

#include "vcap.h"

static GList *m_index;
static struct vcap_data_entry root;

#define VDS struct vcap_data_entry *
static int
vcap_level (struct vcap_data_entry *entry)
{
  int level;

  if (entry == NULL)
    return -1;

  for (level = 0; entry->parent != NULL; level++)
	{
	  entry = entry->parent;
	}

  return level;
}

static gint
vcap_data_sort(gpointer a, gpointer b)
{
  vcap_data_entry_t *e = (vcap_data_entry_t *)a;
  vcap_data_entry_t *f = (vcap_data_entry_t *)b;  

  if (e->level > f->level) 
	return -1;

  if (e->level < f->level)
	return 1;
  
  return 0;

}

struct vcap_data_entry *
vcap_data_create (const char *ident, struct vcap_data_entry *parent)
{
  struct vcap_data_entry * lookup;
  lookup = vcap_data_lookup (ident);

  if (lookup != NULL)
    return lookup;

  struct vcap_data_entry *new;

  VCAP_LOCK_DATA_TSAFE ();
  new = (struct vcap_data_entry *) malloc (sizeof (struct vcap_data_entry));
  memset (new, 0, sizeof (struct vcap_data_entry));

  new->ident  = strdup (ident);
  new->amount = 0;
  new->parent = (parent == NULL) ? &root : parent;
  new->level  = vcap_level (new);
  new->parent->ecount++;
  /* this is somewhat dumb, we have to make it better in the <future> */
  new->parent->entries =
	realloc (new->parent->entries,
			 sizeof (struct vcap_data_entry *) * new->parent->ecount);
  new->parent->entries[new->parent->ecount - 1] = new;

  /* update our search index */
  m_index = g_list_insert_sorted (m_index, new, vcap_data_sort);

  VCAP_UNLOCK_DATA_TSAFE ();
  return new;
}

/* dumb sequential search */
struct vcap_data_entry *
vcap_data_lookup (const char *ident)
{
  GList *ptr;
  struct vcap_data_entry *res;

  if (m_index == NULL)
    {
      return NULL;
    }

  ptr = g_list_first (m_index);
  do
    {
      res = (struct vcap_data_entry *) ptr->data;

      if (strncmp (res->ident, ident, IDENT_MAX) == 0)
		{
		  return res;
		}
    }
  while ((ptr = g_list_next (ptr)) != NULL);

  return NULL;
}

void
vcap_data_foreach (struct vcap_data_entry * n, void (*fp)(struct vcap_data_entry*, void*), void *userp)
{
  struct vcap_data_entry * curr;
  unsigned int i;

  if (n != NULL) 
	{
	  fp(n, userp);
	  curr = n;
	}
  else 
	{
	  curr = &root;
	}
  
  
  for (i = 0; i < curr->ecount; i++)
	{
	  vcap_data_foreach(curr->entries[i], fp, userp);	  
	}

}

void
vcap_data_add (struct vcap_data_entry *entry, unsigned int add)
{
  if (entry == NULL)
    return;

  VCAP_LOCK_DATA_TSAFE ();
  entry->amount += add;
  VCAP_UNLOCK_DATA_TSAFE ();
}

const struct vcap_data_entry *
vcap_data_root(const struct vcap_data_entry * entry)
{
  if (entry == NULL)
	return &root;

  while(entry && entry->parent != NULL && entry->parent != &root)
	{
	  entry = entry->parent;
	}
  return entry;
}

GList *
vcap_data_raw_list (void)
{
  return m_index;
}
