#ifndef VENDINGMACHINE_H_
#define VENDINGMACHINE_H_

#include "CoinManager.h"
#include "Display.h"
#include "ProductManager.h"
#include <vector>

class VendingMachine : public CoinManager::Listener,
					   public ProductManager::Listener
{
private:
	VendingMachine();
public:
	VendingMachine (CoinManager* coinManager,
					Display* display);

	virtual ~VendingMachine();

	void
	onCoinInserted(int value);

	void
	onReturnCoins();

	void
	addProduct(ProductManager* productManager);

	void
	onProductSelected(ProductManager* productManager);

private:
	void
	showCredit() const;

	void
	showThanks() const;

	void
	showSoldOut() const;

	void
	showPrice(int price) const;

	void
	resetDisplay() const;

private:
	CoinManager* coinManager;
	Display* display;
	int credit;
	std::vector<ProductManager*> products;

	static const std::string INSERT_COIN_STRING;
	static const std::string EXACT_CHANGE_STRING;
	static const std::string CREDIT_STRING;
	static const std::string PRICE_STRING;
	static const std::string THANKS_STRING;
	static const std::string SOLD_OUT_STRING;
};

#endif /* VENDINGMACHINE_H_ */
