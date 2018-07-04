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

struct type *infertype(struct lambda *l,struct context *D) {
    if (!l) return NULL;
    //printf("TN:"); printnode(l); putchar('\n');
    switch(l->t) {
        case LAMBDA_ATOM: {
                struct contextrecord *r = context_findterm(D,l->atom.s);
                if (!r) return NULL;
                //printnode(r->expr);
                if (!r->t) r->t=infertype(r->expr,D);
                /*printf("%s:",l->atom.s);
                printtype(r->t);
                putchar('\n');*/
                return duptype(r->t);
            }
        case LAMBDA_ABSTR: {
                if (l->abstr.overtype) { // /\a:*.T:B : \/a.B
                    struct type *t=infertype(l->abstr.expr,D);
                    struct type *s=type_poly(strdup(l->abstr.v),t);
                    return s;
                } else { // \x:s.T:t : s->t
                    struct type *s=l->abstr.type;
                    if (l->abstr.type) context_addterm(D,l->abstr.v,s,NULL);
                    else return NULL;
                    struct type *t=infertype(l->abstr.expr,D);
                    context_deleteterm(D,l->abstr.v);
                    s=type_function(s,t);
                    if (!s || !t) return NULL;
                    return s;
                }
            }
        case LAMBDA_APPL: {
                if (l->appl.overtype) { // Г |-t:\/a.B ===> Г|-t[A]:B{a:=A}
                    struct type *t1=infertype(l->appl.lhs,D);
                    struct type *t2=l->appl.rhs.t;
                    if (t2 && t2->t==TYPE_NAME && !t2->index) {
                        struct typerecord *t2_r=context_findtype(D,t2->name);
                        if (t2_r) t2=duptype(t2_r->t);
                        else t2=duptype(t2);

                    }
                    if (t1 && t1->t==TYPE_POLY) {
                        struct type *s=subtype(t1->args[0],t2,1);
                        free(t1);
                        return s;
                    } else return NULL;
                } else { // Г |- t:A->B,u:A ===> Г|-tu:B
                    struct type *lhst=infertype(l->appl.lhs,D);
                    struct type *rhst=infertype(l->appl.rhs.l,D);
                    if (!lhst || !rhst) return NULL;
                    struct type *result=type_var(NULL);
                    struct type *tf=type_function(rhst,result);
                    bool r=unify(tf,lhst);
                    destroytype(lhst);
                    destroytype(rhst);
                    free(tf);
                    if (!r) return NULL;
                    else {
                        struct type *t=duptype(result->args[0]);
                        free(result);
                        return t;
                    }
                }
            }
        default: return NULL;
    }
}
