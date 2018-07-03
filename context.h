#ifndef __CONTEXT_
#define __CONTEXT_

struct context {
    int size;
    struct contextrecord {
        char *name;
        struct type *t;
        struct lambda *expr;
    } *termlist;
    struct typerecord {
        char *name;
        struct type *t;
    } *typelist;
};

struct context context_init();
int context_addterm(struct context *,char *,struct type *,struct lambda *);
struct contextrecord *context_findterm(struct context *,char *);
void context_deleteterm(struct context *,char *);
int context_addtype(struct context *,char *,struct type *);
struct typerecord *context_findtype(struct context *,char *);
void context_deletetype(struct context *,char *);
void context_destroy(struct context);
void printcontext(struct context *);

#endif
