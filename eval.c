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

struct lambda *eta(struct lambda *l) {
    if (!l) return NULL;
    if (l->t==LAMBDA_ABSTR && !l->abstr.overtype) {
        if (l->abstr.expr && l->abstr.expr->t==LAMBDA_APPL && !l->abstr.expr->appl.overtype) {
            if (l->abstr.expr->appl.rhs.l && l->abstr.expr->appl.rhs.l->t==LAMBDA_ATOM
                && l->abstr.expr->appl.rhs.l->atom.index==1) {
                struct lambda *r=l->abstr.expr->appl.lhs;
                destroynode(l->abstr.expr->appl.rhs.l);
                free(l->abstr.expr);
                destroytype(l->abstr.type);
                free(l->abstr.v);
                free(l);
                return r;
            }
        }
    }
    return l;
}

struct lambda *beta(struct lambda *l) {
    if (!l) return NULL;
    if (l->t==LAMBDA_APPL && !l->appl.overtype) {
        if (l->appl.lhs && l->appl.lhs->t==LAMBDA_ABSTR && !l->appl.lhs->abstr.overtype) {
            struct lambda *lhs_expr=l->appl.lhs->abstr.expr;
            struct lambda *rhs=l->appl.rhs.l;
            free(l->appl.lhs->abstr.v);
            destroytype(l->appl.lhs->abstr.type);
            free(l->appl.lhs);
            free(l);
            l = substitute(lhs_expr, rhs, 1);
            destroynode(rhs);
        }
    }
    return l;
}

struct lambda *eval(struct lambda *l,struct context *D) {
    if (!l) return NULL;
    switch(l->t) {
        case LAMBDA_ATOM:
            //printf("  atom\n");
            if (l->atom.index==0) {
                //printf("  i=0\n");
                struct contextrecord *r=context_findterm(D,l->atom.s);
                if (r) {
                    destroynode(l);
                    l=dupnode(r->expr);
                }
            } else printf("  N\n");
            break;
        case LAMBDA_ABSTR: {
            //printf("  abstr\n");
            struct lambda *old_l=l;
            l=eta(l);
            if (old_l==l) l->abstr.expr=eval(l->abstr.expr,D);
            else l=eval(l,D);
            break;
        }
        case LAMBDA_APPL: {
            struct lambda *old_l=l;
            if (l->appl.overtype) { //we have types checked in infertype;
                //if (!(l->appl.lhs && l->appl.lhs->t==LAMBDA_ABSTR && !l->appl.lhs->abstr.overtype)) return NULL;
                struct lambda *lhs=l->appl.lhs;
                destroytype(l->appl.rhs.t);
                printf("pre-i:%d && %d\n",lhs->t==LAMBDA_ABSTR, lhs->abstr.overtype);
                if (lhs->t==LAMBDA_ABSTR && lhs->abstr.overtype) {
                    printf("i\n");
                    struct lambda *lhs_expr=lhs->abstr.expr;
                    free(lhs->abstr.v);
                    destroytype(lhs->abstr.type);
                    free(lhs);
                    lhs=lhs_expr;
                }
                free(l);
                l=lhs;
            } else {
                printf("  appl\n");
                l=beta(l);
            }
            if (old_l==l) {
                l->appl.lhs=eval(l->appl.lhs,D);
                if (!l->appl.overtype) l->appl.rhs.l=eval(l->appl.rhs.l,D);
            } else {
                l=eval(l,D);
                printf("  !ot\n");
            }
            break;
        }
        default: return NULL;
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
