/* implementation of unify algorithm               */
/* see http://arxiv.org/abs/cs/0603080 for details */

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "type.h"
#include "typestack.h"
#include "unify.h"

bool isvar(struct type *s) {
    return s && s->t==TYPE_VARIABLE;
}

bool unify(struct type *ix, struct type *iy) {
    struct typestack Sx,Sy;
    if (typestack_init(&Sx) || typestack_init(&Sy)) /*fail*/; 
    if (typestack_push(&Sx,ix)) /*fail*/;
    if (typestack_push(&Sy,iy)) /*fail*/;
    while (Sx.top && Sy.top /*not_empty(Sx) and not_empty(Sy)*/) {
        struct type *i=typestack_pop(&Sx);
        struct type *j=typestack_pop(&Sy);
        if (!isvar(i) && !isvar(j)) { // case 1: i is bound to a term and j is bound to a term}
            if (i->t==j->t && i->arity==j->arity
                    && (!i->t==TYPE_NAME || ((!i->name && !j->name) || !strcmp(i->name,j->name))/* type(i)=type(j)=NAME => name(i)=name(j)*/)) {
                if (i->arity > 0) {
                    for (int I=0;I<i->arity;I++) typestack_push(&Sx,i->args[I]);
                    for (int J=0;J<j->arity;J++) typestack_push(&Sy,j->args[J]);
                }
            } else {
                free(Sx.list);
                free(Sy.list);
                return false;
            }
        } else if (!isvar(i) && isvar(j)) { // case 2: i is bound to a term and j is bound to a variable
            struct type *b=j->instance;//find(mgu_begin,j->name); //dereference j
            if (!b/*j is a free variable*/) j->instance=i;//mgu_begin=append(mgu_begin,j->name,i); //bind j to i and set mgu[j] = 1
            else { // j is bound
                if (!isvar(b)) {
                    typestack_push(&Sx,i); //push i on Sx
                    typestack_push(&Sy,b); //push j on Sy
                } else { // j is bound to variable
                    struct type *bb=b->instance;//find(mgu_begin,b->term->name); //deref(deref(j))
                    if (bb) {
                        typestack_push(&Sx,i); //push i on Sx
                        typestack_push(&Sy,bb); //push j on Sy
                    } else j->instance=i;//mgu_begin=append(mgu_begin,j->name,i); // bind j to i
                }
            }
        } else if (isvar(i) && !isvar(j)) { // case 3: i is bound to a variable and j is bound to a term
            // perfectly symmetric to case 2
            struct type *b=i->instance;//find(mgu_begin,i->name); //dereference i
            if (!b/*i is a free variable*/) i->instance=j;//mgu_begin=append(mgu_begin,i->name,j); //bind i to j and set mgu[i] = 1
            else { // i is bound
                if (!isvar(b)) {
                    typestack_push(&Sy,j); //push j on Sx
                    typestack_push(&Sx,b);
                } else { // j is bound to a free variable
                    struct type *bb=b->instance;//find(mgu_begin,b->term->name); //deref(deref(j))
                    if (bb) {
                        typestack_push(&Sy,j); //push i on Sx
                        typestack_push(&Sx,bb); //push j on Sy
                    } else i->instance=j;//mgu_begin=append(mgu_begin,i->name,j); // bind j to i
                }
            }
        } else if (isvar(i) && isvar(j)) { // case 4: i is bound to a variable and j is bound to a variable
            struct type *fi=i->instance;//find(mgu_begin,i->name);
            struct type *fj=j->instance;//find(mgu_begin,j->name);
            if (!fi && !fj/*i is free and j is free*/) {
                i->instance=j;//mgu_begin=append(mgu_begin,i->name,j);
                //mgu_begin=append(mgu_begin,j->name,i);
            } else if (!fi && fj/*i is free and j is bound*/) {
                if (!isvar(fj) || strcmp(fj->name,i->name)) //if we do not bind X to deref(Y) if deref(Y)=X
                    i->instance=fj;//mgu_begin=append(mgu_begin,i->name,fj->term);
            } else if (fi && !fj/*i is bound and j is free*/) {
                if (!isvar(fi) || strcmp(fi->name,j->name))
                    j->instance=fi;//mgu_begin=append(mgu_begin,j->name,fi->term);
            } if (fi && fj/*i is bound and j is bound*/) {
                typestack_push(&Sx,fi); //push the term to which i is bound on Sx
                typestack_push(&Sy,fj); //push the term to which j is bound on Sy
            }
        }
    } // end main loop
    free(Sx.list);
    free(Sy.list);
    return true;
} // end function unify
