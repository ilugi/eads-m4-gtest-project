#include "gmock/gmock.h"
#include "VendingMachine.h"
#include "CoinManager.h"
#include "ProductManager.h"
#include "Display.h"
#include <sstream>

using namespace testing;
using testing::_;
using testing::NiceMock;

class MockDisplay : public Display
{
public:
	MOCK_METHOD1(show, void(std::string));
};

class MockProductManager : public ProductManager
{
public:
	MOCK_METHOD1(addListener, void(ProductManager::Listener*));
	MOCK_METHOD1(removeListener, void(ProductManager::Listener*));
	MOCK_METHOD0(getName, std::string());
	MOCK_METHOD0(getPrice, int());
	MOCK_METHOD0(dispense, void());
	MOCK_METHOD0(getStock, int());
};

class MockCoinManager : public CoinManager
{
public:
	MOCK_METHOD1(addListener, void(CoinManager::Listener*));
	MOCK_METHOD1(removeListener, void(CoinManager::Listener*));
	MOCK_METHOD1(acceptLastCoin, void(bool));
	MOCK_METHOD1(returnCoins, void(int));
	MOCK_METHOD0(isChangeAvailable, bool());
};

TEST(VendingMachineInitialization, RegistersUnregistersOnCoinManager)
{
	MockCoinManager coinManager;
	MockDisplay display;

	EXPECT_CALL(coinManager, addListener(_));
	EXPECT_CALL(display, show(_)).Times(AnyNumber());
	EXPECT_CALL(coinManager, isChangeAvailable()).WillRepeatedly(Return(false));;
	VendingMachine* vending = new VendingMachine(&coinManager, &display);

	EXPECT_CALL(coinManager, removeListener(_));
	delete vending;
}

TEST(VendingMachineInitialization, ShowInsertCoinOnInitialization)
{
	MockCoinManager coinManager;
	MockDisplay display;

	EXPECT_CALL(coinManager, addListener(_));
	EXPECT_CALL(coinManager, isChangeAvailable()).WillRepeatedly(Return(true));;
	EXPECT_CALL(display, show("INSERT COIN"));
	VendingMachine* vending = new VendingMachine(&coinManager, &display);

	EXPECT_CALL(coinManager, removeListener(_));
	delete vending;
}

TEST(VendingMachineInitialization, ShowExactChangeOnInitializationIfNoChange)
{
	MockCoinManager coinManager;
	MockDisplay display;

	EXPECT_CALL(coinManager, addListener(_));
	EXPECT_CALL(coinManager, isChangeAvailable()).WillRepeatedly(Return(false));;
	EXPECT_CALL(display, show("EXACT CHANGE ONLY"));
	VendingMachine* vending = new VendingMachine(&coinManager, &display);

	EXPECT_CALL(coinManager, removeListener(_));
	delete vending;
}


class VendingMachineInitializedFixture : public testing::Test
{
protected:

	virtual void SetUp()
	{
		EXPECT_CALL(coinManager, addListener(_));
		EXPECT_CALL(display, show(_)).Times(AnyNumber());
		EXPECT_CALL(coinManager, isChangeAvailable()).WillRepeatedly(Return(true));
		vending = new VendingMachine(&coinManager, &display);
	}

	virtual void TearDown()
	{
		EXPECT_CALL(coinManager, removeListener(_));
		delete vending;
	}

	VendingMachine* vending;
	MockCoinManager coinManager;
	MockDisplay display;

	std::string
	getSoldOutString()
	{
		return "SOLD OUT";
	}

	std::string
	getInsertCoinString()
	{
		return "INSERT COIN";
	}

	std::string
	getThanksString()
	{
		return "THANK YOU";
	}

	std::string
	getExactChangeString()
	{
		return "EXACT CHANGE ONLY";
	}

	std::string
	getPriceString(int price)
	{
		std::stringstream message;
		message << "PRICE: " << price;
		return message.str();
	}


	std::string
	getCreditString(int credit)
	{
		std::stringstream message;
		message << "CREDIT: " << credit;
		return message.str();
	}
};

class VendingMachineCoinAcceptOrRejectFixture : public VendingMachineInitializedFixture
{
};

TEST_F(VendingMachineCoinAcceptOrRejectFixture, RejectPennies)
{
	EXPECT_CALL(coinManager, acceptLastCoin(false)).Times(1);
	EXPECT_CALL(coinManager, acceptLastCoin(true)).Times(0);
	vending->onCoinInserted(1);
}

TEST_F(VendingMachineCoinAcceptOrRejectFixture, AcceptNickles)
{
	EXPECT_CALL(coinManager, acceptLastCoin(false)).Times(0);
	EXPECT_CALL(coinManager, acceptLastCoin(true)).Times(1);
	vending->onCoinInserted(5);
}

TEST_F(VendingMachineCoinAcceptOrRejectFixture, AcceptDimes)
{
	EXPECT_CALL(coinManager, acceptLastCoin(false)).Times(0);
	EXPECT_CALL(coinManager, acceptLastCoin(true)).Times(1);
	vending->onCoinInserted(10);
}

