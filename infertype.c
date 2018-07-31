#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "context.h"
#include "unify.h"
#include "lambda.h"

struct type *typeshift(struct type *where,int with,int atindex) {
    if (!where) return NULL;
    switch(where->t) {
        case TYPE_NAME:
        case TYPE_VARIABLE:
            if (where->index>atindex) {
                where->index+=with;
            }
            return where;
        case TYPE_APPL:
        case TYPE_FUNCTION:
            where->args[0]=typeshift(where->args[0],with,atindex);
            where->args[1]=typeshift(where->args[1],with,atindex);
            return where;
        case TYPE_ABSTR:
        case TYPE_POLY: {
            int ni=((where->t==TYPE_POLY)?0:1);
            where->args[0]=typeshift(where->args[0],with+ni,atindex+1);
            return where;
        }
        default: break;
    };
    return where;
}

struct type *subtype(struct type *where,struct type *with,int atindex) {
    if (!where || !with) return NULL;
    switch(where->t) {
        case TYPE_NAME:
            if (where->index==atindex) {
                destroytype(where);
                return typeshift(duptype(with),atindex-1,0);
            } else return where;
        case TYPE_FUNCTION:
        case TYPE_APPL:
            where->args[0]=subtype(where->args[0],with,atindex);
            where->args[1]=subtype(where->args[1],with,atindex);
            return where;
        case TYPE_POLY:
        case TYPE_ABSTR:
            where->args[0]=subtype(where->args[0],with,atindex+1);
            return where;
        default: break;
    }
    return where;
}

struct type *expandtype(struct type *lc,const struct context *const D) {
    if (!lc) return NULL;
    switch(lc->t) {
        case TYPE_NAME:
            if (lc->index==0) {
                struct typerecord *r=context_findtype(D,lc->name);
                if (r) {
                    destroytype(lc);
                    lc=expandtype(duptype(r->t),D);
                }
            }
            break;
        case TYPE_POLY:
        case TYPE_ABSTR:
            lc->args[0]=expandtype(lc->args[0],D);
            break;
        case TYPE_FUNCTION:
        case TYPE_APPL:
            lc->args[0]=expandtype(lc->args[0],D);
            lc->args[1]=expandtype(lc->args[1],D);
            break;
        default:
            break;
    }
    return lc;
}

struct type *typebeta(struct type *l) {
    if (!l) return NULL;
    if (l->t==TYPE_APPL) {
        if (l->args[0] && l->args[0]->t==TYPE_ABSTR) {
            struct type *lhs_expr=l->args[0]->args[0];
            struct type *rhs=l->args[1];
            free(l->args[0]->name);
            destroykind(l->args[0]->kind);
            free(l->args[0]);
            free(l);
            l = typeshift(subtype(lhs_expr, rhs, 1),-1,1);
            destroytype(rhs);
        }
    }
    return l;
}

struct type *evaltype(struct type *l,const struct context *const D) {
    if (!l) return NULL;
    switch(l->t) {
        case TYPE_NAME:
            l=expandtype(l,D);
            break;
        case TYPE_POLY:
        case TYPE_ABSTR: {
            l->args[0]=evaltype(l->args[0],D);
            break;
        }
        case TYPE_APPL: {
            struct type *old_l=l;
            l->args[0]=evaltype(l->args[0],D);
            l=typebeta(l);
            if (old_l==l) {
                l->args[0]=evaltype(l->args[0],D);
                l->args[1]=evaltype(l->args[1],D);
            } else {
                l=evaltype(l,D);
            }
            break;
        }
        case TYPE_FUNCTION:
            l->args[0]=evaltype(l->args[0],D);
            l->args[1]=evaltype(l->args[1],D);
            break;
        default: return NULL;
    }
    return l;
}

struct type *infertype(struct lambda *l,struct context *D) {
    if (!l) return NULL;
    switch(l->t) {
        case LAMBDA_ATOM: {
            struct contextrecord *r = context_findterm(D,l->atom.s);
            if (!r) return NULL;
            if (!r->t) r->t=expandtype(infertype(r->expr,D),D);
            struct type *s=evaltype(duptype(r->t),D);
            return s;
        }
        case LAMBDA_ABSTR: {
            if (l->abstr.overtype) { // /\a:*.T:B : \/a.B
                struct type *it=infertype(l->abstr.expr,D);
                if (!it) return NULL;
                struct type *t=expandtype(it,D);
                struct type *s=type_poly(strdup(l->abstr.v),t,mkkind(KIND_PROP,0));
                if (!s || !t) {
                   destroytype(s);
                   destroytype(t);
                   return NULL;
                }
                return s;
            } else { // \x:s.T:t : s->t
                struct type *s=expandtype(duptype(l->abstr.type),D);
                if (l->abstr.type) context_addterm(D,l->abstr.v,s,NULL);
                else {
                    context_deleteterm(D,l->abstr.v);
                    destroytype(s);
                    return NULL;
                }
                struct type *t=expandtype(infertype(l->abstr.expr,D),D);
                context_deleteterm(D,l->abstr.v);
                if (!s || !t) {
                    destroytype(s);
                    destroytype(t);
                    return NULL;
                }
                s=type_function(s,t);
                return s;
            }
        }
        case LAMBDA_APPL: {
            if (l->appl.overtype) { // Г |-t:\/a.B ===> Г|-t[A]:B{a:=A}
                struct type *t1=expandtype(infertype(l->appl.lhs,D),D);
                struct type *t2=expandtype(duptype(l->appl.rhs.t),D);
                if (t1 && t1->t==TYPE_POLY) {
                    struct type *s=subtype(t1->args[0],t2,1);
                    destroytype(t2);
                    free(t1->name);
                    destroykind(t1->kind);
                    free(t1);
                    return s;
                } else {
                    destroytype(t1);
                    destroytype(t2);
                    return NULL;
                }
            } else { // Г |- t:A->B,u:A ===> Г|-tu:B
                struct type *lhst=expandtype(infertype(l->appl.lhs,D),D);
                struct type *rhst=expandtype(infertype(l->appl.rhs.l,D),D);
                if (!lhst || !rhst) {
                    destroytype(lhst);
                    destroytype(rhst);
                    return NULL;
                }
                struct type *result=type_var(NULL);
                struct type *tf=type_function(rhst,result);
                bool r=unify(tf,lhst);
                struct type *t=NULL;
                if (r && result) t=duptype(result->args[0]);
                destroytype(lhst);
                destroytype(rhst);
                free(tf);
                free(result);
                return t;
            }
        }
        default: return NULL;
    }
}
