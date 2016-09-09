#ifndef COINMANAGER_H_
#define COINMANAGER_H_

class CoinManager
{
public:
	class Listener
	{
		virtual void
		onCoinInserted(int value) = 0;

		virtual void
		onReturnCoins() = 0;
	};

	CoinManager() {}
	virtual ~CoinManager() {}

	virtual void
	addListener(Listener* listener) = 0;

	virtual void
	removeListener(Listener* listener) = 0;

	virtual void
	acceptLastCoin(bool accept) = 0;

	virtual void
	returnCoins(int value) = 0;

	virtual bool
	isChangeAvailable() = 0;
};

#endif /* COINMANAGER_H_ */
