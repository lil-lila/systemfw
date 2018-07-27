#include <stdlib.h>
#include <stdio.h>
#include "kind.h"

struct kind *mkkind(enum tkind kind,int arity/*,struct kind **args*/) {
    struct kind *t=(struct kind *)malloc(sizeof(struct kind));
    if (!t) return NULL;
    t->t=kind;
    t->arity=arity;
    t->args[0]=NULL;
    t->args[1]=NULL;
    return t;
}

struct kind *kind_function(struct kind *in,struct kind *out) {
    struct kind *t=mkkind(KIND_FUNCTION,2);
    t->args[0]=in;
    t->args[1]=out;
    return t;
}

struct kind *kind_var() {
    struct kind *t=mkkind(KIND_VARIABLE,0);
    return t;
}

void printkind(struct kind *t) {
    if (!t) {printf("<implicit>"); return;}
    switch (t->t) {
        case KIND_VARIABLE:
            printf("_");
            break;
        case KIND_PROP:
            printf("*");
            break;
        case KIND_FUNCTION:
            printf("(");
            printkind(t->args[0]);
            printf(" -> ");
            printkind(t->args[1]);
            printf(")");
            break;
        default: fprintf(stderr, "unknown kind\n"); break;
    }
}

void destroykind(struct kind *t) {
    if (!t) return;
    switch (t->t) {
        case KIND_VARIABLE:
        case KIND_PROP:
            break;
        case KIND_FUNCTION:
            destroykind(t->args[0]);
            destroykind(t->args[1]);
            break;
        default: break;
    }
    free(t);
}

bool cmpkind(struct kind *t1,struct kind *t2) {
    if ((t1 && !t2) || (!t1 && t2)) return false;
    if (t1 == t2) return true;
    if (t1->t==t2->t) {
        switch(t1->t) {
            case KIND_PROP: return true;
            case KIND_FUNCTION:
                return cmpkind(t1->args[0],t2->args[0]) && cmpkind(t1->args[1],t2->args[1]);
            default: return false;
        }
    } else return false;
}

struct kind *dupkind(struct kind *t) {
    if (!t) return NULL;
    struct kind *nt=mkkind(t->t,t->arity);
    if (!nt) return NULL;
    switch (t->t) {
        case KIND_VARIABLE:
        case KIND_PROP:
            break;
        case KIND_FUNCTION:
            nt->args[0]=dupkind(t->args[0]);
            nt->args[1]=dupkind(t->args[1]);
            break;
        default: break;
    }
    return nt;
}
/*int destroyterm(struct kind *t) {
    free(t->name);
    for (int i=0;i<t->arity;i++) destroyterm(t->args[i]);
    free(t->args);
    free(t);
    return 0;
}

void printterm_(struct kind *t) {
    printf("%s",t->name);
    if (t->arity!=0) {
        printf("(");
        printterm_(t->args[0]);
        for (int i=1;i<t->arity;i++) {printf(","); printterm_(t->args[i]);}
        printf(")");
    }
}

void printterm(struct kind *t) {
    printterm_(t);
    printf("\n");
}*/
