#ifndef __NODE_STACK_H__
#define __NODE_STACK_H__

#include "Node.h"
typedef struct NodeStack {
  Node* node;
  struct NodeStack* next;
} NodeStack;

void node_stack_destroy(NodeStack*);
void node_stack_push(NodeStack**, Node*);
void node_stack_pop(NodeStack**);
Node* node_stack_top(NodeStack*);

#endif /* __NODE_STACK_H__ */
