#include "VendingMachine.h"
#include "CoinManager.h"
#include "Display.h"
#include <sstream>

const std::string VendingMachine::INSERT_COIN_STRING = "INSERT COIN";
const std::string VendingMachine::EXACT_CHANGE_STRING = "EXACT CHANGE ONLY";
const std::string VendingMachine::CREDIT_STRING = "CREDIT";
const std::string VendingMachine::PRICE_STRING = "PRICE";
const std::string VendingMachine::THANKS_STRING = "THANK YOU";
const std::string VendingMachine::SOLD_OUT_STRING = "SOLD OUT";

VendingMachine::VendingMachine(CoinManager* coinManager,
							   Display* display)
: coinManager(coinManager), display(display), credit(0)
{
	coinManager->addListener(this);
	resetDisplay();
}

VendingMachine::~VendingMachine()
{
	coinManager->removeListener(this);
	while(!products.empty())
	{
		products.back()->removeListener(this);
		delete products.back();
		products.pop_back();
	}
}

void
VendingMachine::onCoinInserted(int value)
{
	if (value == 1)
	{
		coinManager->acceptLastCoin(false);
		return;
	}
	coinManager->acceptLastCoin(true);
	credit += value;
	resetDisplay();
}

void
VendingMachine::onReturnCoins()
{
	coinManager->returnCoins(credit);
	credit = 0;
	resetDisplay();
}

void
VendingMachine::showCredit() const
{
	std::stringstream message;
	message << CREDIT_STRING << ": " << credit;
	display->show(message.str());
}

void
VendingMachine::showPrice(int price) const
{
	std::stringstream message;
	message << PRICE_STRING << ": " << price;
	display->show(message.str());
}

void
VendingMachine::showThanks() const
{
	display->show(THANKS_STRING);
}

void
VendingMachine::showSoldOut() const
{
	display->show(SOLD_OUT_STRING);
}

void
VendingMachine::resetDisplay() const
{
	if (credit != 0)
	{
		showCredit();
		return;
	}
	if (coinManager->isChangeAvailable())
		display->show(INSERT_COIN_STRING);
	else
		display->show(EXACT_CHANGE_STRING);
}

void
VendingMachine::addProduct(ProductManager* productManager)
{
	productManager->addListener(this);
	products.push_back(productManager);
}

void
VendingMachine::onProductSelected(ProductManager* productManager)
{
	if (productManager->getStock() == 0)
	{
		showSoldOut();
	}
	else if (credit < productManager->getPrice())
	{
		showPrice(productManager->getPrice());
	}
	else
	{
		productManager->dispense();
		showThanks();
		int change = credit - productManager->getPrice();
		if (change > 0)
			coinManager->returnCoins(change);
		credit = 0;
	}
	resetDisplay();
}
