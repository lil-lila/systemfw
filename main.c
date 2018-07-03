#include <stdlib.h>
#include <stdio.h>
#include "lc.h"
#include "lambda.h"
#include "debruijn.h"
#include "eval.h"

int main() {
    struct ctx c=ctx_init();
    context_addterm(&c.ctx,"add",type_function(mktype(TYPE_NAME,"N",0),type_function(mktype(TYPE_NAME,"N",0),mktype(TYPE_NAME,"N",0))),NULL);
    context_addterm(&c.ctx,"a1",mktype(TYPE_NAME,"N",0),NULL);
    context_addterm(&c.ctx,"a2",mktype(TYPE_NAME,"N",0),NULL);
    lc_context_t *p=lc_create(&c);
    union lt _l;
    lc_parse(p,&_l);
    struct lambda *l=_l.l;
    ctx_destroy(&c);
    lc_destroy(p);
    /*
    printnode(l);
    putchar('\n');
    putchar('\n');
    printf("is:%d\n",reducible(l));
    struct lambda *m=dupnode(l);
    m=eval(m);
    printf("m:");
    printnode(m);
    putchar('\n');
    destroynode(m);
    destroynode(l);*/
    return 0;
}
