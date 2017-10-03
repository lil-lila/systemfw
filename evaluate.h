#ifndef __EVALUATE__
#define __EVALUATE__ 1

extern struct lambdaexpr *copytree(const struct lambdaexpr *const,const int);
extern struct lambdaexpr *evaluate(struct lambdaexpr *);

#endif
