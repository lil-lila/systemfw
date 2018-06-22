#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "lambda.h"

struct lambda *dupnode(struct lambda *l) {
    struct lambda *lc=(struct lambda *)malloc(sizeof(struct lambda));
    if (!lc) return NULL;
    switch(lc->t=l->t) {
        case LAMBDA_ATOM:
            lc->atom.s=strdup(l->atom.s);
            lc->atom.index=l->atom.index;
            break;
        case LAMBDA_ABSTR:
            lc->abstr.v=strdup(l->abstr.v);
            lc->abstr.expr=dupnode(l->abstr.expr);
            break;
        case LAMBDA_APPL:
            lc->appl.lhs=dupnode(l->appl.lhs);
            lc->appl.rhs=dupnode(l->appl.rhs);
        default:
            break;
    }
    return lc;
}

void destroynode(struct lambda *lc) {
    if (!lc) return;
    switch(lc->t) {
        case LAMBDA_ATOM:
            free(lc->atom.s);
            break;
        case LAMBDA_ABSTR:
            free(lc->abstr.v);
            destroynode(lc->abstr.expr);
            break;
        case LAMBDA_APPL:
            destroynode(lc->appl.lhs);
            destroynode(lc->appl.rhs);
            break;
        default:
            break;
    }
    free(lc);
}

void printnode(struct lambda *lc) {
    if (!lc) return;
    switch(lc->t) {
        case LAMBDA_ATOM:
            printf("%s_%d",lc->atom.s,lc->atom.index);
            break;
        case LAMBDA_ABSTR:
            printf("\\%s.",lc->abstr.v);
            printnode(lc->abstr.expr);
            //putchar(')');
            break;
        case LAMBDA_APPL:
            if (lc->appl.lhs->t==LAMBDA_ABSTR) putchar('(');
            printnode(lc->appl.lhs);
            if (lc->appl.lhs->t==LAMBDA_ABSTR) putchar(')');
            putchar(' ');
            if (lc->appl.rhs->t!=LAMBDA_ATOM) putchar('(');
            printnode(lc->appl.rhs);
            if (lc->appl.rhs->t!=LAMBDA_ATOM) putchar(')');
            break;
        default:
            break;
    }
}
