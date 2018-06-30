#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "context.h"
#include "lambda.h"
#include "type.h"

struct context context_init() {
    struct context c;
    c.size=0;
    c.list=NULL;
    return c;
}

int context_add(struct context *c,char *n,struct type *t,struct lambda *l) {
    if (!c->size || !c->list) c->list=(struct contextrecord *)malloc((c->size=1)*sizeof(struct contextrecord));
    else {
        if (context_find(c,n)) return 0;
        c->list=(struct contextrecord *)realloc(c->list,++c->size*sizeof(struct contextrecord));
    }
    if (!c->size) return 0;
    c->list[c->size-1].expr=l;
    c->list[c->size-1].name=n;
    c->list[c->size-1].t=t;
    return 1;
}

struct contextrecord *context_find(struct context *c,char *name) {
    if (!c->size) return NULL;
    for(int i=c->size-1;i>=0;i--) if (!strcmp(c->list[i].name,name)) return &c->list[i];
    return NULL;

}

void context_delete(struct context *c,char *n) {
    if (c && c->list)
        for (int i=c->size-1;i>=0;i--)
            if (!strcmp(c->list[i].name,n)) {
                c->list[i]=c->list[c->size-1];
                c->list=(struct contextrecord *)realloc(c->list,--c->size*(sizeof(struct contextrecord)));
                return;
            }
    return;
}

void context_destroy(struct context c) {
    if (c.list) free(c.list);
}

void printcontext(struct context *c) {
    for (int i=0;i<c->size;i++) {
        printf("%s:",c->list[i].name);
        printtype(c->list[i].t);
        printf(" = ");
        printnode(c->list[i].expr);
        putchar('\n');
    }
}
