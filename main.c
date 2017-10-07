#include <stdlib.h>
#include <stdio.h>
#include "lc.h"
#include "lambdaexpr.h"
#include "debruijn.h"
#include "evaluate.h"

int main() {
	struct ctx c=ctx_init();
	lc_context_t *p=lc_create(&c);
	struct lambdaexpr *l,*m;
	lc_parse(p,&l);
	printnode(l);
	putchar('\n');
	m=substitute(l->appl.lhs,l->appl.rhs,0);
	printnode(m);
	putchar('\n');
	destroynode(l);
	destroynode(m);
	lc_destroy(p);
	ctx_destroy(c);
	return 0;
}
