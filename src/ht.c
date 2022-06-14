#include <assert.h>
#include <stdio.h>

#include "Parser.h"

int main(int argc, char* argv[]) {
  FILE* f = fopen("../test.html", "r");

  fseek(f, 0, SEEK_END);
  int fsz = ftell(f);
  fseek(f, 0, SEEK_SET);

  char src[fsz + 1];

  int rd = fread(src, 1, fsz, f);
  fclose(f);
  src[rd] = 0;

  Parser* parser = parser_create(src);
  parser_run(parser);

  parser_destroy(parser);

  return 0;
}
