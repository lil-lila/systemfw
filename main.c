#include <stdlib.h>
#include <stdio.h>
#include "lc.h"
#include "lambda.h"
#include "debruijn.h"
#include "eval.h"

int main() {
	struct debruijn c=debruijn_init();
	lc_context_t *p=lc_create(&c);
	struct lambda *l;
	lc_parse(p,&l);
	debruijn_destroy(c);
	
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
	lc_destroy(p);
	return 0;
}
