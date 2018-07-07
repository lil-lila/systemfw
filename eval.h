#ifndef __EVAL_

#include <stdbool.h>
#include "context.h"

struct lambda *eval(struct lambda *,struct context *);
//bool reducible(struct lambda *);
//struct lambda *substitute(struct lambda *,struct lambda *,int);

#endif
