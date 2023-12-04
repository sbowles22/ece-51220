//****************************************************************************//
// FILE INPUT AND OUTPUT FUNCTIONS
//****************************************************************************//

#include <stdlib.h>
#include <stdio.h>
#include "layout.h"
#include "ctree.h"
#include "file_io.h"

////////////////
// FILE INPUT //
////////////////

CTree* load_ct_from_file(char* filename) {
    CTree* ct = NULL;
    
    FILE* f = fopen(filename, "r");
    if (f == NULL) {
        return NULL;
    }

    CTree* node; 
    CTreeStack* cts = ct_stack_create();
    if (cts == NULL) {
        fclose(f);
        return NULL;
    }

    // Option* option = NULL;
    int ol_fp_start;
    int ol_size;
    int w;
    int h;
    while (!feof(f)) {
        node = ct_create_node();
        switch (fpeek(f)) {
            case 'V':   node -> type = VERTICAL;
                        break;
            case 'H':   node -> type = HORIZONTAL;
                        break;
            default:    node -> type = BLOCK;
        }
        switch (node -> type) {
            case VERTICAL:      fscanf(f, "V\n");
                                break;
            case HORIZONTAL:    fscanf(f, "H\n");
                                break;
            case BLOCK:         fscanf(f, "%d(", &(node -> id));
                                ol_fp_start = ftell(f);
                                ol_size = 0;
                                while (fpeek(f) != ')') {
                                    fscanf(f, "(%d,%d)", &w, &h);
                                    ol_size++;
                                }
                                node -> options.ops = ol_size;
                                node -> options.olist = malloc(ol_size * sizeof(Option*));
                                fseek(f, ol_fp_start, SEEK_SET);
                                for (int i = 0; i < ol_size; i++) {
                                    fscanf(f, "(%d,%d)", &w, &h);
                                    node -> options.olist[i] = option_create(w, h, node); // TODO: Switch with Queue
                                }
                                fscanf(f, ")\n");
                                break;
            default:            break;
        }

        switch (node -> type) {
            case VERTICAL:      
            case HORIZONTAL:    node -> right = ct_stack_pop(cts);
                                node -> left = ct_stack_pop(cts);
                                break;
            default: break;
        }

        ct_stack_push(cts, node);
    }

    ct = ct_stack_pop(cts);

    if (!ct_stack_is_empty(cts)) {
        ct_destruct(ct);
        ct_stack_destruct(cts);
        fclose(f);
        return NULL;
    }

    ct_stack_destruct(cts);
    fclose(f);
    return ct;
}

/////////////////
// FILE OUTPUT //
/////////////////

int write_area_to_file(char* filename, Layout* layout) {
    FILE* f = fopen(filename, "w");
    if (f == NULL) {
        return -1;
    }

    fprintf(f, "(%d,%d)\n", layout -> optimal -> w, layout -> optimal -> h);
    
    fclose(f);
    return 0;
}

int write_layout_to_file(char* filename, Layout* layout) {
    FILE* f = fopen(filename, "w");
    if (f == NULL) {
        return -1;
    }

    _write_layout_to_file(f, layout -> optimal, 0, 0);

    fclose(f);
    return 0;
}

void _write_layout_to_file(FILE* f, Option* option, int x, int y) {
    if (option == NULL) {
        return;
    }

    switch (option -> ctnode -> type) {
        case VERTICAL:      _write_layout_to_file(f, option -> left,  x,                       y);
                            _write_layout_to_file(f, option -> right, x + option -> left -> w, y);
                            break;
        case HORIZONTAL:    _write_layout_to_file(f, option -> left,  x, y + option -> right -> h);
                            _write_layout_to_file(f, option -> right, x, y                       );
                            break;
        case BLOCK:         fprintf(f,"%d((%d,%d)(%d,%d))\n", option -> ctnode -> id, option -> w, option -> h, x, y);
                            break;
        default:            break;
    }
}

/////////////////////
// FILE IO UTILITY //
/////////////////////

int fpeek(FILE *stream)
{
    int c;

    c = fgetc(stream);
    ungetc(c, stream);

    return c;
}
