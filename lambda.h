#ifndef __LAMBDA_
#define __LAMBDA_

#include <stdbool.h>
#include "type.h"

enum ltype {LAMBDA_ATOM,LAMBDA_APPL,LAMBDA_ABSTR};

struct lambda {
    enum ltype t;
    union {
        struct atom {
            int index;
            char *s;
        } atom;
        struct appl {
            bool overtype;
            struct lambda *lhs;
            union {
                struct lambda *l;
                struct type *t;
            } rhs;
        } appl;
        struct abstr {
            bool overtype;
            char *v;
            union {
                struct type *type;
                struct kind *kind;
            };
            struct lambda *expr;
        } abstr;
    };
};

struct lambda *dupnode(struct lambda *);
void destroynode(struct lambda *);
void printnode(struct lambda *);

#endif
