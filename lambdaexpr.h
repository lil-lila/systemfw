#ifndef __LAMBDAEXPR__
#define __LAMBDAEXPR__ 1

struct lambdaexpr;

struct terminal {
	/*unsigned */int index; // De Bruijn index
	char *s;
};

struct application {
	struct lambdaexpr *lhs;
	struct lambdaexpr *rhs;
};

struct abstraction {
	char *id;//struct lambdaexpr *id;
	struct lambdaexpr *expr;
};

enum nodetype {APPL,ABSTR,TERM};

struct lambdaexpr {
	enum  nodetype type;
	union {
		struct application appl;
		struct abstraction abstr;
		struct terminal term;
	};
};

struct lambdaexpr *mknode(enum nodetype);
int destroynode(struct lambdaexpr *);

#endif
