#include "pch.h"
#include "TradeItem.h"
#include "PlayerTrade.h"

PlayerTrade::PlayerTrade()
{
}

PlayerTrade::~PlayerTrade()
{
}

void PlayerTrade::CheckTrade(TradeWrapper trade)
{
	ArrayWrapper<OnlineProductWrapper> me = trade.GetSendingProducts();
	ArrayWrapper<OnlineProductWrapper> him = trade.GetReceivingProducts();

	std::vector<TradeWrapper::Currency> meCurrency = trade.GetSendingCurrency();
	std::vector<TradeWrapper::Currency> himCurrency = trade.GetReceivingCurrency();

	Clear();

	AddItems(me, sendValue);
	AddItems(him, recvValue);

	AddCredits(meCurrency, sendValue);
	AddCredits(himCurrency, recvValue);
}

void PlayerTrade::Clear()
{
	sendValue = TradeValue();
	recvValue = TradeValue();
}

void PlayerTrade::Render(CanvasWrapper canvas, Vector2 pos, Vector2 size, bool useAvg)
{
	// Draw box
	canvas.SetColor(0, 0, 0, 55);
	canvas.SetPosition(pos);
	canvas.FillBox(size);

	string gMin = std::to_string(sendValue.min);
	string gMax = std::to_string(sendValue.max);
	string rMin = std::to_string(recvValue.min);
	string rMax = std::to_string(recvValue.max);

	int rSum = (recvValue.min + recvValue.max) / 2;
	int gSum = (sendValue.min + sendValue.max) / 2;

	canvas.SetColor(255, 255, 255, 255);
	canvas.SetPosition(Vector2{ pos.X + 5, pos.Y + 5 });

	if (useAvg)
		canvas.DrawString("Give up: " + std::to_string(gSum));
	else
		canvas.DrawString("Give up: " + gMin + " - " + gMax);

	canvas.SetColor(255, 255, 255, 255);
	canvas.SetPosition(Vector2{ pos.X + 5, pos.Y + 20 });

	if (useAvg)
		canvas.DrawString("Receive: " + std::to_string(rSum));
	else
		canvas.DrawString("Receive: " + rMin + " - " + rMax);

	canvas.SetColor(255, 255, 255, 130);
	canvas.DrawLine(
		Vector2{ pos.X + 5, pos.Y + 43 },
		Vector2{ pos.X + size.X - 5, pos.Y + 43 }
	);
	string state = "Result: ";

	rSum == gSum ? canvas.SetColor(255, 255, 255, 255) :
		rSum > gSum ? canvas.SetColor(50, 168, 82, 255) :
		canvas.SetColor(168, 50, 50, 255);

	rSum == gSum ? state = "Result: " :
		rSum > gSum ? state = "Profit: " : state = "Loss: ";

	canvas.SetPosition(Vector2{ pos.X + 5, pos.Y + 45 });

	if (useAvg)
		canvas.DrawString(state + std::to_string(rSum - gSum));
	else
		canvas.DrawString(state +
			std::to_string(std::abs(recvValue.min - sendValue.min)) +
			" - " +
			std::to_string(std::abs(recvValue.max - sendValue.max)));
}

void PlayerTrade::AddItems(ArrayWrapper<OnlineProductWrapper> items, TradeValue& trader)
{
	for (TradeItem i : items)
	{
		PaintPrice price = i.GetPrice();
		LOG("Adding: {} | {} - {}", i.GetLongLabel().ToString(), price.min, price.max);
		trader.min += price.min;
		trader.max += price.max;
	}
}

void PlayerTrade::AddCredits(std::vector<TradeWrapper::Currency> currency, TradeValue& trader)
{
	for (TradeWrapper::Currency c : currency)
	{
		if (c.currency_id == Currencies::Credits)
		{
			LOG("Adding: {} cr", c.quantity);
			trader.min += c.quantity;
			trader.max += c.quantity;
		}
	}
}
