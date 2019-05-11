#include "ConcreteFactory.h"
#include "ConcreteProduct.h"

#include <iostream>

using namespace std;


ConcreteFactory::ConcreteFactory()
{
    cout<<"ConcreteFactory...."<<endl;
}

ConcreteFactory::~ConcreteFactory()
{

}

Product* ConcreteFactory::CreateProduct()
{
    return new ConcreteProduct();
}
