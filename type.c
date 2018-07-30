#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "type.h"

struct type *mktype(enum ttype type,char *name,int arity/*,struct type **args*/) {
    struct type *t=(struct type *)malloc(sizeof(struct type));
    if (!t) return NULL;
    t->t=type;
    t->name=name;
    t->arity=arity;
    t->kind=NULL;
    t->args[0]=NULL;
    t->args[1]=NULL;
    return t;
}

struct type *type_function(struct type *in,struct type *out) {
    struct type *t=mktype(TYPE_FUNCTION,NULL,2);
    if (!t) return NULL;
    t->args[0]=in;
    t->args[1]=out;
    return t;
}

struct type *type_appl(struct type *in,struct type *out) {
    struct type *t=mktype(TYPE_APPL,NULL,2);
    if (!t) return NULL;
    t->args[0]=in;
    t->args[1]=out;
    return t;
}

struct type *type_var(char *name) {
    struct type *t=mktype(TYPE_VARIABLE,name,0);
    return t;
}

struct type *type_poly(char *name,struct type *e,struct kind *k) {
    struct type *t=mktype(TYPE_POLY,name,1);
    if (!t) return NULL;
    t->args[0]=e;
    t->kind=k;
    return t;
}

struct type *type_abstr(char *name,struct type *e,struct kind *k) {
    struct type *t=mktype(TYPE_ABSTR,name,1);
    if (!t) return NULL;
    t->args[0]=e;
    t->kind=k;
    return t;
}

void printtype(struct type *t) {
    if (!t) {printf("<>"); return;}
    switch (t->t) {
        case TYPE_VARIABLE:
            if (t->args[0]) {printf("i:"); printtype(t->args[0]);}
            else {
                if (t->name) printf("UV:%s", t->name);
                else printf("<unknown>");
            }
            break;
        case TYPE_NAME:
            printf("%s_%d",t->name,t->index);
            break;
        case TYPE_POLY:
            printf("\\/%s:",t->name);
            printkind(t->kind);
            putchar('.');
            printtype(t->args[0]);
            break;
        case TYPE_FUNCTION:
            printf("(");
            printtype(t->args[0]);
            printf(" -> ");
            printtype(t->args[1]);
            printf(")");
            break;
        case TYPE_ABSTR:
            printf("\\%s:",t->name);
            printkind(t->kind);
            putchar('.');
            if (t->args[0] && t->args[0]->t==TYPE_APPL) putchar('(');
            printtype(t->args[0]);
            if (t->args[0] && t->args[0]->t==TYPE_APPL) putchar(')');
            break;
        case TYPE_APPL:
            if (t->args[0] && t->args[0]->t==TYPE_ABSTR) putchar('(');
            printtype(t->args[0]);
            if (t->args[0] && t->args[0]->t==TYPE_ABSTR) putchar(')');
            putchar(' ');
            if (t->args[1] && t->args[1]->t!=TYPE_NAME) putchar('(');
            printtype(t->args[1]);
            if (t->args[1] && t->args[1]->t!=TYPE_NAME) putchar(')');
            break;
        default: fprintf(stderr, "unknown type\n"); break;
    }
}

struct type *duptype(struct type *t) {
    if (!t) return NULL;
    struct type *ct=mktype(t->t,NULL,t->arity);
    if (!ct) return NULL;
    switch (ct->t) {
        case TYPE_VARIABLE:
        case TYPE_NAME:
            ct->name=strdup(t->name);
            ct->index=t->index;
            break;
        case TYPE_POLY:
        case TYPE_ABSTR:
            ct->name=strdup(t->name);
            ct->kind=dupkind(t->kind);
            ct->args[0]=duptype(t->args[0]);
            break;
        case TYPE_FUNCTION:
        case TYPE_APPL:
            ct->args[0]=duptype(t->args[0]);
            ct->args[1]=duptype(t->args[1]);
            break;
        default: break;
    }
    return ct;
}

bool cmptype(struct type *t1,struct type *t2) {
    if ((t1 && !t2) || (!t1 && t2)) return false;
    if (t1 == t2) return true;
    if (t1->t==t2->t) {
        switch(t1->t) {
            case TYPE_NAME:
                if (t1->index==t2->index) {
                    if (t1->index==0) {
                        if (t1->name==NULL && t2->name==NULL) return true;
                        else if ((t1->name && !t2->name) || (!t1->name && t2->name)) return false;
                        else return strcmp(t1->name,t2->name)==0;
                    } else return true;
                }
            case TYPE_FUNCTION:
                return cmptype(t1->args[0],t2->args[0]) && cmptype(t1->args[1],t2->args[1]);
            case TYPE_POLY:
            case TYPE_VARIABLE:
                return cmptype(t1->args[0],t2->args[0]);
            default: return false;
        }
    } else return false;
}

void destroytype(struct type *t) {
    if (!t) return;
    switch (t->t) {
        case TYPE_VARIABLE: 
        case TYPE_NAME:
            free(t->name);
            break;
        case TYPE_FUNCTION:
        case TYPE_APPL:
            destroytype(t->args[0]);
            destroytype(t->args[1]);
            break;
        case TYPE_POLY:
        case TYPE_ABSTR:
            free(t->name);
            destroytype(t->args[0]);
            destroykind(t->kind);
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
