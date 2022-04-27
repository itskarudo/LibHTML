#ifndef __TOKEN_H__
#define __TOKEN_H__

#include <stdbool.h>

#include "Attribute.h"

#define ENUMERATE_TYPES           \
  __ENUMERATE_TYPE(DocTypeType)   \
  __ENUMERATE_TYPE(StartTagType)  \
  __ENUMERATE_TYPE(EndTagType)    \
  __ENUMERATE_TYPE(CharacterType) \
  __ENUMERATE_TYPE(EOFType)

enum TokenType {
#define __ENUMERATE_TYPE(x) x,
  ENUMERATE_TYPES
#undef __ENUMERATE_TYPE
};

typedef struct {
  char* value;
  enum TokenType type;
  bool self_closing;
  AttributeList attributes;
} Token;

Token* token_create(enum TokenType);
void token_destroy(Token*);
void token_value_append(Token*, char c);
void print_token(Token*);

static inline void token_add_attribute(Token* token) {
  attribute_list_add_attribute(&token->attributes);
}
static inline void token_append_to_attribute_name(Token* token, char c) {
  attribute_append_to_name(attribute_list_last(&token->attributes), c);
}

static inline void token_append_to_attribute_value(Token* token, char c) {
  attribute_append_to_value(attribute_list_last(&token->attributes), c);
}

#endif /* __TOKEN_H__ */
