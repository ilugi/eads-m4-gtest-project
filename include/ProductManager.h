#ifndef PRODUCTMANAGER_H_
#define PRODUCTMANAGER_H_

#include <string>

class ProductManager {
public:
	class Listener
	{
	public:
		Listener() {}
		virtual ~Listener() {}

		virtual void
		onProductSelected(ProductManager* productManager) = 0;
	};

	ProductManager() {}
	virtual ~ProductManager() {}

	virtual void
	addListener(Listener* listener) = 0;

	virtual void
	removeListener(Listener* listener) = 0;

	virtual std::string
	getName() = 0;

	virtual int
	getPrice() = 0;

	virtual void
	dispense() = 0;

	virtual int
	getStock() = 0;
};

#endif /* PRODUCTMANAGER_H_ */
