#include <stdlib.h>
#include <stdio.h>
#include "layout.h"
#include "ctree.h"
#include "utils.h"

Layout* layout_create(void) {
    Layout* layout = malloc(sizeof(Layout));
    if (layout == NULL) {
        return NULL;
    }

    layout -> ct = NULL;
    layout -> optimal = NULL;

    return layout;
}

void layout_destruct(Layout* lout) {
    if (lout == NULL) {
        return;
    }

    ct_destruct(lout -> ct);

    free(lout);
}

int layout_clear(Layout* layout) {
    CTreeStack* cts = ct_stack_create();
    if (cts == NULL) {
        return -1;
    }

    // Clear all nodes to stack
    CTree* node;
    ct_stack_push(cts, layout -> ct);
    while (!ct_stack_is_empty(cts)) {
        node = ct_stack_pop(cts);
        if (node -> type == VERTICAL || node -> type == HORIZONTAL) {
            ct_stack_push(cts, node -> left);
            ct_stack_push(cts, node -> right);

            ol_destruct(node -> options);
        }
    }

    layout -> optimal = NULL;

    ct_stack_destruct(cts);

    return 0;
}

int layout_no_choice(Layout* layout) {
    CTreeStack* cts_temp = ct_stack_create();
    if (cts_temp == NULL) {
        return -1;
    }
    
    CTreeStack* cts = ct_stack_create();
    if (cts_temp == NULL) {
        return -1;
    }

    // Find leftmost block node for traversal
    CTree* first = layout -> ct;
    while (first -> left != NULL) {
        first = first -> left;
    }

    // Consider its placement [bottom left] as (0,0)
    first -> options.olist[0] -> x = 0;
    first -> options.olist[0] -> y = 0;

    // Add all nodes to stack
    CTree* node;
    ct_stack_push(cts_temp, layout -> ct);
    while (!ct_stack_is_empty(cts_temp)) {
        node = ct_stack_pop(cts_temp);
        if (node -> type == VERTICAL || node -> type == HORIZONTAL) {
            ct_stack_push(cts_temp, node -> left);
            ct_stack_push(cts_temp, node -> right);
        }
        ct_stack_push(cts, node);
    }

    ct_stack_destruct(cts_temp);

    // cts now has all nodes in post-order fashion
    int w;
    int h;
    Option* option;

    while (!ct_stack_is_empty(cts)) {
        node = ct_stack_pop(cts);
        switch (node -> type) {
        case VERTICAL:      w = (node -> left -> options.olist[0] -> w) + (node -> right -> options.olist[0] -> w);
                            h = max(node -> left -> options.olist[0] -> h, node -> right -> options.olist[0] -> h);
                            option = option_create(w, h, node);

                            option -> x = node -> left -> options.olist[0] -> x + node -> left -> options.olist[0] -> w;
                            option -> y = node -> left -> options.olist[0] -> y;
                            option -> left = node -> left -> options.olist[0];
                            option -> right = node -> right -> options.olist[0];

                            node -> options.olist = malloc(sizeof(Option**));
                            node -> options.olist[0] = option;
                            node -> options.ops = 1;
                            break;
        case HORIZONTAL:    w = max(node -> left -> options.olist[0] -> w, node -> right -> options.olist[0] -> w);
                            h = (node -> left -> options.olist[0] -> h) + (node -> right -> options.olist[0] -> h);
                            option = option_create(w, h, node);

                            option -> x = node -> left -> options.olist[0] -> x;
                            option -> y = node -> left -> options.olist[0] -> y + node -> left -> options.olist[0] -> h;
                            option -> left = node -> left -> options.olist[0];
                            option -> right = node -> right -> options.olist[0];

                            node -> options.olist = malloc(sizeof(Option**));
                            node -> options.olist[0] = option;
                            node -> options.ops = 1;
                            break;
        default:            break;
        }
    }
    ct_stack_destruct(cts);

    layout -> optimal = layout -> ct -> options.olist[0];
    return 0;
}

int layout_with_choice(Layout* layout) {
    if (layout_clear(layout) < 0) {
        return -1;
    }

    CTreeStack* cts_temp = ct_stack_create();
    if (cts_temp == NULL) {
        return -1;
    }
    
    CTreeStack* cts = ct_stack_create();
    if (cts_temp == NULL) {
        return -1;
    }

    // Find leftmost block node for traversal
    CTree* first = layout -> ct;
    while (first -> left != NULL) {
        first = first -> left;
    }

    // Consider its placement [bottom left] as (0,0)
    for (int i = 0; i < first -> options.ops; i++) {
        first -> options.olist[i] -> x = 0;
        first -> options.olist[i] -> y = 0;
    }

    // Add all nodes to stack
    CTree* node;
    ct_stack_push(cts_temp, layout -> ct);
    while (!ct_stack_is_empty(cts_temp)) {
        node = ct_stack_pop(cts_temp);
        if (node -> type == VERTICAL || node -> type == HORIZONTAL) {
            ct_stack_push(cts_temp, node -> left);
            ct_stack_push(cts_temp, node -> right);
        }
        ct_stack_push(cts, node);
    }

    ct_stack_destruct(cts_temp);

    // cts now has all nodes in post-order fashion
    while (!ct_stack_is_empty(cts)) {
        node = ct_stack_pop(cts);
        ol_merge(node);
    }
    ct_stack_destruct(cts);

#ifdef DEBUG
    ol_print(layout -> ct -> options);
    printf("\n");
#endif

    // Determine Optimal Layout by Minimum Area
    OptionList top_olist = layout -> ct -> options;
    layout -> optimal = top_olist.olist[0];
    long optimal_area = layout -> optimal -> w * layout -> optimal -> h;
    for (int i = 1; i < top_olist.ops; i++) {
        if (top_olist.olist[i] -> w * top_olist.olist[i] -> h < optimal_area) {
            layout -> optimal = top_olist.olist[i];
            optimal_area = top_olist.olist[i] -> w * top_olist.olist[i] -> h;
        }
    }  

    return 0;
}

