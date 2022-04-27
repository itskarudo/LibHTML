#ifndef __ATTRIBUTE_H__
#define __ATTRIBUTE_H__

#include <stdlib.h>

typedef struct {
  char* name;
  char* value;
} Attribute;

typedef struct {
  Attribute* data;
  size_t size;
} AttributeList;

void attribute_list_init(AttributeList*);
void attribute_list_add_attribute(AttributeList* al);
Attribute* attribute_list_at(AttributeList*, size_t index);
Attribute* attribute_list_last(AttributeList*);
void attribute_list_destroy(AttributeList*);

void attribute_append_to_name(Attribute*, char c);
void attribute_append_to_value(Attribute*, char c);

#endif /* __ATTRIBUTE_H__ */
