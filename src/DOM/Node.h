#ifndef __NODE_H__
#define __NODE_H__

enum NodeType { DocumentNode, DocTypeNode, ElementNode, TextNode };

typedef struct {
  char* prefix;
  char* local_name;
  char* id;
  char* class_name;
} element_data;

typedef struct {
  char* name;
  char* public_id;
  char* system_id;
} doctype_data;

typedef struct {
  char* data;
} character_data;

typedef struct Node {
  enum NodeType type;
  struct Node *parent, *first_child, *last_child, *next_sibling, *prev_sibling;

  union {
    element_data element;
    doctype_data doctype;
    character_data character;
  } data;
} Node;

Node* node_create(enum NodeType, Node* parent);
void node_destroy(Node*);
void node_append_child(Node* parent, Node*);
void node_insert_before(Node*, Node* sibling);
void print_node(Node*, int indent);

#endif /* __NODE_H__ */
