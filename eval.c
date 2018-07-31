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
        case LAMBDA_ABSTR: {
            int ni=(where->abstr.overtype?0:1);
            where->abstr.expr=shift(where->abstr.expr,with+ni,atindex+ni);
            return where;
        }
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
                if (r && r->expr) {
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

struct lambda *eval(struct lambda *l,const struct context *const D) {
    if (!l) return NULL;
    switch(l->t) {
        case LAMBDA_ATOM:
            l=expand(l,D);
            if (l->t!=LAMBDA_ATOM) l=eval(l,D);
            break;
        case LAMBDA_ABSTR: {
            l->abstr.expr=eval(l->abstr.expr,D);
            break;
        }
        case LAMBDA_APPL: {
            struct lambda *old_l=l;
            l->appl.lhs=eval(l->appl.lhs,D);
            if (l->appl.overtype) { //we have types checked in infertype;
                struct lambda *lhs=l->appl.lhs;
                if (lhs->t==LAMBDA_ABSTR && lhs->abstr.overtype) {
                    struct lambda *lhs_expr=applytype(lhs->abstr.expr,l->appl.rhs.t,D,1);
                    destroytype(l->appl.rhs.t);
                    free(lhs->abstr.v);
                    destroykind(lhs->abstr.kind);
                    free(lhs);
                    free(l);
                    l=lhs_expr;
                }
            } else {
                l=beta(l);
                if (old_l!=l) l=eval(l,D);
                else l->appl.rhs.l=eval(l->appl.rhs.l,D);
            }
            if (old_l==l) { //keine beta-reduction
                l->appl.lhs=eval(l->appl.lhs,D);
                if (!l->appl.overtype) l->appl.rhs.l=eval(l->appl.rhs.l,D);
            } else {
                l=eval(l,D);
            }
            break;
        }
        default: return NULL;
    }
    return l;
}
