#ifndef __TYPE_
#define __TYPE_

enum ttype {
    /* type variable */
    TYPE_VARIABLE,
    /* type operator */
    /*TYPE_INTEGER, //arity = 0
    TYPE_BOOLEAN,
    TYPE_STRING,*/
    TYPE_NAME,
    TYPE_FUNCTION, //arity = 2
    TYPE_PAIR
    //,TYPE_LIST //arity = 1
};

struct type {
    enum ttype t;
    char *name;
    //size_t id;
    int arity;
    struct type *args[2]; // for function,pair
    /* for type variables */
    struct type *instance;
};

struct type *mktype(enum ttype,char *,int);
struct type *type_function(struct type *,struct type *);
struct type *type_var(char *);
void printtype(struct type *);
void destroytype(struct type *);

#endif
