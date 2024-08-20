all: clean
	flex scanner.lex
	bison -d parser.ypp
	g++ -std=c++17 -g -o hw5_gal *.c *.cpp
clean:
	rm -f lex.yy.c
	rm -f parser.tab.*pp
	rm -f hw5_gal
.PHONY: all clean