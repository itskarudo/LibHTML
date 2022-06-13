#include "Tokenizer.h"

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "Token.h"

#define BEGIN_STATE(state) \
  state:                   \
  case state:

#define CURRENT_CHARACTER t->source[t->cursor]

#define SWITCH_TO(st)               \
  do {                              \
    tokenizer_consume_character(t); \
    t->state = st;                  \
    goto st;                        \
  } while (0)

#define RECONSUME_IN(st) \
  do {                   \
    t->state = st;       \
    goto st;             \
  } while (0)

#define EMIT_CURRENT_TOKEN           \
  do {                               \
    Token* token = t->current_token; \
    return token;                    \
  } while (0)

#define EMIT_TOKEN_AND_SWITCH_TO(st) \
  do {                               \
    tokenizer_consume_character(t);  \
    t->state = st;                   \
    Token* token = t->current_token; \
    t->current_token = NULL;         \
    return token;                    \
  } while (0)

#define ON(c) if (CURRENT_CHARACTER == c)

#define ON_ASCII_ALPHA if (isalpha(CURRENT_CHARACTER))
#define ON_ASCII_LOWER_ALPHA if (islower(CURRENT_CHARACTER))

#define ANYTHING_ELSE if (1)

#define ON_WHITESPACE                                               \
  if ((CURRENT_CHARACTER == '\t') || (CURRENT_CHARACTER == '\n') || \
      (CURRENT_CHARACTER == ' ') || (CURRENT_CHARACTER == '\f'))

/**
 * TODO:
 * since we're using a source string instead of a character stream,
 * "EOF" would be the null byte at the string end,
 * in practice this sould be an EOF value if we switch to using a FILE* stream
 */

#define ON_EOF if (CURRENT_CHARACTER == 0)

Tokenizer* tokenizer_create(char* source) {
  Tokenizer* tokenizer = malloc(sizeof(Tokenizer));
  tokenizer->source = source;
  tokenizer->state = Data;
  tokenizer->cursor = -1;

  tokenizer->current_token = NULL;
  return tokenizer;
}

static bool on_string(Tokenizer* t, char* str) {
  int i = 0;
  for (i = 0; str[i]; i++)
    if (t->source[t->cursor + i] != str[i]) return false;

  t->cursor += i - 1;
  return true;
}

static inline void tokenizer_consume_character(Tokenizer* t) { t->cursor++; }

