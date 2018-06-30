#ifndef _TYPESTACK__
#define _TYPESTACK__

#include "type.h"

struct typestack {
    int size;
    int top;
    struct type **list;
};

int typestack_init(struct typestack *s);
int typestack_push(struct typestack *s,struct type *t);
struct type *typestack_pop(struct typestack *s);

#endif
