#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include "ctree.h"
#include "math.h"

/////////////////
// OPTION LIST //
/////////////////

void ol_destruct(OptionList ol) {
    if (ol.olist == NULL) {
        return;
    }

    for (int i = 0; i < ol.ops; i++) {
        if (ol.olist[i] != NULL) {
            free(ol.olist[i]);
        }
    }

    free(ol.olist);

    ol.ops = 0;
    ol.olist = NULL;
}

// OptionList* ol_node_create(Option* option) {
//     OptionList* node = malloc(sizeof(OptionList));
//     if (node == NULL) {
//         return NULL;
//     }

//     node -> next = NULL;
//     node -> option = option;

//     return node;
// }

Option* option_create(int w, int h, CTree* ctnode) {
    Option* op = malloc(sizeof(Option));
    if (op == NULL) {
        return NULL;
    }

    op -> w = w;
    op -> h = h;

    op -> ctnode = ctnode;
    op -> x = INT_MIN;
    op -> y = INT_MIN;
    op -> left = NULL;
    op -> right = NULL;

    return op;
}

// void ol_pareto_filter(OptionList ol) {
//     ol_sort_by_width(ol);

//     int h_filter = ol.olist[0] -> h;

//     int new_ops = 0;
//     for (int i = 1; i < ol.ops; i++) {

//     }
// }

// void qsort(void *base, size_t nitems, size_t size, int (*compar)(const void *, const void*))

int option_cmp_width (const void * a, const void * b) {
    int wa = ((Option**) a)[0] -> w;
    int wb = ((Option**) b)[0] -> w;

    // printf("%p %p\n", a, b);
    // printf("%d - %d: %d\n", wa, wb, wa-wb);
    return wa - wb;
}

int option_cmp_height (const void * a, const void * b) {
    int ha = ((Option**) a)[0] -> h;
    int hb = ((Option**) b)[0] -> h;
    
    return ha - hb;
}

void ol_sort_by_width(OptionList ol) {
    qsort(ol.olist, ol.ops, sizeof(ol.olist[0]), option_cmp_width);
}

void ol_sort_by_height(OptionList ol) {
    qsort(ol.olist, ol.ops, sizeof(ol.olist[0]), option_cmp_height);
}

// OptionList* ol_destroy_node(OptionList* ol, OptionList* p, OptionList* q) {
//     if (q == NULL) {
//         return ol;
//     }
    
//     if (p != NULL) {
//         p -> next = q -> next;
//     }
//     if (q == ol) {
//         ol = q -> next;
//     }

//     free(q -> option);
//     free(q);

//     return ol;
// }

#ifdef DEBUG
void ol_print(OptionList ol) {
    if (ol.olist == NULL) {
        return;
    }

    for (int i = 0; i < ol.ops; i++) {
        printf("(%2d,%2d)", ol.olist[i] -> w, ol.olist[i] -> h);
        if (i != ol.ops - 1) {
            printf(", ");
        }
    }
}
#endif

///////////////////////
// OPTION LIST STACK //
///////////////////////

OLStack* ols_create(void) {
    OLStack* ols = malloc(sizeof(OLStack));
    if (ols == NULL) {
        return NULL;
    }

    ols -> head = NULL;
    ols -> size = 0;
    return ols;
}

void ols_destruct(OLStack* ols) {
    Option* option;
    while (!ols_is_empty(ols)) {
        option = ols_pop(ols);
        if (option != NULL) {
            free(option);
        }
    }

    free(ols);
}

void ols_push(OLStack* ols, Option* option) {
    OLSNode* ols_node = malloc(sizeof(OLSNode));
    if (ols_node == NULL) {
        return;
    }

    ols_node -> next = ols -> head;
    ols_node -> option = option;

    ols -> head = ols_node;
    (ols -> size)++;

}

Option* ols_pop(OLStack* ols) {

    OLSNode* ols_node = ols -> head;
    Option* option = ols_node -> option;

    ols -> head = ols_node -> next;
    free(ols_node);
    (ols -> size)--;
    
    return option;
}

int ols_is_empty(OLStack* ols) {
    return (ols->head == NULL);
}

/////////////////////
// CONSTRAINT TREE //
/////////////////////

CTree* ct_create_node(void) {
    CTree* node = malloc(sizeof(CTree));
    if (node == NULL) {
        return NULL;
    }

    node -> id = 0;
    node -> type = NONE;
    node -> left = NULL;
    node -> right = NULL;
    node -> options.olist = NULL;
    node -> options.ops = 0;

    return node;
}

void ct_destruct(CTree* head) {
    if (head == NULL) {
        return;
    }

    ol_destruct(head -> options);
    
    ct_destruct(head -> left);
    ct_destruct(head -> right);
    free(head);
}

#ifdef DEBUG
void ct_print(CTree* head) {
    if (head == NULL) {
        return;
    }

    switch (head -> type) {
        case VERTICAL:      printf("%4d | V, L: %4d, R: %4d | ", 
                                head -> id, head -> left -> id, head -> right -> id);
                            ol_print(head -> options);
                            printf("\n");
                            break;
        case HORIZONTAL:    printf("%4d | H, L: %4d, R: %4d | ", 
                                head -> id, head -> left -> id, head -> right -> id);
                            ol_print(head -> options);
                            printf("\n");
                            break;
        case BLOCK:         printf("%4d | B, ", head -> id);
                            ol_print(head -> options);
                            printf("\n");
                            break;
        case NONE:          printf("ERROR, None constraint node detected\n");
    }

    ct_print(head -> left);
    ct_print(head -> right);
}
#endif

//////////////////////////
// CONSTRAINT TREE LIST //
//////////////////////////

void ctl_destruct(CTreeList* ctl) {
    CTreeList* next;
    while (ctl != NULL) {
        next = ctl -> next;
        ct_destruct(ctl -> root);
        free(ctl);
        ctl = next;
    }
}

///////////////////////////
// CONSTRAINT TREE STACK //
///////////////////////////

CTreeStack* ct_stack_create(void) {
    CTreeStack* cts = malloc(sizeof(CTreeStack));
    if (cts == NULL) {
        return NULL;
    }

    cts -> head = NULL;

    return cts;
}

void ct_stack_destruct(CTreeStack* cts) {
    ctl_destruct(cts -> head);
    free(cts); 
}

int ct_stack_is_empty(CTreeStack* cts) {
    return (cts -> head) == NULL;
}

void ct_stack_push(CTreeStack* cts, CTree* root) {
    CTreeList* new = malloc(sizeof(CTreeList));
    // TODO: Possible memory error

    new -> next = cts -> head;
    new -> root = root;

    cts -> head = new;
}

CTree* ct_stack_pop(CTreeStack* cts) {
    CTreeList* node = cts -> head;
    CTree* ct = node -> root;

    cts -> head = node -> next;
    free(node);

    return ct;
}
