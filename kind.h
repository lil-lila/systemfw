#ifndef __kind_
#define __kind_

enum tkind {
    /* kind variable */
    KIND_VARIABLE,
    /* kind operator */
    /*kind_INTEGER, //arity = 0
    kind_BOOLEAN,
    kind_STRING,*/
    KIND_PROP,
    KIND_FUNCTION, //arity = 2
    //,kind_LIST //arity = 1
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
void destroykind(struct kind *);

#endif
