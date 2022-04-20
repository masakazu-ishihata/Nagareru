# Makefile
TAR  = nagareru
CC   = g++ -O3 -Wall -std=c++11
LIBS = -lm
INCS = 
OBJS = main.o

all : $(TAR)

$(TAR): $(OBJS)
	$(CC) -o $@ $(OBJS) $(LIBS) $(INCS)

clean:
	rm -f $(OBJS) $(TAR) *~ *.bak *.dot *.lst *.gif *.cnf *.sol

.cpp.o:
	$(CC) $(INCS) -c $<

main.o: main.hpp NagareruSpec.hpp NagareruProblem.hpp NagareruProblemGenerator.hpp FrontierManager.hpp
