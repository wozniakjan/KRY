# 1. projekt do KRY
# Vigenerova sifra
#
# autor: Jan Wozniak

PROJ  = kry
SRC   = src/
FLAGS = -std=c++98 -Wall -pedantic -w
CC    = g++
OBJS  = $(shell ls src/*cpp | sed 's/src\///g' | sed 's/cpp/o/g')

%.o: $(SRC)%.cpp
	$(CC) -c $(FLAGS) $< -o $@ 

$(PROJ): $(OBJS)
	$(CC) $(FLAGS) $(OBJS) -o $(PROJ) 

run: 
		./kry < ./sifry/ahoj.txt

pack:
	tar -zcvf xwozni00.tar.gz src/* Makefile dokumentace.pdf

clean:
	rm -rf kry
	rm -rf main.o
