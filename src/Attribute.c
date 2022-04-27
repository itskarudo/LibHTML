#include "Attribute.h"

#include <stdlib.h>
#include <string.h>

static void attribute_init(Attribute* attr) {
  attr->name = calloc(1, sizeof(char));
  attr->value = calloc(1, sizeof(char));
}

void attribute_append_to_name(Attribute* attr, char c) {
  attr->name = realloc(attr->name, (strlen(attr->name) + 2) * sizeof(char));
  strcat(attr->name, (char[]){c, 0});
}

void attribute_append_to_value(Attribute* attr, char c) {
  attr->value = realloc(attr->value, (strlen(attr->value) + 2) * sizeof(char));
  strcat(attr->value, (char[]){c, 0});
}

void attribute_list_init(AttributeList* al) {
  al->data = NULL;
  al->size = 0;
}

static void attribute_list_resize(AttributeList* al, size_t size) {
  if (size > al->size) {
    size_t allocsize = (size) * sizeof(Attribute);
    al->data = (Attribute*)realloc(al->data, allocsize);
  }

  al->size = size;
}

void attribute_list_add_attribute(AttributeList* al) {
  attribute_list_resize(al, al->size + 1);
  attribute_init(&al->data[al->size - 1]);
}

Attribute* attribute_list_last(AttributeList* al) {
  return &al->data[al->size - 1];
}

Attribute* attribute_list_at(AttributeList* al, size_t index) {
  return &al->data[index];
}

void attribute_list_destroy(AttributeList* al) {
  for (int i = 0; i < al->size; i++) {
    free(al->data[i].name);
    free(al->data[i].value);
  }
  free(al->data);
}
