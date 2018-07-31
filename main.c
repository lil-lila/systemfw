#include <stdlib.h>
#include <stdio.h>
#include "lc.h"
#include "lambda.h"
#include "debruijn.h"
#include "eval.h"

int main() {
    struct ctx c=ctx_init();
    lc_context_t *p=lc_create(&c);
    union lt _l;
    lc_parse(p,&_l);
    //struct lambda *l=_l.l;
    ctx_destroy(&c);
    lc_destroy(p);
    return 0;
}
