#include "NodeStack.h"

#include <assert.h>
#include <stdlib.h>

void node_stack_destroy(NodeStack* s) {
  assert(s);
  while (s != NULL) {
    NodeStack* tmp = s->next;
    free(s);
    s = tmp;
  }
}

void node_stack_push(NodeStack** s, Node* node) {
  NodeStack* e = malloc(sizeof(NodeStack));
  e->node = node;
  e->next = *s;
  *s = e;
}
void node_stack_pop(NodeStack** s) {
  assert(s);
  NodeStack* tmp = *s;
  *s = tmp->next;
  free(tmp);
}
Node* node_stack_top(NodeStack* s) { return s->node; }
