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
            if (l->appl.overtype) lc->appl.rhs.t=duptype(l->appl.rhs.t);
            else lc->appl.rhs.l=dupnode(l->appl.rhs.l);
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
            destroytype(lc->abstr.type);
            destroynode(lc->abstr.expr);
            break;
        case LAMBDA_APPL:
            destroynode(lc->appl.lhs);
            if (lc->appl.overtype) destroytype(lc->appl.rhs.t);
            else destroynode(lc->appl.rhs.l);
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
            printf("%s%s:",lc->abstr.overtype?"/\\":"\\",lc->abstr.v);
            printtype(lc->abstr.type);
            putchar('.');
            printnode(lc->abstr.expr);
            break;
        case LAMBDA_APPL:
            if (lc->appl.lhs->t==LAMBDA_ABSTR) putchar('(');
            printnode(lc->appl.lhs);
            if (lc->appl.lhs->t==LAMBDA_ABSTR) putchar(')');
            putchar(' ');
            if (lc->appl.overtype) {
                putchar('[');
                printtype(lc->appl.rhs.t);
                putchar(']');
            } else {
                if (lc->appl.rhs.l->t!=LAMBDA_ATOM) putchar('(');
                printnode(lc->appl.rhs.l);
                if (lc->appl.rhs.l->t!=LAMBDA_ATOM) putchar(')');
            }
            break;
        default:
            break;
    }
}
