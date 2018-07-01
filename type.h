#ifndef __TYPE_
#define __TYPE_

enum ttype {
    /* type variable */
    TYPE_VARIABLE,
    /* type operator */
    /*TYPE_INTEGER, //arity = 0
    TYPE_BOOLEAN,
    TYPE_STRING,*/
    TYPE_POLY, //over type with name 'name'
    TYPE_NAME,
    TYPE_FUNCTION, //arity = 2
    TYPE_PAIR
    //,TYPE_LIST //arity = 1
};

struct type {
    enum ttype t;
    int arity,index;
    char *name;
    struct type *args[2];
};

struct type *mktype(enum ttype,char *,int);
struct type *type_function(struct type *,struct type *);
struct type *type_var(char *);
void printtype(struct type *);
void destroytype(struct type *);

#endif
