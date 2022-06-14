#include "Node.h"

#include <stdio.h>
#include <stdlib.h>

static inline void safe_free(void* ptr) {
  if (ptr) free(ptr);
}

Node* node_create(enum NodeType type, Node* parent)

{
  Node* node = malloc(sizeof(Node));
  node->type = type;
  node->parent = parent;
  node->first_child = node->last_child = node->next_sibling =
      node->prev_sibling = NULL;

  switch (type) {
    case ElementNode: {
      node->data.element.prefix = NULL;
      node->data.element.local_name = NULL;
      node->data.element.id = NULL;
      node->data.element.class_name = NULL;
      break;
    }
    case DocTypeNode: {
      node->data.doctype.name = NULL;
      node->data.doctype.public_id = NULL;
      node->data.doctype.system_id = NULL;
      break;
    }
    case TextNode: {
      node->data.character.data = NULL;
      break;
    }
    default:
      break;
  }

  return node;
}
void node_destroy(Node* node) {
  while (node->first_child != NULL) {
    Node* tmp = node->first_child->next_sibling;
    node_destroy(node->first_child);
    node->first_child = tmp;
  }

  switch (node->type) {
    case ElementNode: {
      safe_free(node->data.element.prefix);
      safe_free(node->data.element.local_name);
      safe_free(node->data.element.id);
      safe_free(node->data.element.class_name);
      break;
    }
    case DocTypeNode: {
      safe_free(node->data.doctype.name);
      safe_free(node->data.doctype.public_id);
      safe_free(node->data.doctype.system_id);
      break;
    }
    case TextNode: {
      safe_free(node->data.character.data);
      break;
    }
    default:
      break;
  }

  free(node);
}

void node_append_child(Node* parent, Node* node) {
  if (parent->first_child == NULL) parent->first_child = node;
  if (parent->last_child) parent->last_child->next_sibling = node;
  node->prev_sibling = parent->last_child;
  parent->last_child = node;
}
void node_insert_before(Node* node, Node* sibling) {
  if (sibling->parent->first_child == sibling)
    sibling->parent->first_child = node;

  if (sibling->prev_sibling) sibling->prev_sibling->next_sibling = node;
  node->prev_sibling = sibling->prev_sibling;

  sibling->prev_sibling = node;
  node->next_sibling = sibling;
}
void print_node(Node* node, int indent) {
  printf("%*s", indent, "");
  switch (node->type) {
    case DocumentNode:
      printf("Node<Document>\n");
      break;
    case DocTypeNode:
      printf("Node<DocType>(%s)\n", node->data.doctype.name);
      break;
    case ElementNode:
      printf("Node<Element>(%s)\n", node->data.element.local_name);
      break;
    case TextNode:
      printf("%s\n", node->data.character.data);
      break;
  }

  for (Node* child = node->first_child; child; child = child->next_sibling)
    print_node(child, indent + 2);
}
