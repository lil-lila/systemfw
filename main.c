#include <stdlib.h>
#include <stdio.h>
#include "lc.h"
#include "lambdaexpr.h"
#include "debruijn.h"

int main() {
	struct ctx c=ctx_init();
	lc_context_t *p=lc_create(&c);
	struct lambdaexpr *l;
	lc_parse(p,&l);
	destroynode(l);
	lc_destroy(p);
	ctx_destroy(c);
	return 0;
}
