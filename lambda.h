#ifndef __LAMBDA_
#define __LAMBDA_

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
            struct lambda *expr;
        } abstr;
    };
};

struct lambda *dupnode(struct lambda *);
void destroynode(struct lambda *);
void printnode(struct lambda *);

#endif
