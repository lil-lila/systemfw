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
            lc->abstr.overtype=l->abstr.overtype;
            lc->abstr.v=strdup(l->abstr.v);
            lc->abstr.expr=dupnode(l->abstr.expr);
            lc->abstr.type=duptype(l->abstr.type);
            break;
        case LAMBDA_APPL:
            lc->appl.overtype=l->appl.overtype;
            lc->appl.lhs=dupnode(l->appl.lhs);
            if (l->appl.overtype) lc->appl.rhs.t=duptype(l->appl.rhs.t);
            else lc->appl.rhs.l=dupnode(l->appl.rhs.l);
        default:
            break;
    }
    return lc;
}

#if 0
bool cmpnode(struct lambda *t1,struct lambda *t2) {
    if ((t1 && !t2) || (!t1 && t2)) return false;
    if (t1 == t2) return true;
    if (t1->t==t2->t) {
        switch(t1->t) {
            case LAMBDA_ATOM:
                if (t1->index==t2->index) {
                    if (t1->index==0) {
                        if (t1->name==NULL && t2->name==NULL) return true;
                        else if ((t1->name && !t2->name) || (!t1->name && t2->name)) return false;
                        else return strcmp(t1->name,t2->name)==0;
                    } else return true;
                }
            case LAMBDA_ABSTR:
                //if (t1->abstr.overtype!=t2->abstr.overtype) return false;
                //if (t1->abstr.overtype) 
                //return cmptype(t1->args[0],t2->args[0]) && cmptype(t1->args[1],t2->args[1]);
            case LAMBDA_APPL:
                if (t1->appl.overtype!=t2->appl.overtype) return false;
                if (t1->appl.overtype) return cmpnode(t1->appl.lhs,t2->appl.lhs) && cmptype(t1->appl.rhs.t,t2->appl.rhs.t);
                else return cmpnode(t1->appl.lhs,t2->appl.lhs) && cmpnode(t1->appl.rhs.l,t2->appl.rhs.l);
            default: return false;
        }
    } else return false;
}
#endif

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
            if (lc->abstr.expr->t==LAMBDA_APPL) putchar('(');
            printnode(lc->abstr.expr);
            if (lc->abstr.expr->t==LAMBDA_APPL) putchar(')');
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
