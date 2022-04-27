#include <assert.h>
#include <stdio.h>

#include "Token.h"
#include "Tokenizer.h"

int main(int argc, char* argv[]) {
  FILE* f = fopen("../test.html", "r");

  fseek(f, 0, SEEK_END);
  int fsz = ftell(f);
  fseek(f, 0, SEEK_SET);

  char src[fsz + 1];

  int rd = fread(src, 1, fsz, f);
  fclose(f);
  src[rd] = 0;

  Tokenizer* tokenizer = tokenizer_create(src);
  for (Token* token = tokenizer_next_token(tokenizer); token->type != EOFType;
       token = tokenizer_next_token(tokenizer)) {
    print_token(token);

    token_destroy(token);
  }

  tokenizer_destroy(tokenizer);

  return 0;
}
