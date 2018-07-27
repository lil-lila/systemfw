#ifndef __CONTEXT_
#define __CONTEXT_

#include "kind.h"
#include "type.h"
#include "lambda.h"

struct context {
    //int size,typesize;
    struct contextrecord {
        char *name;
        struct type *t;
        struct lambda *expr;
        struct contextrecord *prev;
    } *termlist;
    struct typerecord {
        char *name;
        struct type *t;
        struct kind *k;
        struct typerecord *prev;
    } *typelist;
};

struct context context_init();
int context_addterm(struct context *,char *,struct type *,struct lambda *);
struct contextrecord *context_findterm(const struct context *const,char *);
void context_deleteterm(struct context *,char *);
int context_addtype(struct context *,char *,struct type *,struct kind *);
struct typerecord *context_findtype(const struct context *const,char *);
struct type *fold(struct context *,struct type *);
void context_deletetype(struct context *,char *);
void context_destroy(struct context);
void printcontext(struct context *);

#endif