void ol_merge(CTree* node) {
    if (node -> type == BLOCK) {
        return;
    }

    OLStack* ols = ols_create();
    if (ols == NULL) {
        return;
    }

    Option** left_ol = node -> left -> options.olist;
    Option** right_ol = node -> right -> options.olist;
    int left_ops = node -> left -> options.ops;
    int right_ops = node -> right -> options.ops;
    int lx = 0;
    int rx = 0;
    int next_lx = lx;
    int next_rx = rx;

    Option* option;
    int w;
    int h;
    int x;
    int y;

    switch (node -> type) {
        case VERTICAL:
            ol_sort_by_width(node -> left -> options);
            ol_sort_by_width(node -> right -> options);
        break;
        case HORIZONTAL:
            ol_sort_by_height(node -> left -> options);
            ol_sort_by_height(node -> right -> options);
        break;
        default: break;
    }

    while (lx < left_ops && rx < right_ops) {
        switch (node -> type) {
            case VERTICAL:
                w = (left_ol[lx] -> w) + (right_ol[rx] -> w);
                if (left_ol[lx] -> h > right_ol[rx] -> h) {
                    h = left_ol[lx] -> h;
                    next_lx = lx + 1;
                } 
                else if (left_ol[lx] -> h < right_ol[rx] -> h) {
                    h = right_ol[rx] -> h;
                    next_rx = rx + 1;
                } 
                else {
                    h = left_ol[lx] -> h;
                    next_lx = lx + 1;
                    next_rx = rx + 1;
                }

                x = left_ol[lx] -> x + left_ol[lx] -> w;
                y = left_ol[lx] -> y;
            break;
            case HORIZONTAL:
                h = (left_ol[lx] -> h) + (right_ol[rx] -> h);
                if (left_ol[lx] -> w > right_ol[rx] -> w) {
                    w = left_ol[lx] -> w;
                    next_lx = lx + 1;
                } 
                else if (left_ol[lx] -> w < right_ol[rx] -> w) {
                    w = right_ol[rx] -> w;
                    next_rx = rx + 1;
                } 
                else {
                    w = left_ol[lx] -> w;
                    next_lx = lx + 1;
                    next_rx = rx + 1;
                }

                y = left_ol[lx] -> y + left_ol[lx] -> h;
                x = left_ol[lx] -> x;
            break;
            default: break;
        }

        option = option_create(w, h, node);

        option -> x = x;
        option -> y = y;
        option -> left = left_ol[lx];
        option -> right = right_ol[rx];

        lx = next_lx;
        rx = next_rx;

        ols_push(ols, option);
    }

    node -> options.ops = ols -> size;
    node -> options.olist = malloc(ols -> size * sizeof(Option*));
    for (int i = ols -> size - 1; i >= 0; i--) {
        node -> options.olist[i] = ols_pop(ols);
    }

    ols_destruct(ols);
}

    // int w;
    // int h;
    // Option* option;
    // OptionList* ol_new_head;

        // switch (node -> type) {
        // case VERTICAL:

        // // TODO FIX
        // for (OptionList* left_op = node -> left -> options; left_op != NULL; left_op = left_op -> next) {
        //     for (OptionList* right_op = node -> right -> options; right_op != NULL; right_op = right_op -> next) {
        //         w = (left_op -> option -> w) + (right_op -> option -> w);
        //         h = max(left_op -> option -> h, right_op -> option -> h);
        //         option = option_create(w, h, node);

        //         option -> x = left_op -> option -> x + left_op -> option -> w;
        //         option -> y = left_op -> option -> y;
        //         option -> left = left_op -> option;
        //         option -> right = right_op -> option;

        //         ol_new_head = ol_node_create(option);
        //         ol_new_head -> next = node -> options;
        //         node -> options = ol_new_head;
        //     }
        // }
        // node -> options = ol_pareto_filter(node -> options);

        //                     break;
        // case HORIZONTAL:    
        
        // for (OptionList* left_op = node -> left -> options; left_op != NULL; left_op = left_op -> next) {
        //     for (OptionList* right_op = node -> right -> options; right_op != NULL; right_op = right_op -> next) {
        //         w = max(left_op -> option -> w, right_op -> option -> w);
        //         h = (left_op -> option -> h) + (right_op -> option -> h);
        //         option = option_create(w, h, node);

        //         option -> x = left_op -> option -> x;
        //         option -> y = left_op -> option -> y + left_op -> option -> h;
        //         option -> left = left_op -> option;
        //         option -> right = right_op -> option;

        //         ol_new_head = ol_node_create(option);
        //         ol_new_head -> next = node -> options;
        //         node -> options = ol_new_head;
        //     }
        // }
        // node -> options = ol_pareto_filter(node -> options);

        //                     break;
        // default:            break;
        // }
