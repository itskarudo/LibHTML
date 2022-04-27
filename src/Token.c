#include "Token.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Token* token_create(enum TokenType type) {
  Token* token = malloc(sizeof(Token));
  token->type = type;
  token->self_closing = false;

  token->value = calloc(1, sizeof(char));
  attribute_list_init(&token->attributes);

  return token;
}
void token_destroy(Token* token) {
  attribute_list_destroy(&token->attributes);
  free(token->value);
  free(token);
}

void print_token(Token* token) {
  switch (token->type) {
#define __ENUMERATE_TYPE(type)                              \
  case type: {                                              \
    printf("Token(%s): \"%s\" { ", #type, token->value);    \
    for (int i = 0; i < token->attributes.size; i++) {      \
      printf("%s=\"%s\", ", token->attributes.data[i].name, \
             token->attributes.data[i].value);              \
    }                                                       \
    printf("} %s\n", token->self_closing ? "(SC)" : "");    \
    break;                                                  \
  }
    ENUMERATE_TYPES
#undef __ENUMERATE_TYPE
  }
}

void token_value_append(Token* token, char c) {
  token->value =
      realloc(token->value, (strlen(token->value) + 2) * sizeof(char));
  strcat(token->value, (char[]){c, 0});
}

