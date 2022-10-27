# 编译器设定和编译选项
CC = gcc
FLEX = flex
BISON = bison
CFLAGS = -std=c99

# 编译目标：src目录下的所有.c文件
CFILES = $(shell find ./ -name "*.c")
OBJS = $(CFILES:.c=.o)
LFILE = $(shell find ./ -name "*.l")
YFILE = $(shell find ./ -name "*.y")
LFC = $(shell find ./ -name "*.l" | sed s/[^/]*\\.l/lex.yy.c/)
YFC = $(shell find ./ -name "*.y" | sed s/[^/]*\\.y/syntax.tab.c/)
LFO = $(LFC:.c=.o)
YFO = $(YFC:.c=.o)

parser: syntax $(filter-out $(LFO),$(OBJS))
	$(CC) -o parser $(filter-out $(LFO),$(OBJS)) -lfl -ly

syntax: lexical syntax-c
	$(CC) -c $(YFC) -o $(YFO)

lexical: $(LFILE)
	$(FLEX) -o $(LFC) $(LFILE)

syntax-c: $(YFILE)
	$(BISON) -o $(YFC) -d  -v $(YFILE)

scanner: lexical	
	$(CC) main.c lex.yy.c -lfl -o scanner
-include $(patsubst %.o, %.d, $(OBJS))

# 定义的一些伪目标
.PHONY: clean test phase1
test:
	./parser ../Test/test.cmm

alltest:
	./parser ../Test/test1.cmm
	./parser ../Test/test2.cmm
	./parser ../Test/test3.cmm
	./parser ../Test/test4.cmm
	./parser ../Test/test5.cmm
	./parser ../Test/test6.cmm
	./parser ../Test/test7.cmm	
	./parser ../Test/test8.cmm
	./parser ../Test/test9.cmm
	./parser ../Test/test10.cmm
	./parser ../Test/test11.cmm
	./parser ../Test/test12.cmm
	./parser ../Test/test13.cmm
	./parser ../Test/test14.cmm	
	./parser ../Test/test15.cmm
	./parser ../Test/test16.cmm
	./parser ../Test/test17.cmm
	./parser ../Test/test18.cmm	
	./parser ../Test/test19.cmm
	./parser ../Test/test20.cmm
	./parser ../Test/test21.cmm
	./parser ../Test/test22.cmm

clean:
	rm -f parser lex.yy.c syntax.tab.c syntax.tab.h syntax.output scanner
	rm -f $(OBJS) $(OBJS:.o=.d)
	rm -f $(LFC) $(YFC) $(YFC:.c=.h)
	rm -f *~
phase1:
	./scanner ../Test/test2.cmm