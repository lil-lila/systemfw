CC=cc
CFLAGS=-Wall -g
OBJ=context.o debruijn.o eval.o infertype.o lambda.o lc.o main.o type.o typestack.o unify.o
VG=valgrind
VGARGS=-v --leak-resolution=high --show-leak-kinds=all --track-origins=yes --log-file=summary --leak-check=full --leak-check-heuristics=all
.PHONY: clean check debug

a.out: $(OBJ)
	$(CC) $(OBJ) $(LDFLAGS)

debug : CFLAGS += -pg -fprofile-arcs -ftest-coverage
debug : LDFLAGS += -lgcov
debug : a.out

check: a.out
	$(VG) $(VGARGS) ./a.out < ts

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

lc.c: lc.peg
	./pcc lc.peg

clean:
	rm -rf a.out *.o *.gcno *.gcda *.gcov lc.c lc.h
