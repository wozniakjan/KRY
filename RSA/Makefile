# 2. projekt do KRY
# Implementace a prolomeni RSA 
#
# autor: Jan Wozniak

PROJ  = kry
SRC   = src/
FLAGS = -std=c99 -Wall -pedantic -w
CC    = gcc
LIBS  = -lgmp

$(PROJ): $(SRC)main.c 
	$(CC) $(FLAGS) $(SRC)main.c -o $(PROJ) $(LIBS) 

run: 
	./kry -g 8

pack:
	tar -zcvf xwozni00.tar.gz src/* Makefile dokumentace.pdf

clean:
	rm -rf kry
	rm -rf *.o