TEST_F(VendingMachineCoinAcceptOrRejectFixture, AcceptQuarters)
{
	EXPECT_CALL(coinManager, acceptLastCoin(false)).Times(0);
	EXPECT_CALL(coinManager, acceptLastCoin(true)).Times(1);
	vending->onCoinInserted(25);
}

class VendingMachineReturnCoinsFixture : public VendingMachineCoinAcceptOrRejectFixture
{
};

TEST_F(VendingMachineReturnCoinsFixture, ReturnCoinsReturns)
{
	EXPECT_CALL(coinManager, acceptLastCoin(_)).Times(AnyNumber());

	vending->onCoinInserted(1);
	vending->onCoinInserted(5);
	vending->onCoinInserted(1);
	vending->onCoinInserted(10);
	vending->onCoinInserted(25);

	EXPECT_CALL(coinManager, returnCoins(40)).Times(1);
	vending->onReturnCoins();
}


class VendingMachineCreditUpdateFixture : public VendingMachineInitializedFixture
{
};

TEST_F(VendingMachineCreditUpdateFixture, UpdateCreditOnEveryCoin)
{
	EXPECT_CALL(coinManager, acceptLastCoin(_)).Times(AnyNumber());
	EXPECT_CALL(display, show(_)).Times(0);
	vending->onCoinInserted(1);
	EXPECT_CALL(display, show(getCreditString(5))).Times(1);
	vending->onCoinInserted(5);
	EXPECT_CALL(display, show(_)).Times(0);
	vending->onCoinInserted(1);
	EXPECT_CALL(display, show(getCreditString(15))).Times(1);
	vending->onCoinInserted(10);
	EXPECT_CALL(display, show(getCreditString(40))).Times(1);
	vending->onCoinInserted(25);
}

TEST_F(VendingMachineCreditUpdateFixture, ReturnCoinsResetsCredit)
{
	EXPECT_CALL(coinManager, acceptLastCoin(_)).Times(AnyNumber());
	vending->onCoinInserted(1);
	vending->onCoinInserted(5);
	vending->onCoinInserted(1);
	vending->onCoinInserted(10);
	vending->onCoinInserted(25);

	EXPECT_CALL(display, show(getInsertCoinString())).Times(1);
	EXPECT_CALL(coinManager, returnCoins(40)).Times(1);
	vending->onReturnCoins();
}

TEST_F(VendingMachineCreditUpdateFixture, ReturnCoinsResetsCreditWhenNoChangeAvailable)
{
	EXPECT_CALL(coinManager, isChangeAvailable()).WillRepeatedly(Return(false));
	EXPECT_CALL(coinManager, acceptLastCoin(_)).Times(AnyNumber());
	vending->onCoinInserted(1);
	vending->onCoinInserted(5);
	vending->onCoinInserted(1);
	vending->onCoinInserted(10);
	vending->onCoinInserted(25);

	EXPECT_CALL(display, show(getExactChangeString())).Times(1);
	EXPECT_CALL(coinManager, returnCoins(40)).Times(1);
	vending->onReturnCoins();
}

TEST_F(VendingMachineInitializedFixture, RegistersUnregistersOnProductManager)
{
	MockProductManager* colaManager = new MockProductManager();

	EXPECT_CALL(*colaManager, addListener(_)).Times(1);
	vending->addProduct(colaManager);
	EXPECT_CALL(*colaManager, removeListener(_)).Times(1);
}

TEST_F(VendingMachineInitializedFixture, RegistersUnregistersOnTwoProductManager)
{
	MockProductManager* colaManager = new MockProductManager();
	MockProductManager* chipsManager = new MockProductManager();

	EXPECT_CALL(*colaManager, addListener(_)).Times(1);
	EXPECT_CALL(*chipsManager, addListener(_)).Times(1);
	vending->addProduct(colaManager);
	vending->addProduct(chipsManager);
	EXPECT_CALL(*colaManager, removeListener(_)).Times(1);
	EXPECT_CALL(*chipsManager, removeListener(_)).Times(1);
}

class VendingMachineProductManagementFixture : public VendingMachineInitializedFixture
{
protected:
	virtual void SetUp()
	{
		VendingMachineInitializedFixture::SetUp();

		colaManager = new MockProductManager();
		chipsManager = new MockProductManager();
		candyManager = new MockProductManager();

		EXPECT_CALL(*colaManager, getName()).WillRepeatedly(Return("COLA"));
		EXPECT_CALL(*chipsManager, getName()).WillRepeatedly(Return("CHIPS"));
		EXPECT_CALL(*candyManager, getName()).WillRepeatedly(Return("CANDY"));

		EXPECT_CALL(*colaManager, getPrice()).WillRepeatedly(Return(100));
		EXPECT_CALL(*chipsManager, getPrice()).WillRepeatedly(Return(50));
		EXPECT_CALL(*candyManager, getPrice()).WillRepeatedly(Return(65));

		EXPECT_CALL(*colaManager, addListener(_)).Times(1);
		EXPECT_CALL(*chipsManager, addListener(_)).Times(1);
		EXPECT_CALL(*candyManager, addListener(_)).Times(1);

		EXPECT_CALL(*colaManager, getStock()).WillRepeatedly(Return(100));
		EXPECT_CALL(*chipsManager, getStock()).WillRepeatedly(Return(100));
		EXPECT_CALL(*candyManager, getStock()).WillRepeatedly(Return(100));

		vending->addProduct(colaManager);
		vending->addProduct(chipsManager);
		vending->addProduct(candyManager);
	}

