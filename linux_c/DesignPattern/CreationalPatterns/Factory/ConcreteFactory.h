
#ifndef _CONCRETEFACTORY_H_
#define _CONCRETEFACTORY_H_ 

#include "Factory.h"

class ConcreteFactory: public Factory
{
    public:
	~ConcreteFactory();
	ConcreteFactory();
	Product* CreateProduct();

    protected:

    private:

};

#endif //~_CONCRETEFACTORY_H_
