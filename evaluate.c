#include <stdlib.h>
#include <string.h>
#include "lambdaexpr.h"

struct lambdaexpr *copytree(const struct lambdaexpr *const orig) {
	if (!orig) return NULL;
	struct lambdaexpr *copy=mknode(orig->type);
	if (!copy) return NULL;
	switch(orig->type) {
		case TERM:
			copy->term.s=(char *)malloc((strlen(orig->term.s)+1)*sizeof(char));
			if (!copy->term.s) goto FAIL;
			strcpy(copy->term.s,orig->term.s);
			copy->term.index=orig->term.index;
			break;
		case ABSTR:
			copy->abstr.id=(char *)malloc((strlen(orig->abstr.id)+1)*sizeof(char));
			if (!copy->abstr.id) goto FAIL;
			strcpy(copy->abstr.id,orig->abstr.id);
			copy->abstr.expr=copytree(orig->abstr.expr);
			break;
		case APPL:
			copy->appl.lhs=copytree(orig->appl.lhs);
			copy->appl.rhs=copytree(orig->appl.rhs);
			break;
		default:
		FAIL:
			free(copy);
			copy=NULL;
			break;
	}
	return copy;
}

struct lambdaexpr *substitute() {

}

struct lambdaexpr *evaluate(struct lambdaexpr *ast) {
	if (ast && ast->type==APPL) {
		if ((ast->appl.lhs && ast->appl.lhs->type==ABSTR) && (ast->appl.rhs && ast->appl.rhs->type==ABSTR)) {
			//ast=substitute(ast->rhs,ast->lhs.body);
		} else if (ast->appl.lhs && ast->appl.lhs->type==ABSTR) {
			//ast->rhs=evaluate(ast->rhs);
		} else {
			//ast->lhs=evaluate(ast->lhs);
		}
	} else return ast;
}