	virtual void TearDown()
	{
		EXPECT_CALL(*colaManager, removeListener(_)).Times(1);
		EXPECT_CALL(*chipsManager, removeListener(_)).Times(1);
		EXPECT_CALL(*candyManager, removeListener(_)).Times(1);

		VendingMachineInitializedFixture::TearDown();
	}

	MockProductManager* colaManager;
	MockProductManager* chipsManager;
	MockProductManager* candyManager;
};

TEST_F(VendingMachineProductManagementFixture, WhenProductButtonIsPressedAndNoMoneyInsertedThenPriceIsShown)
{
	EXPECT_CALL(display, show(getPriceString(100)));
	EXPECT_CALL(display, show(getInsertCoinString()));
	vending->onProductSelected(colaManager);

	EXPECT_CALL(display, show(getPriceString(50)));
	EXPECT_CALL(display, show(getInsertCoinString()));
	vending->onProductSelected(chipsManager);

	EXPECT_CALL(display, show(getPriceString(65)));
	EXPECT_CALL(display, show(getInsertCoinString()));
	vending->onProductSelected(candyManager);
}

TEST_F(VendingMachineProductManagementFixture, WhenProductButtonIsPressedAndNoEnoughMoneyInsertedThenPriceIsShown)
{
	EXPECT_CALL(coinManager, acceptLastCoin(_)).Times(AnyNumber());
	EXPECT_CALL(coinManager, returnCoins(_)).Times(0);

	vending->onCoinInserted(25);
	EXPECT_CALL(display, show(getPriceString(50)));
	EXPECT_CALL(display, show(getCreditString(25)));
	vending->onProductSelected(chipsManager);

	vending->onCoinInserted(5);
	EXPECT_CALL(display, show(getPriceString(65)));
	EXPECT_CALL(display, show(getCreditString(30)));
	vending->onProductSelected(candyManager);

	vending->onCoinInserted(10);
	EXPECT_CALL(display, show(getPriceString(100)));
	EXPECT_CALL(display, show(getCreditString(40)));
	vending->onProductSelected(colaManager);
}

TEST_F(VendingMachineProductManagementFixture, WhenProductButtonIsPressedAndExactMoneyInsertedThenProductDispensed)
{
	EXPECT_CALL(coinManager, acceptLastCoin(_)).Times(AnyNumber());
	EXPECT_CALL(coinManager, returnCoins(_)).Times(0);

	vending->onCoinInserted(25);
	vending->onCoinInserted(25);
	vending->onCoinInserted(25);
	vending->onCoinInserted(25);
	EXPECT_CALL(*colaManager, dispense()).Times(1);
	EXPECT_CALL(display, show(getThanksString()));
	EXPECT_CALL(display, show(getInsertCoinString()));
	vending->onProductSelected(colaManager);
}

TEST_F(VendingMachineProductManagementFixture, WhenProductButtonIsPressedAndTooMuchMoneyInsertedThenChangeReturned)
{
	EXPECT_CALL(coinManager, acceptLastCoin(_)).Times(AnyNumber());

	vending->onCoinInserted(25);
	vending->onCoinInserted(25);
	vending->onCoinInserted(25);
	vending->onCoinInserted(25);
	EXPECT_CALL(*chipsManager, dispense()).Times(1);
	EXPECT_CALL(coinManager, returnCoins(50)).Times(1);
	EXPECT_CALL(display, show(getThanksString()));
	EXPECT_CALL(display, show(getInsertCoinString()));
	vending->onProductSelected(chipsManager);
}

TEST_F(VendingMachineProductManagementFixture, WhenProductButtonIsPressedAndNoStockSoldOutIsDisplayed)
{
	EXPECT_CALL(*colaManager, getStock()).WillRepeatedly(Return(0));

	EXPECT_CALL(*chipsManager, dispense()).Times(0);
	EXPECT_CALL(coinManager, returnCoins(_)).Times(0);
	EXPECT_CALL(display, show(getSoldOutString()));
	EXPECT_CALL(display, show(getInsertCoinString()));
	vending->onProductSelected(colaManager);
}

TEST_F(VendingMachineProductManagementFixture, WhenProductButtonIsPressedAndThereIsCreditAndNoStockSoldOutIsDisplayed)
{
	EXPECT_CALL(*colaManager, getStock()).WillRepeatedly(Return(0));
	EXPECT_CALL(coinManager, acceptLastCoin(_)).Times(AnyNumber());

	vending->onCoinInserted(25);

	EXPECT_CALL(*chipsManager, dispense()).Times(0);
	EXPECT_CALL(coinManager, returnCoins(_)).Times(0);
	EXPECT_CALL(display, show(getSoldOutString()));
	EXPECT_CALL(display, show(getCreditString(25)));
	vending->onProductSelected(colaManager);
}
