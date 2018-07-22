#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "lambda.h"
#include "context.h"
#include "infertype.h"

struct lambda *shift(struct lambda *where,int with,int atindex) {
    if (!where) return NULL;
    switch(where->t) {
        case LAMBDA_ATOM:
            if (where->atom.index>atindex) {
                where->atom.index+=with;
            }
            return where;
        case LAMBDA_APPL:
            where->appl.lhs=shift(where->appl.lhs,with,atindex);
            if (!where->appl.overtype)
                where->appl.rhs.l=shift(where->appl.rhs.l,with,atindex);
            // else do nothing;
            return where;
            break;
        case LAMBDA_ABSTR:
            where->abstr.expr=shift(where->abstr.expr,with+1,atindex+(where->abstr.overtype?0:1));
            return where;
            break;
        default:
            break;
    };
    return where;
}

struct lambda *substitute(struct lambda *where,struct lambda *with,int atindex) {
    if (!where || !with) return NULL;
    switch(where->t) {
        case LAMBDA_ATOM:
            if (where->atom.index==atindex) {
                destroynode(where);
                return shift(dupnode(with),atindex-1,0);
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
            where->abstr.expr=substitute(where->abstr.expr,with,atindex+(where->abstr.overtype?0:1));
            return where;
            break;
        default:
            break;
    };
    return where;
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
            l = shift(substitute(lhs_expr, rhs, 1),-1,1);
            destroynode(rhs);
        }
    }
    return l;
}

struct lambda *expand(struct lambda *lc,const struct context *const D) {
    if (!lc) return NULL;
    switch(lc->t) {
        case LAMBDA_ATOM:
            if (lc->atom.index==0) {
                struct contextrecord *r=context_findterm(D,lc->atom.s);
                if (r) {
                    destroynode(lc);
                    lc=expand(dupnode(r->expr),D);
                }
            }
            break;
        case LAMBDA_ABSTR:
            lc->abstr.expr=expand(lc->abstr.expr,D);
            break;
        case LAMBDA_APPL:
            lc->appl.lhs=expand(lc->appl.lhs,D);
            if (!lc->appl.overtype)
                lc->appl.rhs.l=expand(lc->appl.rhs.l,D);
            break;
        default:
            break;
    }
    return lc;
}

struct lambda *applytype(struct lambda *l,struct type *t,const struct context *const D,int atindex) {
    if (!l) return NULL;
    switch(l->t) {
        case LAMBDA_ATOM:
            break;
        case LAMBDA_ABSTR: {
            int ni=atindex;
            if (!l->abstr.overtype) l->abstr.type=subtype(l->abstr.type,t,atindex);
            else ni++;
            l->abstr.expr=applytype(l->abstr.expr,t,D,ni);
            break;
        }
        case LAMBDA_APPL:
            l->appl.lhs=applytype(l->appl.lhs,t,D,atindex);
            if (!l->appl.overtype) l->appl.rhs.l=applytype(l->appl.rhs.l,t,D,atindex);
            else l->appl.rhs.t=subtype(l->appl.rhs.t,t,atindex);
            break;
        default:
            break;
    }
    return l;

}

bool reducible(struct lambda *l) {
    if (!l) return false;
    if (l->t==LAMBDA_APPL && !l->appl.overtype) {
        if (l->appl.lhs && l->appl.lhs->t==LAMBDA_ABSTR && !l->appl.lhs->abstr.overtype) {
            return true;
        }
    }
    switch (l->t) {
        case LAMBDA_ATOM:
            return !l->atom.index;
        case LAMBDA_ABSTR:
            return reducible(l->abstr.expr);
        case LAMBDA_APPL:
            return reducible(l->appl.lhs) && l->appl.overtype?false:reducible(l->appl.rhs.l);
        default: return false;
    }
}

struct lambda *eval_(struct lambda *l,const struct context *const D) {
    if (!l) return NULL;
    switch(l->t) {
        case LAMBDA_ATOM:
            l=expand(l,D);
            break;
        case LAMBDA_ABSTR: {
            l->abstr.expr=eval_(l->abstr.expr,D);
            break;
        }
        case LAMBDA_APPL: {
            //printf("appl:"); printnode(l); putchar('\n');
            struct lambda *old_l=l;
            l->appl.lhs=expand(l->appl.lhs,D);
            if (l->appl.overtype) { //we have types checked in infertype;
                //if (!(l->appl.lhs && l->appl.lhs->t==LAMBDA_ABSTR && !l->appl.lhs->abstr.overtype)) return NULL;
                struct lambda *lhs=l->appl.lhs;
                if (lhs->t==LAMBDA_ABSTR && lhs->abstr.overtype) {
                    struct lambda *lhs_expr=applytype(lhs->abstr.expr,l->appl.rhs.t,D,1);
                    destroytype(l->appl.rhs.t);
                    free(lhs->abstr.v);
                    destroytype(lhs->abstr.type);
                    free(lhs);
                    lhs=lhs_expr;
                    free(l);
                    l=lhs;
                }// else destroytype(l->appl.rhs.t);
            } else {
                l=beta(l);
                if (old_l!=l) l=eval_(l,D);
                //if (l && l->t==LAMBDA_ABSTR) l->abstr.expr=eval_(l->abstr.expr,D);
            }
            if (old_l==l) { //keine beta-reduction
                l->appl.lhs=eval_(l->appl.lhs,D);
                if (!l->appl.overtype) l->appl.rhs.l=eval_(l->appl.rhs.l,D);
            } else {
                l=eval_(l,D);
            }
            break;
        }
        default: return NULL;
    }
    return l;
}

struct lambda *eval(struct lambda *l,const struct context *const D) {
    //printf("begin:%d\n",reducible(l));
    while(reducible(l)) l=eval_(l,D);
    //printf("end:%d\n",reducible(l));
    return l;
}
