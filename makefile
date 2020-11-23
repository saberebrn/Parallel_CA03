CC=g++
CFLAGS=-c -msse4.1
INCDIR=`pkg-config --cflags --libs opencv4`
CST11=-std=c++11


all: main
main: Q1.o Q2.o 
	$(CC) Q1.o $(LIBDIR) $(INCDIR) $(CST11) -o Q1
	$(CC) Q2.o $(LIBDIR) $(INCDIR) $(CST11) -o Q2

Q1.o: Q1.cpp
	$(CC) Q1.cpp $(CFLAGS) $(INCDIR) $(CST11) 

Q2.o: Q2.cpp
	$(CC) Q2.cpp $(CFLAGS) $(INCDIR) $(CST11) 

clean:
	rm -f *o Q1 Q2
