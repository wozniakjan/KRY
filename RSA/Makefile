# 2. projekt do KRY
# Implementace a prolomeni RSA 
#
# autor: Jan Wozniak

PROJ  = kry
SRC   = src/
FLAGS = -std=c++98 -Wall -pedantic -w
CC    = g++
OBJS  = $(shell ls src/*cpp | sed 's/src\///g' | sed 's/cpp/o/g')
LIBS  = -lgmp

%.o: $(SRC)%.cpp
	$(CC) -c $(FLAGS) $< -o $@ $(LIBS) 

$(PROJ): $(OBJS)
	$(CC) $(FLAGS) $(OBJS) -o $(PROJ) $(LIBS) 

run: 
	./kry -g 8

pack:
	tar -zcvf xwozni00.tar.gz src/* Makefile dokumentace.pdf

clean:
	rm -rf kry
	rm -rf *.o
