#ifndef __INFERTYPE_
#define __INFERTYPE_

struct type *infertype(struct lambda *,struct context *);
struct type *evaltype(struct type *,struct context *);
struct type *subtype(struct type *,struct type *,int);

#endif
