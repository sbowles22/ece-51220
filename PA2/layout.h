#include "ctree.h"

#ifndef LAYOUT_HEADER
#define LAYOUT_HEADER

typedef struct {
    CTree* ct;
    Option* optimal;
} Layout;

Layout* layout_create(void);
void layout_destruct(Layout* lout);

int layout_clear(Layout* layout);

int layout_no_choice(Layout* layout);
int layout_with_choice(Layout* layout);

void ol_merge(CTree* node);

#endif
