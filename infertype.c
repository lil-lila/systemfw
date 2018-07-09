#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "context.h"
#include "unify.h"
#include "lambda.h"

struct type *subtype(struct type *where,struct type *with,int atindex) {
    if (!where || !with) return NULL;
    switch(where->t) {
        case TYPE_NAME:
            if (where->index==atindex) {
                destroytype(where);
                return duptype(with);
            } else return where;
        case TYPE_FUNCTION:
            where->args[0]=subtype(where->args[0],with,atindex);
            where->args[1]=subtype(where->args[1],with,atindex);
            return where;
        case TYPE_POLY:
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
            lc->args[0]=expandtype(lc->args[0],D);
            break;
        case TYPE_FUNCTION:
            lc->args[0]=expandtype(lc->args[0],D);
            lc->args[1]=expandtype(lc->args[1],D);
            break;
        default:
            break;
    }
    return lc;
}

struct type *infertype(struct lambda *l,struct context *D) {
    if (!l) return NULL;
    //printf("   TN:"); printnode(l); putchar('\n');
    switch(l->t) {
        case LAMBDA_ATOM: {
                struct contextrecord *r = context_findterm(D,l->atom.s);
                if (!r) return NULL;
                if (!r->t) r->t=expandtype(infertype(r->expr,D),D);
                return duptype(r->t);
            }
        case LAMBDA_ABSTR: {
                if (l->abstr.overtype) { // /\a:*.T:B : \/a.B
                    struct type *t=expandtype(infertype(l->abstr.expr,D),D);
                    struct type *s=type_poly(strdup(l->abstr.v),t);
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
                    if (!s || !t) return NULL;
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
                    //printf("result: %d\n",r);
                    return t;
                }
            }
        default: return NULL;
    }
}
