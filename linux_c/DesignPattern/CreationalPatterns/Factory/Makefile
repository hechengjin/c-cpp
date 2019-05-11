factory:main.o ConcreteFactory.o Factory.o ConcreteProduct.o Product.o
	g++ -o factory main.o ConcreteFactory.o Factory.o ConcreteProduct.o Product.o
main.o: main.cpp
	g++ -c main.cpp
PConcreteProduct.o:ConcreteProduct.cpp ConcreteProduct.h
	g++ -c ConcreteProduct.cpp
Product.o : Product.cpp Product.h
	g++ -c Product.cpp
Factory.o: Factory.cpp Factory.h
	g++ -c Factory.cpp
ConcreteFactory.o: ConcreteFactory.cpp ConcreteFactory.h
	g++ -c ConcreteFactory.cpp
