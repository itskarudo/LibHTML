#ifndef __PARSER_H__
#define __PARSER_H__

#include "DOM/Node.h"
#include "DOM/NodeStack.h"
#include "Tokenizer.h"

#define ENUMERATE_INSERTION_MODES               \
    __ENUMERATE_INSERTION_MODE(Initial)         \
    __ENUMERATE_INSERTION_MODE(BeforeHTML)      \
    __ENUMERATE_INSERTION_MODE(BeforeHead)      \
    __ENUMERATE_INSERTION_MODE(InHead)          \
    __ENUMERATE_INSERTION_MODE(InHeadNoscript)  \
    __ENUMERATE_INSERTION_MODE(AfterHead)       \
    __ENUMERATE_INSERTION_MODE(InBody)          \
    __ENUMERATE_INSERTION_MODE(Text)            \
    __ENUMERATE_INSERTION_MODE(InTable)         \
    __ENUMERATE_INSERTION_MODE(InTableText)     \
    __ENUMERATE_INSERTION_MODE(InCaption)       \
    __ENUMERATE_INSERTION_MODE(InColumnGroup)   \
    __ENUMERATE_INSERTION_MODE(InTableBody)     \
    __ENUMERATE_INSERTION_MODE(InRow)           \
    __ENUMERATE_INSERTION_MODE(InCell)          \
    __ENUMERATE_INSERTION_MODE(InSelect)        \
    __ENUMERATE_INSERTION_MODE(InSelectInTable) \
    __ENUMERATE_INSERTION_MODE(InTemplate)      \
    __ENUMERATE_INSERTION_MODE(AfterBody)       \
    __ENUMERATE_INSERTION_MODE(InFrameset)      \
    __ENUMERATE_INSERTION_MODE(AfterFrameset)   \
    __ENUMERATE_INSERTION_MODE(AfterAfterBody)  \
    __ENUMERATE_INSERTION_MODE(AfterAfterFrameset)

enum InsertionMode {
#define __ENUMERATE_INSERTION_MODE(x) x,
    ENUMERATE_INSERTION_MODES
#undef __ENUMERATE_INSERTION_MODE
};

struct AdjustedInsertionLocation {
    Node *parent, *before_node;
};

typedef struct {
    Tokenizer* tokenizer;
    enum InsertionMode insertion_mode;
    enum InsertionMode original_insertion_mode;
    Node* doc;
    Node* head_element;
    NodeStack* stack_of_open_elements;
} Parser;
Parser* parser_create(char* source);
void parser_destroy(Parser*);
void parser_run(Parser*);

#endif /* __PARSER_H__ */
