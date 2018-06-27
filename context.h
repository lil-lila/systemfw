#ifndef __CONTEXT_
#define __CONTEXT_

struct context {
    int size;
    struct contextrecord {
        char *name;
        struct type *t;
        struct lambda *expr;
    } *list;
};

struct context context_init();
int context_add(struct context *,char *,struct type *,struct lambda *);
struct contextrecord *context_find(struct context *,char *);
void context_destroy(struct context);

#endif
