#ifndef CTREE_HEADER
#define CTREE_HEADER

typedef struct _Onode {
    int h;
    int w;

    // Placement
    struct _Tnode* ctnode; 
    int x;
    int y;
    struct _Onode* left;
    struct _Onode* right;
} Option;

typedef struct _OL {
    int ops;
    Option** olist;
} OptionList;

typedef struct _OLStack {
    int size;
    struct _OLSNode* head;
} OLStack;

typedef struct _OLSNode {
    Option* option;
    struct _OLSNode* next;
} OLSNode;

typedef enum {
    VERTICAL, HORIZONTAL, BLOCK, NONE
} CutType;

typedef struct _Tnode {
    int id;
    CutType type;

    struct _Tnode* left;
    struct _Tnode* right;

    OptionList options;
} CTree;

typedef struct _CTLnode {
    struct _CTLnode* next;
    CTree* root;
} CTreeList;

typedef struct {
    CTreeList* head;
} CTreeStack;

void ol_destruct(OptionList ol);
void ol_sort_by_width(OptionList ol);
void ol_sort_by_height(OptionList ol);
Option* option_create(int w, int h, CTree* ctnode);

// void qsort(void *base, size_t nitems, size_t size, int (*compar)(const void *, const void*))
int option_cmp_width (const void * a, const void * b);
int option_cmp_height (const void * a, const void * b);

#ifdef DEBUG
void ol_print(OptionList ol);
#endif

OLStack* ols_create(void);
void ols_destruct(OLStack* ols);
void ols_push(OLStack* ols, Option* option);
Option* ols_pop(OLStack* ols);
int ols_is_empty(OLStack* ols);

CTree* ct_create_node(void);
void ct_destruct(CTree* head);

#ifdef DEBUG
void ct_print(CTree* head);
#endif

void ctl_destruct(CTreeList* ctl);

CTreeStack* ct_stack_create(void);
void ct_stack_destruct(CTreeStack* cts);

int ct_stack_is_empty(CTreeStack* cts);
void ct_stack_push(CTreeStack* cts, CTree* root);
CTree* ct_stack_pop(CTreeStack* cts);

#endif
