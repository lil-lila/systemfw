#include <stdlib.h>
#include <stdio.h>
#include "type.h"

struct type *mktype(enum ttype type,char *name,int arity/*,struct type **args*/) {
    struct type *t=(struct type *)malloc(sizeof(struct type));
    if (!t) return NULL;
    t->t=type;
    t->name=name;
    t->arity=arity;
    t->args[0]=NULL;
    t->args[1]=NULL;
    return t;
}

struct type *type_function(struct type *in,struct type *out) {
    struct type *t=mktype(TYPE_FUNCTION,NULL,2);
    t->args[0]=in;
    t->args[1]=out;
    return t;
}

struct type *type_var(char *name) {
    struct type *t=mktype(TYPE_VARIABLE,name,0);
    return t;
}

struct type *type_poly(char *name,struct type *e) {
    struct type *t=mktype(TYPE_POLY,name,1);
    t->args[0]=e;
    return t;
}

void printtype(struct type *t) {
    if (!t) {printf("<implicit>"); return;}
    switch (t->t) {
        case TYPE_VARIABLE:
            if (t->args[0]) printtype(t->args[0]);
            else {
                if (t->name) printf("%s", t->name);
                else printf("<unknown>");
            }
            break;
        case TYPE_NAME:
            printf("%s",t->name);
            break;
        case TYPE_FUNCTION:
            printf("(");
            printtype(t->args[0]);
            printf(" -> ");
            printtype(t->args[1]);
            printf(")");
            break;
        /*case TYPE_PAIR:
            printf("(");
            printtype(t->args[0]);
            printf(" * ");
            printtype(t->args[1]);
            printf(")");
            break;*/
        default: fprintf(stderr, "unknown type\n"); break;
    }
}

void destroytype(struct type *t) {
    if (!t) return;
    switch (t->t) {
        case TYPE_VARIABLE: 
        case TYPE_NAME:
            break;
        case TYPE_FUNCTION:
        //case TYPE_PAIR:
            destroytype(t->args[0]);
            destroytype(t->args[1]);
            break;
        default: break;
    }
    free(t);
}
/*int destroyterm(struct type *t) {
    free(t->name);
    for (int i=0;i<t->arity;i++) destroyterm(t->args[i]);
    free(t->args);
    free(t);
    return 0;
}

void printterm_(struct type *t) {
    printf("%s",t->name);
    if (t->arity!=0) {
        printf("(");
        printterm_(t->args[0]);
        for (int i=1;i<t->arity;i++) {printf(","); printterm_(t->args[i]);}
        printf(")");
    }
}

void printterm(struct type *t) {
    printterm_(t);
    printf("\n");
}*/
