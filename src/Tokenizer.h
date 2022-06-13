#ifndef __TOKENIZER_H__
#define __TOKENIZER_H__

#include "Token.h"
#define ENUMERATE_STATES                        \
  __ENUMERATE_STATE(Data)                       \
  __ENUMERATE_STATE(TagOpen)                    \
  __ENUMERATE_STATE(MarkupDeclaration)          \
  __ENUMERATE_STATE(DocType)                    \
  __ENUMERATE_STATE(BeforeDocTypeName)          \
  __ENUMERATE_STATE(DocTypeName)                \
  __ENUMERATE_STATE(TagName)                    \
  __ENUMERATE_STATE(EndTagOpen)                 \
  __ENUMERATE_STATE(BeforeAttributeName)        \
  __ENUMERATE_STATE(AttributeName)              \
  __ENUMERATE_STATE(AfterAttributeName)         \
  __ENUMERATE_STATE(BeforeAttributeValue)       \
  __ENUMERATE_STATE(AttributeValueDoubleQuoted) \
  __ENUMERATE_STATE(AttributeValueSingleQuoted) \
  __ENUMERATE_STATE(AttributeValueUnquoted)     \
  __ENUMERATE_STATE(AfterAttributeValue)        \
  __ENUMERATE_STATE(SelfClosingStartTag)        \
  __ENUMERATE_STATE(RCDATA)                     \
  __ENUMERATE_STATE(RCDATALessThanSign)         \
  __ENUMERATE_STATE(RCDATAEndTagOpen)           \
  __ENUMERATE_STATE(RCDATAEndTagName)

enum TokenizerState {
#define __ENUMERATE_STATE(x) x,
  ENUMERATE_STATES
#undef __ENUMERATE_STATE
};

typedef struct {
  enum TokenizerState state;
  char* source;
  int cursor;
  Token* current_token;
} Tokenizer;

Tokenizer* tokenizer_create(char* source);
void tokenizer_destroy(Tokenizer*);
Token* tokenizer_next_token(Tokenizer*);

#endif /* __TOKENIZER_H__ */
