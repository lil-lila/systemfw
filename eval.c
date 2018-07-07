#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "lambda.h"
#include "context.h"

struct lambda *substitute(struct lambda *where,struct lambda *with,int atindex) {
    if (!where || !with) return NULL;
    switch(where->t) {
        case LAMBDA_ATOM:
            if (where->atom.index==atindex) {
                destroynode(where);
                return dupnode(with);
            } else return where;
            break;
        case LAMBDA_APPL:
            where->appl.lhs=substitute(where->appl.lhs,with,atindex);
            if (!where->appl.overtype)
                where->appl.rhs.l=substitute(where->appl.rhs.l,with,atindex);
            // else do nothing;
            return where;
            break;
        case LAMBDA_ABSTR:
            where->abstr.expr=substitute(where->abstr.expr,with,atindex+1);
            return where;
            break;
        default:
            break;
    };
    return where;
}

bool reducible(struct lambda *ast) {
    if (!ast) return false;
    if (ast->t==LAMBDA_ATOM) return ast->atom.index==0;
    else if (ast->t==LAMBDA_ABSTR && !reducible(ast->abstr.expr)) return false;
    else if (ast->t==LAMBDA_APPL) {
        if (ast->appl.lhs->t==LAMBDA_ABSTR) {
            printf("B: %d\n",(ast->appl.overtype == ast->appl.lhs->abstr.overtype));
            if (ast->appl.overtype == ast->appl.lhs->abstr.overtype) return true;
            else return false;
        }
        else reducible(ast->appl.lhs) || (!ast->appl.overtype && reducible(ast->appl.rhs.l));
    }
    return false;//return reducible(ast->abstr.expr);
}

struct lambda *eval(struct lambda *l,struct context *D) {
    if (!l) return NULL;
    switch(l->t) {
        case LAMBDA_ATOM:
            printf("  atom\n");
            if (l->atom.index==0) {
                printf("  i=0\n");
                struct contextrecord *r=context_findterm(D,l->atom.s);
                if (r) {
                    destroynode(l);
                    l=dupnode(r->expr);
                }
            } else printf("  N\n");
            break;
        case LAMBDA_ABSTR:
            printf("  abstr\n");
            l->abstr.expr=eval(l->abstr.expr,D);
            break;
        case LAMBDA_APPL:
            printf("  appl\n");
            l->appl.lhs=eval(l->appl.lhs,D);
            if (!l->appl.overtype) l->appl.rhs.l=eval(l->appl.rhs.l,D);
            else printf("  !ot\n");
    }
    return l;
}
#if 0
struct lambda *eval(struct lambda *l,struct context *D) {
    if (!l) return NULL;
    //printf("\nr:%d\n",reducible(l));
    while (reducible(l)) {
        if (l->t==LAMBDA_APPL) {
            if (l->appl.lhs && l->appl.lhs->t==LAMBDA_ABSTR) { // b-reduction
                // assume that appl.overtype==appl.lhs->abstr.overtype
                if (!l->appl.lhs->abstr.overtype) {
                    struct lambda *lhs_expr=l->appl.lhs->abstr.expr;
                    struct lambda *rhs=l->appl.rhs.l;
                    free(l->appl.lhs->abstr.v);
                    free(l->appl.lhs);
                    free(l);
                    l = substitute(lhs_expr, rhs, 1);
                    destroynode(rhs);
                } else {
                    printf("A\n");
                    struct lambda *lhs_expr=l->appl.lhs->abstr.expr;
                    free(l->appl.lhs->abstr.v);
                    free(l->appl.lhs);
                    destroytype(l->appl.rhs.t);
                    free(l);
                    l=lhs_expr;
                }
            } else {
                l->appl.lhs = eval(l->appl.lhs,D);
                if (!l->appl.overtype) l->appl.rhs.l = eval(l->appl.rhs.l,D);
            }
        } else if (l->t==LAMBDA_ATOM && l->atom.index==0 /* if it is free variable */) {
            struct contextrecord *r=context_findterm(D,l->atom.s);
            if (r) {
                destroynode(l);
                l=dupnode(r->expr);
            }
        }
    }
    return l;
}
#endif