Token* tokenizer_next_token(Tokenizer* t) {
  t->current_token = NULL;
  for (;;) {
    tokenizer_consume_character(t);
    switch (t->state) {
      BEGIN_STATE(Data) {
        ON('<') { SWITCH_TO(TagOpen); }

        ON_WHITESPACE continue;

        ON_EOF {
          t->current_token = token_create(EOFType);
          EMIT_CURRENT_TOKEN;
        }

        ANYTHING_ELSE {
          t->current_token = token_create(CharacterType);
          token_value_append(t->current_token, CURRENT_CHARACTER);
          EMIT_CURRENT_TOKEN;
        }

        break;
      }

      BEGIN_STATE(TagOpen) {
        ON('!') { SWITCH_TO(MarkupDeclaration); }

        ON('/') { SWITCH_TO(EndTagOpen); }

        ON_ASCII_ALPHA {
          t->current_token = token_create(StartTagType);
          RECONSUME_IN(TagName);
        }

        break;
      }

      BEGIN_STATE(TagName) {
        ON('>') { EMIT_TOKEN_AND_SWITCH_TO(Data); }

        ON('/') { SWITCH_TO(SelfClosingStartTag); }

        ON_WHITESPACE { SWITCH_TO(BeforeAttributeName); }
        ANYTHING_ELSE {
          token_value_append(t->current_token, CURRENT_CHARACTER);
        }
        break;
      }

      BEGIN_STATE(SelfClosingStartTag) {
        ON('>') {
          t->current_token->self_closing = true;
          EMIT_TOKEN_AND_SWITCH_TO(Data);
        }
      }

      BEGIN_STATE(BeforeAttributeName) {
        ON_WHITESPACE continue;

        ON('>') { RECONSUME_IN(AfterAttributeName); }

        ON('/') { SWITCH_TO(SelfClosingStartTag); }

        ANYTHING_ELSE {
          token_add_attribute(t->current_token);
          RECONSUME_IN(AttributeName);
        }

        break;
      }

      BEGIN_STATE(AttributeName) {
        ON_WHITESPACE { RECONSUME_IN(AfterAttributeName); }

        ON('>') { RECONSUME_IN(AfterAttributeName); }
        ON('=') { SWITCH_TO(BeforeAttributeValue); }

        ANYTHING_ELSE {
          token_append_to_attribute_name(t->current_token, CURRENT_CHARACTER);
        }

        break;
      }

      BEGIN_STATE(AfterAttributeName) {
        ON_WHITESPACE continue;

        ON('>') { EMIT_TOKEN_AND_SWITCH_TO(Data); }

        ON('/') { SWITCH_TO(SelfClosingStartTag); }

        break;
      }

      BEGIN_STATE(BeforeAttributeValue) {
        ON_WHITESPACE continue;

        ON('\'') { SWITCH_TO(AttributeValueSingleQuoted); }
        ON('"') { SWITCH_TO(AttributeValueDoubleQuoted); }

        break;
      }

      BEGIN_STATE(AttributeValueSingleQuoted) {
        ON('\'') { SWITCH_TO(AfterAttributeValue); }

        ANYTHING_ELSE {
          token_append_to_attribute_value(t->current_token, CURRENT_CHARACTER);
        }

        break;
      }

      BEGIN_STATE(AttributeValueDoubleQuoted) {
        ON('"') { SWITCH_TO(AfterAttributeValue); }

        ANYTHING_ELSE {
          token_append_to_attribute_value(t->current_token, CURRENT_CHARACTER);
        }

        break;
      }

      BEGIN_STATE(AfterAttributeValue) {
        ON_WHITESPACE { SWITCH_TO(BeforeAttributeName); }

        ON('>') { EMIT_TOKEN_AND_SWITCH_TO(Data); }

        ON('/') { SWITCH_TO(SelfClosingStartTag); }

        break;
      }

      BEGIN_STATE(EndTagOpen) {
        ON_ASCII_ALPHA {
          t->current_token = token_create(EndTagType);
          RECONSUME_IN(TagName);
        }

        break;
      }

      BEGIN_STATE(MarkupDeclaration) {
        if (on_string(t, "DOCTYPE")) {
          SWITCH_TO(DocType);
        }
        break;
      }

      BEGIN_STATE(DocType) {
        ON_WHITESPACE { SWITCH_TO(BeforeDocTypeName); }

        break;
      }

      BEGIN_STATE(BeforeDocTypeName) {
        ANYTHING_ELSE {
          t->current_token = token_create(DocTypeType);
          token_value_append(t->current_token, CURRENT_CHARACTER);
          SWITCH_TO(DocTypeName);
        }

        break;
      }

      BEGIN_STATE(DocTypeName) {
        ON('>') { EMIT_TOKEN_AND_SWITCH_TO(Data); }
        ANYTHING_ELSE {
          token_value_append(t->current_token, CURRENT_CHARACTER);
        }
        break;
      }

      BEGIN_STATE(RCDATA) {
        ON('<') { SWITCH_TO(RCDATALessThanSign); }

        ANYTHING_ELSE {
          t->current_token = token_create(CharacterType);
          token_value_append(t->current_token, CURRENT_CHARACTER);
          EMIT_CURRENT_TOKEN;
        }
        break;
      }

      BEGIN_STATE(RCDATALessThanSign) {
        ON('/') { SWITCH_TO(RCDATAEndTagOpen); }

        ANYTHING_ELSE { RECONSUME_IN(RCDATA); }

        break;
      }

      BEGIN_STATE(RCDATAEndTagOpen) {
        ON_ASCII_ALPHA {
          t->current_token = token_create(EndTagType);
          RECONSUME_IN(RCDATAEndTagName);
        }

        break;
      }

      BEGIN_STATE(RCDATAEndTagName) {
        ON('>') { EMIT_TOKEN_AND_SWITCH_TO(Data); }

        ON_ASCII_LOWER_ALPHA {
          token_value_append(t->current_token, CURRENT_CHARACTER);
        }

        break;
      }
    }
  }
}

void tokenizer_destroy(Tokenizer* tokenizer) {
  if (tokenizer->current_token != NULL) token_destroy(tokenizer->current_token);
  free(tokenizer);
}

