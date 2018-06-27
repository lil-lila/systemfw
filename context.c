#include <stdlib.h>
#include "context.h"

struct context context_init() {
    struct context c;
    c.size=0;
    c.list=NULL;
}

int context_add(struct context *c,char *n,struct type *t,struct lambda *l) {
    if (!c->size || !c->list) c->list=(struct contextrecord *)malloc((c->size=1)*sizeof(struct contextrecord));
    else c->list=(struct contextrecord *)realloc(c->list,++c->size*sizeof(struct contextrecord));
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

void context_destroy(struct context *c) {
    free(c->list);
}
