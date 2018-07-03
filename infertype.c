#include <stdlib.h>
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
            break;
        case TYPE_FUNCTION:
            where->args[0]=subtype(where->args[0],with,atindex);
            where->args[1]=subtype(where->args[0],with,atindex);
            return where;
            break;
        case TYPE_POLY:
            where->args[0]=subtype(where->args[0],with,atindex+1);
            return where;
            break;
        default:
            break;
    };
    return where;
}

struct type *infertype(struct lambda *l,struct context *D) {
    if (!l) return NULL;
    switch(l->t) {
        case LAMBDA_ATOM: {
                struct contextrecord *r = context_findterm(D,l->atom.s);
                if (!r) return NULL;
                printnode(r->expr);
                if (!r->t) r->t=infertype(r->expr,D);
                return r->t;
            }
        case LAMBDA_ABSTR: {
                struct type *s;
                if (l->abstr.overtype) { // \x:s.T:t : s->t
                    struct type *t=infertype(l->abstr.expr,D);
                    
                } else {
                    if (l->abstr.type) context_addterm(D,l->abstr.v,s=l->abstr.type,NULL);
                    else context_addterm(D,l->abstr.v,s=type_var(l->abstr.v),NULL); // just in case
                    struct type *t=infertype(l->abstr.expr,D);
                    /*if (l->abstr.type)*/context_deleteterm(D,l->abstr.v);
                    if (!s || !t) return NULL;
                    return type_function(s,t);
                }
            }
        case LAMBDA_APPL: {
                if (l->appl.overtype) {

                } else {
                    struct type *lhst=infertype(l->appl.lhs,D);
                    struct type *rhst=infertype(l->appl.rhs.l,D);
                    if (!lhst || !rhst) return NULL;
                    struct type *result=type_var(NULL);
                    bool r=unify(type_function(rhst,result),lhst);
                    return result;
                }
            }
        default: return NULL;
    }
}
