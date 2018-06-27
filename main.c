#include <stdlib.h>
#include <stdio.h>
#include "lc.h"
#include "lambda.h"
#include "debruijn.h"
#include "eval.h"

int main() {
    struct ctx c;
    c.db=debruijn_init();
    c.ctx=context_init();
    lc_context_t *p=lc_create(&c);
    struct lambda *l;
    lc_parse(p,&l);
    debruijn_destroy(c.db);
    for (int i=0;i<c.ctx.size;i++) {
        printf("%s:",c.ctx.list[i].name);
        //printtype(c.ctx.list[i].t);
        printf(" = ");
        printnode(c.ctx.list[i].expr);
        putchar('\n');
    }
    context_destroy(c.ctx);
    lc_destroy(p);

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
    destroynode(l);
    return 0;
}
