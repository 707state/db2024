#!/usr/bin/bash
flex --header-file=lex.yy.hpp -o lex.yy.cpp lex.l
bison --defines=yacc.tab.h -o yacc.tab.cpp yacc.y
