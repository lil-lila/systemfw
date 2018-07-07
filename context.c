#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "context.h"
#include "lambda.h"
#include "type.h"

struct context context_init() {
    struct context c;
    //c.size=0;
    //c.typesize=0;
    c.termlist=NULL;
    c.typelist=NULL;
    return c;
}

int context_addterm(struct context *c,char *n,struct type *t,struct lambda *l) {
    struct contextrecord *p=c->termlist;
    if (!c->termlist) c->termlist=(struct contextrecord *)malloc(sizeof(struct contextrecord));
    else {
        if (context_findterm(c,n)) return 0;
        c->termlist=(struct contextrecord *)malloc(sizeof(struct contextrecord));
    }
    if (!c->termlist) return 0;
    c->termlist->prev=p;
    c->termlist->expr=l;
    c->termlist->name=n;
    c->termlist->t=t;
    return 1;
}

struct contextrecord *context_findterm(struct context *c,char *name) {
    struct contextrecord *r;
    if (!(r=c->termlist)) return NULL;
    while (r && strcmp(r->name,name)) r=r->prev;
    return r;

}

void context_deleteterm(struct context *c,char *n) {
    if (!c) return;
    struct contextrecord *cur=c->termlist,*next=NULL;
    while (cur && strcmp(n,cur->name)) {
        next=cur;
        cur=cur->prev;
    }
    if (cur) {
        if (next) next->prev=cur->prev;
        else c->termlist=c->termlist->prev;
        free(cur);
    }
}

void context_destroy(struct context c) {
    struct contextrecord *ccur=c.termlist;
    while (ccur) {
        struct contextrecord *p=ccur->prev;
        free(ccur->name);
        destroynode(ccur->expr);
        destroytype(ccur->t);
        free(ccur);
        ccur=p;
    }
    struct typerecord *tcur=c.typelist;
    while (tcur) {
        struct typerecord *p=tcur->prev;
        free(tcur->name);
        destroytype(tcur->t);
        free(tcur);
        tcur=p;
    }
}

int context_addtype(struct context *c,char *n,struct type *t) {
    struct typerecord *p=c->typelist;
    if (!c->typelist) c->typelist=(struct typerecord *)malloc(sizeof(struct typerecord));
    else {
        if (context_findtype(c,n)) return 0;
        c->typelist=(struct typerecord *)malloc(sizeof(struct typerecord));
    }
    if (!c->typelist) return 0;
    c->typelist->prev=p;
    c->typelist->name=n;
    c->typelist->t=t;
    return 1;
}

struct typerecord *context_findtype(struct context *c,char *name) {
    struct typerecord *r;
    if (!(r=c->typelist)) return NULL;
    while (r && strcmp(r->name,name)) r=r->prev;
    return r;

}

void context_deletetype(struct context *c,char *n) {
    if (!c) return;
    struct typerecord *cur=c->typelist,*next=NULL;
    while (cur && strcmp(n,cur->name)) {
        next=cur;
        cur=cur->prev;
    }
    if (cur) {
        if (next) next->prev=cur->prev;
        else c->typelist=c->typelist->prev;
        free(cur);
    }
}

struct type *fold(struct context *c,struct type *f) {
    if (!c) return NULL;
    struct typerecord *cur=c->typelist;
    while (cur) {
        if (cmptype(cur->t,f)) return mktype(TYPE_NAME,strdup(cur->name),0);
        cur=cur->prev;
    }
    return NULL;

}

/*
int context_addtype(struct context *c,char *n,struct type *t) {
    if (!c->typesize || !c->typelist) c->typelist=(struct typerecord *)malloc((c->typesize=1)*sizeof(struct typerecord));
    else {
        if (context_findtype(c,n)) return 0;
        c->typelist=(struct typerecord *)realloc(c->typelist,++c->typesize*sizeof(struct typerecord));
    }
    if (!c->typelist) return 0;
    c->typelist[c->typesize-1].name=n;
    c->typelist[c->typesize-1].t=t;
    return 1;
}

struct typerecord *context_findtype(struct context *c,char *name) {
    if (!c->typesize) return NULL;
    for(int i=c->typesize-1;i>=0;i--) if (!strcmp(c->typelist[i].name,name)) return &c->typelist[i];
    return NULL;

}

struct type *fold(struct context *c,struct type *f) {
    if (!c) return NULL;
    for(int i=c->typesize-1;i>=0;i--) if (cmptype(c->typelist[i].t,f)) return mktype(TYPE_NAME,c->typelist[i].name,0);
    return NULL;

}

void context_deletetype(struct context *c,char *n) {
    if (c && c->typelist)
        for (int i=c->typesize-1;i>=0;i--)
            if (!strcmp(c->typelist[i].name,n)) {
                c->typelist[i]=c->typelist[c->typesize-1];
                c->typelist=(struct typerecord *)realloc(c->typelist,--c->typesize*(sizeof(struct typerecord)));
                return;
            }
    return;
}
*/
void printcontext(struct context *c) {
    struct contextrecord *ccur=c->termlist;
    while (ccur) {
        struct contextrecord *p=ccur->prev;
        printf("%s:",ccur->name);
        printtype(ccur->t);
        printf(" = ");
        printnode(ccur->expr);
        putchar('\n');
        ccur=p;
    }
    struct typerecord *tcur=c->typelist;
    while (tcur) {
        struct typerecord *p=tcur->prev;
        printf(":%s",tcur->name);
        printf(" = ");
        printtype(tcur->t);
        putchar('\n');
        tcur=p;
    }
}
