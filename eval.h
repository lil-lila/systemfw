#ifndef __EVAL_

#include <stdbool.h>

struct lambda *eval(struct lambda *);
bool reducible(struct lambda *);
struct lambda *substitute(struct lambda *,struct lambda *,int);

#endif
