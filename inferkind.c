#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "context.h"
#include "unify.h"
#include "type.h"
/*
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
*/
struct kind *inferkind(struct type *t,struct context *D) {
    if (!t) return NULL;
    //printf("   TN:"); printnode(l); putchar('\n');
    switch(t->t) {
        case TYPE_NAME: {
                struct typerecord *r = context_findtype(D,t->name);
                if (!r) return mkkind(KIND_PROP,0);//NULL;
                if (!r->k) r->k=inferkind(r->t,D);
                return dupkind(r->k);
        }
        case TYPE_FUNCTION: { //Г |- T::*, S::* ===> (T->S)::*
            struct kind *k1=inferkind(t->args[0],D);
            struct kind *k2=inferkind(t->args[0],D);
            struct kind *r=NULL;
            if (k1 && k2 && k1->t==KIND_PROP && k2->t==KIND_PROP)
                r=k1;
            destroykind(k2);
            return r;
        }
        case TYPE_POLY: {// Г,x::s|- T::* ===> \/x.T ::*
            struct kind *s=t->kind;
            if (t->args[0]) context_addtype(D,t->name,NULL,s);
            else {
                context_deletetype(D,t->name);
                //destroykind(s);
                return NULL;
            }
            struct kind *T=inferkind(t->args[0],D);
            context_deletetype(D,t->name);
            struct kind *r=NULL;
            if (T && T->t==KIND_PROP)
                r=T;
            return r;
        }
        case TYPE_ABSTR: { // Г|- x::s. Г,x::s |- T::t ===> \x::s.T :: s->t
            struct kind *s=dupkind(t->kind);
            if (t->args[0]) context_addtype(D,t->name,NULL,s);
            else {
                context_deletetype(D,t->name);
                destroykind(s);
                return NULL;
            }
            struct kind *T=inferkind(t->args[0],D);
            context_deletetype(D,t->name);
            if (!s || !T) {
                destroykind(s);
                destroykind(T);
                return NULL;
            }
            s=kind_function(s,T);
            return s;
        }
        case TYPE_APPL: { // Г |- t:A->B,u:A ===> Г|-tu:B
            struct kind *lhst=inferkind(t->args[0],D);
            struct kind *rhst=inferkind(t->args[1],D);
            if (!lhst || !rhst) {
                destroykind(lhst);
                destroykind(rhst);
                return NULL;
            }
            if (lhst->t==KIND_FUNCTION && cmpkind(lhst->args[0],rhst)) {
                destroykind(rhst);
                destroykind(lhst->args[0]);
                struct kind *r=lhst->args[1];
                free(lhst);
                return r;
            } else {
                destroykind(lhst);
                destroykind(rhst);
                return NULL;
            }
            /*struct kind *result=kind_var(NULL);
            struct kind *tf=kind_function(rhst,result);
            bool r=unify(tf,lhst);
            struct kind *t=NULL;
            if (r && result) t=dupkind(result->args[0]);
            destroykind(lhst);
            destroykind(rhst);
            free(tf);
            free(result);
            return t;*/
        }
        default: return NULL;
    }
}
