#include <stdlib.h>
#include <string.h>
#include "debruijn.h"

struct ctx ctx_init() {
	struct ctx s;
	s.count=0;
	s.terms=NULL;
	return s;
}

int ctx_destroy(struct ctx s) {
	if (s.terms) free(s.terms);
	return 0;
}

int push(struct ctx *s,const char *const c) {
	if (!s->count || !s->terms) s->terms=(char const **)malloc((s->count=1)*sizeof(char *));
	else s->terms=(char const **)realloc(s->terms,++s->count*sizeof(char *));
	if (!s->terms) return -1;
	s->terms[s->count-1]=c;//(char *)malloc((strlen(c)+1)*sizeof(char));
	/*if (!s->term[s->count-1]) return -1;
	strcpy(s->term[s->count-1],c);*/
	return s->count-1;
}

int pop(struct ctx *s) {
	s->terms=(char const **)realloc(s->terms,--s->count*sizeof(char *));
}

int indexof(const struct ctx *const s,const char *const t) {
	if (!s->count) return 0;
	for(unsigned int i=0;i<s->count;i++) if (!strcmp(s->terms[i],t)) return s->count-i;
	return 0;
}

