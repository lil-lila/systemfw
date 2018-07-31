#ifndef __KIND_
#define __KIND_

#include <stdbool.h>

enum tkind {
    /* kind variable */
    KIND_VARIABLE,
    /* kind operator */
    KIND_PROP,
    KIND_FUNCTION, //arity = 2
};

struct kind {
    enum tkind t;
    int arity;
    struct kind *args[2]; // for function,pair
};

struct kind *mkkind(enum tkind,int);
struct kind *kind_function(struct kind *,struct kind *);
struct kind *kind_var();
void printkind(struct kind *);
bool cmpkind(struct kind *,struct kind *);
struct kind *dupkind(struct kind *);
void destroykind(struct kind *);

#endif
