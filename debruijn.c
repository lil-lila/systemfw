#include <stdlib.h>
#include <string.h>
#include "debruijn.h"

struct debruijn debruijn_init() {
    struct debruijn s;
    s.count=0;
    s.terms=NULL;
    return s;
}

int debruijn_destroy(struct debruijn s) {
    if (s.terms) free(s.terms);
    return 0;
}

int debruijn_push(struct debruijn *s,const char *const c) {
    if (!s->count || !s->terms) s->terms=(char const **)malloc((s->count=1)*sizeof(char *));
    else s->terms=(char const **)realloc(s->terms,++s->count*sizeof(char *));
    if (!s->terms) return -1;
    s->terms[s->count-1]=c;//(char *)malloc((strlen(c)+1)*sizeof(char));
    /*if (!s->term[s->count-1]) return -1;
    strcpy(s->term[s->count-1],c);*/
    return s->count-1;
}

void debruijn_pop(struct debruijn *s) {
    s->terms=(char const **)realloc(s->terms,--s->count*sizeof(char *));
}

int indexof(const struct debruijn *const s,const char *const t) {
    if (!s->count) return 0;
    for(int i=s->count-1;i>=0;i--) if (!strcmp(s->terms[i],t)) return s->count-i;
    return 0;
}

