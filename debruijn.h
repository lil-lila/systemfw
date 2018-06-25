#ifndef __DEBRUIJN_
#define __DEBRUIJN_
struct ctx {
	char const **terms;
	unsigned int count;
};


extern struct ctx ctx_init();
extern int ctx_destroy(struct ctx);
extern void ctx_pop(struct ctx *);
extern int ctx_push(struct ctx *,const char *const);
extern int indexof(const struct ctx *const,const char *const);

#endif
