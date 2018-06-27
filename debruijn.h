#ifndef __DEBRUIJN_
#define __DEBRUIJN_
struct debruijn {
	char const **terms;
	unsigned int count;
};


extern struct debruijn debruijn_init();
extern int debruijn_destroy(struct debruijn);
extern void debruijn_pop(struct debruijn *);
extern int debruijn_push(struct debruijn *,const char *const);
extern int indexof(const struct debruijn *const,const char *const);

#endif
