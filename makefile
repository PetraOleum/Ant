all: ant

ant : main.o slater.o cmap.o
	g++ -o ant main.o slater.o cmap.o -lncurses

main.o:
	g++ -c main.cpp

slater.o:
	g++ -c slater.cpp

cmap.o:
	g++ -c cmap.cpp

clean:
	rm ant *.o
