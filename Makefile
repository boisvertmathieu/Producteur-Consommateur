tp2: Produit.o File.o main.o
	g++ Produit.o File.o main.o -o tp2 -lpthread

main.o: main.cpp
	g++ -c main.cpp

File.o: File.cpp
	g++ -c File.cpp 

Produit.o: Produit.cpp
	g++ -c Produit.cpp 

clean:
	rm -rf *.o tp2
