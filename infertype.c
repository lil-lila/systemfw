#include <stdlib.h>
#include "context.h"
#include "unify.h"
#include "lambda.h"

struct type *infertype(struct lambda *l,struct context *D) {
    printf("b\n");
    if (!l) return NULL;
    switch(l->t) {
        case LAMBDA_ATOM: {
                printf("ATOM\n");
                struct contextrecord *r = context_find(D,l->atom.s);
                if (!r) return NULL;
                printf("enter\n");
                printnode(r->expr);
                if (!r->t) r->t=infertype(r->expr,D);
                printf("exit\n");
                return r->t;
                break;
            }
        case LAMBDA_ABSTR: {
                printf("ABSTR\n");
                // \x:s.T:t : s->t
                struct type *s;
                if (l->abstr.type) {
                    context_add(D,l->abstr.v,s=l->abstr.type,NULL);
                    printtype(s);
                }
                else context_add(D,l->abstr.v,s=type_var(l->abstr.v),NULL);
                struct type *t=infertype(l->abstr.expr,D);
                /*if (l->abstr.type)*/context_delete(D,l->abstr.v);
                if (!s || !t) return NULL;
                return type_function(s,t);
            break;
            }
        case LAMBDA_APPL: {
                struct type *lhst=infertype(l->appl.lhs,D);
                struct type *rhst=infertype(l->appl.rhs,D);
                if (!lhst || !rhst) return NULL;
                struct type *result=type_var(NULL);
                bool r=unify(type_function(rhst,result),lhst);
                return result;
            break;
            }
        default: return NULL;
    }
}
