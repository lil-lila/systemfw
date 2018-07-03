#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "context.h"
#include "lambda.h"
#include "type.h"

struct context context_init() {
    struct context c;
    c.size=0;
    c.termlist=NULL;
    c.typelist=NULL;
    return c;
}

int context_addterm(struct context *c,char *n,struct type *t,struct lambda *l) {
    if (!c->size || !c->termlist) c->termlist=(struct contextrecord *)malloc((c->size=1)*sizeof(struct contextrecord));
    else {
        if (context_findterm(c,n)) return 0;
        c->termlist=(struct contextrecord *)realloc(c->termlist,++c->size*sizeof(struct contextrecord));
    }
    if (!c->size) return 0;
    c->termlist[c->size-1].expr=l;
    c->termlist[c->size-1].name=n;
    c->termlist[c->size-1].t=t;
    return 1;
}

struct contextrecord *context_findterm(struct context *c,char *name) {
    if (!c->size) return NULL;
    for(int i=c->size-1;i>=0;i--) if (!strcmp(c->termlist[i].name,name)) return &c->termlist[i];
    return NULL;

}

void context_deleteterm(struct context *c,char *n) {
    if (c && c->termlist)
        for (int i=c->size-1;i>=0;i--)
            if (!strcmp(c->termlist[i].name,n)) {
                c->termlist[i]=c->termlist[c->size-1];
                c->termlist=(struct contextrecord *)realloc(c->termlist,--c->size*(sizeof(struct contextrecord)));
                return;
            }
    return;
}

void context_destroy(struct context c) {
    if (c.termlist) free(c.termlist);
    if (c.typelist) free(c.typelist);
}

int context_addtype(struct context *c,char *n,struct type *t) {
    if (!c->size || !c->typelist) c->typelist=(struct typerecord *)malloc((c->size=1)*sizeof(struct typerecord));
    else {
        if (context_findtype(c,n)) return 0;
        c->typelist=(struct typerecord *)realloc(c->typelist,++c->size*sizeof(struct typerecord));
    }
    if (!c->size) return 0;
    c->typelist[c->size-1].name=n;
    c->typelist[c->size-1].t=t;
    return 1;
}

struct typerecord *context_findtype(struct context *c,char *name) {
    if (!c->size) return NULL;
    for(int i=c->size-1;i>=0;i--) if (!strcmp(c->typelist[i].name,name)) return &c->typelist[i];
    return NULL;

}

void context_deletetype(struct context *c,char *n) {
    if (c && c->typelist)
        for (int i=c->size-1;i>=0;i--)
            if (!strcmp(c->typelist[i].name,n)) {
                c->typelist[i]=c->typelist[c->size-1];
                c->typelist=(struct typerecord *)realloc(c->typelist,--c->size*(sizeof(struct typerecord)));
                return;
            }
    return;
}

void printcontext(struct context *c) {
    for (int i=0;i<c->size;i++) {
        printf("%s:",c->termlist[i].name);
        printtype(c->termlist[i].t);
        printf(" = ");
        printnode(c->termlist[i].expr);
        putchar('\n');
    }
}
