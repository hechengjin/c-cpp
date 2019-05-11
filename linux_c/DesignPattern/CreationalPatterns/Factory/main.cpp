
#include "ConcreteFactory.h"
#include "ConcreteProduct.h"

#include <iostream>
using namespace std;

int main(int argc, char* argv[])
{
    Factory* fac = new ConcreteFactory();

    Product* p = fac->CreateProduct();

    return 0;
}
