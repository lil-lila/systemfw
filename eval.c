#include <stdlib.h>
#include <stdbool.h>
#include "lambda.h"

struct lambda *substitute(struct lambda *where,struct lambda *with,int atindex) {
    if (!where || !with) return NULL;
    switch(where->t) {
        case LAMBDA_ATOM:
            if (where->atom.index==atindex) {
                destroynode(where);
                return dupnode(with);
            } else return where;
            break;
        case LAMBDA_APPL:
            where->appl.lhs=substitute(where->appl.lhs,with,atindex);
            where->appl.rhs=substitute(where->appl.rhs,with,atindex);
            return where;
            break;
        case LAMBDA_ABSTR:
            where->abstr.expr=substitute(where->abstr.expr,with,atindex+1);
            return where;
            break;
        default:
            break;
    };
    return where;
}

bool reducible(struct lambda *ast) {
    if (ast->t==LAMBDA_ABSTR || ast->t==LAMBDA_ATOM) return false;
    if (ast->appl.lhs->t==LAMBDA_ATOM && ast->appl.rhs->t==LAMBDA_ATOM) return false;
    if (ast->appl.lhs->t==LAMBDA_ABSTR) return true;
    if ((reducible(ast->appl.lhs) || reducible(ast->appl.rhs))) return true;
    return false;
}

struct lambda *eval(struct lambda *ast) {
    if (!ast) return 0;
    while (reducible(ast)) {
        if (ast->t==LAMBDA_APPL) {
            if (ast->appl.lhs && ast->appl.lhs->t==LAMBDA_ABSTR
             && (ast->appl.rhs && ast->appl.rhs->t==LAMBDA_ABSTR ||  ast->appl.rhs->t==LAMBDA_ATOM)) {
                struct lambda *lhs_expr=ast->appl.lhs->abstr.expr;
                struct lambda *rhs=ast->appl.rhs;
                free(ast->appl.lhs->abstr.v);
                free(ast->appl.lhs);
                free(ast);
                ast = substitute(lhs_expr, rhs, 1);
                destroynode(rhs);
            } else {
                ast->appl.lhs = eval(ast->appl.lhs);
                ast->appl.rhs = eval(ast->appl.rhs);
            }
        }
    }
    return ast;
}