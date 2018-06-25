#ifndef __LAMBDA_
#define __LAMBDA_

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
            struct lambda *lhs,*rhs;
        } appl;
        struct abstr {
            char *v;
            struct type *type;
            struct lambda *expr;
        } abstr;
    };
};

struct lambda *dupnode(struct lambda *);
void destroynode(struct lambda *);
void printnode(struct lambda *);

#endif
