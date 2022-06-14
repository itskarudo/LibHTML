#include "Parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "DOM/Node.h"
#include "DOM/NodeStack.h"
#include "Token.h"
#include "Tokenizer.h"

Parser* parser_create(char* source) {
  Parser* parser = malloc(sizeof(Parser));
  parser->tokenizer = tokenizer_create(source);
  parser->doc = node_create(DocumentNode, NULL);
  parser->head_element = NULL;

  parser->stack_of_open_elements = NULL;

  parser->insertion_mode = Initial;
  parser->original_insertion_mode = Initial;
  return parser;
}

void parser_destroy(Parser* parser) {
  tokenizer_destroy(parser->tokenizer);
  node_destroy(parser->doc);
  node_stack_destroy(parser->stack_of_open_elements);
  free(parser);
}

static struct AdjustedInsertionLocation get_approapriate_place_for_inserting(
    Parser* parser) {
  Node* target = node_stack_top(parser->stack_of_open_elements);
  // TODO: some foster parenting shenanigans
  struct AdjustedInsertionLocation place = {.parent = target,
                                            .before_node = NULL};
  return place;
}

static Node* insert_html_element(Parser* parser, Token* token) {
  Node* element =
      node_create(ElementNode, node_stack_top(parser->stack_of_open_elements));

  element->data.element.prefix = NULL;
  element->data.element.local_name = strdup(token->value);

  struct AdjustedInsertionLocation adjusted_insertion_location =
      get_approapriate_place_for_inserting(parser);

  if (adjusted_insertion_location.before_node == NULL)
    node_append_child(adjusted_insertion_location.parent, element);
  else
    node_insert_before(element, adjusted_insertion_location.before_node);

  node_stack_push(&parser->stack_of_open_elements, element);

  return element;
}

static void handle_initial(Parser* parser, Token* token) {
  if (token->type == DocTypeType) {
    Node* node = node_create(DocTypeNode, parser->doc);

    node->data.doctype.name = strdup(token->value);
    node->data.doctype.public_id = strdup("");
    node->data.doctype.system_id = strdup("");

    node_append_child(parser->doc, node);
    parser->insertion_mode = BeforeHTML;
  }
}

static void handle_before_html(Parser* parser, Token* token) {
  if ((token->type == StartTagType) && !strcmp(token->value, "html")) {
    Node* node = node_create(ElementNode, parser->doc);

    node->data.element.prefix = NULL;
    node->data.element.local_name = strdup(token->value);

    node_append_child(parser->doc, node);
    node_stack_push(&parser->stack_of_open_elements, node);
    parser->insertion_mode = BeforeHead;
  }
}

static void handle_before_head(Parser* parser, Token* token) {
  if ((token->type == StartTagType) && !strcmp(token->value, "head")) {
    Node* element = insert_html_element(parser, token);

    parser->head_element = element;
    parser->insertion_mode = InHead;
  }
}

static void handle_in_head(Parser* parser, Token* token) {
  if ((token->type == StartTagType) && !strcmp(token->value, "title")) {
    insert_html_element(parser, token);
    parser->tokenizer->state = RCDATA;
    parser->original_insertion_mode = InHead;
    parser->insertion_mode = Text;
  }

  if ((token->type == EndTagType) && !strcmp(token->value, "head")) {
    node_stack_pop(&parser->stack_of_open_elements);
    parser->insertion_mode = AfterHead;
  }
}

static void handle_text(Parser* parser, Token* token) {
  if (token->type == CharacterType) {
    struct AdjustedInsertionLocation adjusted_insertion_location =
        get_approapriate_place_for_inserting(parser);

    if (adjusted_insertion_location.parent->type == DocumentNode) return;

    Node* node_bfr =
        adjusted_insertion_location.before_node == NULL
            ? adjusted_insertion_location.parent->last_child
            : adjusted_insertion_location.before_node->prev_sibling;

    if (node_bfr && node_bfr->type == TextNode) {
      node_bfr->data.character.data =
          realloc(node_bfr->data.character.data,
                  (strlen(node_bfr->data.character.data) + 2) * sizeof(char));
      strcat(node_bfr->data.character.data, token->value);
    } else {
      Node* node = node_create(TextNode, adjusted_insertion_location.parent);
      node->data.character.data = strdup(token->value);
      node_append_child(adjusted_insertion_location.parent, node);
    }
  }

  if (token->type == EndTagType) {
    node_stack_pop(&parser->stack_of_open_elements);
    parser->insertion_mode = parser->original_insertion_mode;
  }
}

static void handle_after_head(Parser* parser, Token* token) {
  if ((token->type == StartTagType) && !strcmp(token->value, "body")) {
    insert_html_element(parser, token);
    parser->insertion_mode = InBody;
  }
}

static void handle_in_body(Parser* parser, Token* token) {
  if (token->type == StartTagType) {
    insert_html_element(parser, token);
    if (!strcmp(token->value, "img"))
      node_stack_pop(&parser->stack_of_open_elements);
  }

  if ((token->type == EndTagType) && !strcmp(token->value, "body"))
    parser->insertion_mode = AfterBody;
}

static void handle_after_body(Parser* parser, Token* token) {
  if ((token->type == EndTagType) && !strcmp(token->value, "html"))
    parser->insertion_mode = AfterAfterBody;
}

static void handle_after_after_body(Parser* parser, Token* token) {
  if (token->type == EOFType) return;
}

void parser_run(Parser* parser) {
  for (Token* token = tokenizer_next_token(parser->tokenizer);
       token->type != EOFType;
       token = tokenizer_next_token(parser->tokenizer)) {
    print_token(token);

    switch (parser->insertion_mode) {
      case Initial: {
        handle_initial(parser, token);
        break;
      }
      case BeforeHTML: {
        handle_before_html(parser, token);
        break;
      }
      case BeforeHead: {
        handle_before_head(parser, token);
        break;
      }
      case InHead: {
        handle_in_head(parser, token);
        break;
      }
      case Text: {
        handle_text(parser, token);
        break;
      }
      case AfterHead: {
        handle_after_head(parser, token);
        break;
      }
      case InBody: {
        handle_in_body(parser, token);
        break;
      }
      case AfterBody: {
        handle_after_body(parser, token);
        break;
      }
      case AfterAfterBody: {
        handle_after_after_body(parser, token);
        break;
      }
      default:
        break;
    }

    token_destroy(token);
  }

  print_node(parser->doc, 0);
}
