#include <stdlib.h>
#include "type.h"
#include "typestack.h"

int typestack_init(struct typestack *s) {
    s->size=1;
    s->top=0;
    s->list=(struct type **)malloc(sizeof(struct type *));
    if (!s->list) return 1;
    return 0;
}

int typestack_push(struct typestack *s,struct type *t) {
    s->list[s->top++]=t;
    if (s->top==s->size) s->list=(struct type **)realloc(s->list,(s->size<<=1)*sizeof(struct type *));
    if (!s->list) return 1;
    return 0;
}

struct type *typestack_pop(struct typestack *s) {
    if (!s->top) return 0;
    return s->list[--s->top];
}

