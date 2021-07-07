g++ -O2 -c simulation.cpp
g++ -O2 -c main.cpp
g++ -O2 -o run main.o simulation.o -lpthread



rm *.o