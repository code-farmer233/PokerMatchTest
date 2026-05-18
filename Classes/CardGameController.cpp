#include "CardGameController.h"

#include <algorithm>
#include <cmath>

USING_NS_CC;

void CardGameController::addTableCard(CardView* card)
{
    if (!card)
    {
        return;
    }
    card->getModel().area = CardArea::Table;
    _tableCards.push_back(card);
    refreshInputState();
}

void CardGameController::addStockCard(CardView* card)
{
    if (!card)
    {
        return;
    }
    card->getModel().area = CardArea::Stock;
    _stockCards.push_back(card);
    refreshStockTop();
    refreshInputState();
}

void CardGameController::setInitialWasteCard(CardView* card)
{
    _wasteTop = card;
    if (_wasteTop)
    {
        _wasteTop->getModel().area = CardArea::Waste;
        _wasteTop->setPosition(_wastePosition);
    }
    refreshInputState();
}

void CardGameController::handleCardClicked(CardView* card)
{
    if (_locked || !card)
    {
        return;
    }

    const auto area = card->getModel().area;
    if (area == CardArea::Stock && card == _stockTop)
    {
        moveCardToWaste(card, ActionType::DrawStockCard);
        return;
    }

    if (area == CardArea::Table)
    {
        if (canMatch(card))
        {
            moveCardToWaste(card, ActionType::MatchTableCard);
        }
        else
        {
            card->playInvalidFeedback();
        }
    }
}

void CardGameController::undo()
{
    if (_locked || _history.empty())
    {
        return;
    }

    auto record = _history.back();
    _history.pop_back();

    setLocked(true);
    _wasteTop = record.previousWasteTop;
    _stockTop = record.previousStockTop;

    record.card->getModel().area = record.fromArea;
    record.card->getModel().faceUp = record.fromFaceUp;
    record.card->setLocalZOrder(record.previousLocalZ);
    record.card->refresh();

    // 如果卡牌回到 Stock，需要重新添加到数组
    if (record.fromArea == CardArea::Stock && record.actionType == ActionType::DrawStockCard)
    {
        auto it = std::find(_stockCards.begin(), _stockCards.end(), record.card);
        if (it == _stockCards.end())
        {
            _stockCards.push_back(record.card);
        }
    }

    auto restore = Sequence::create(
        MoveTo::create(0.22f, record.fromPosition),
        CallFunc::create([this]() {
            refreshStockTop();
            refreshInputState();
            setLocked(false);
        }),
        nullptr);
    record.card->runAction(restore);
}

bool CardGameController::canMatch(const CardView* tableCard) const
{
    if (!_wasteTop || !tableCard)
    {
        return false;
    }
    return std::abs(tableCard->getModel().rank - _wasteTop->getModel().rank) == 1;
}

void CardGameController::moveCardToWaste(CardView* card, ActionType actionType)
{
    MoveRecord record;
    record.actionType = actionType;
    record.card = card;
    record.fromPosition = card->getPosition();
    record.fromArea = card->getModel().area;
    record.fromFaceUp = card->getModel().faceUp;
    record.previousWasteTop = _wasteTop;
    record.previousStockTop = _stockTop;
    record.previousLocalZ = card->getLocalZOrder();
    _history.push_back(record);

    if (actionType == ActionType::DrawStockCard)
    {
        // 从 _stockCards 中移除该卡牌，避免后续 refreshStockTop() 重复处理
        auto it = std::find(_stockCards.begin(), _stockCards.end(), card);
        if (it != _stockCards.end())
        {
            _stockCards.erase(it);
        }
        _stockTop = nullptr;
    }

    setLocked(true);
    card->getModel().area = CardArea::Waste;
    card->getModel().faceUp = true;
    card->refresh();
    card->setLocalZOrder(100 + static_cast<int>(_history.size()));
    refreshStockTop();
    refreshInputState();

    auto move = Sequence::create(
        MoveTo::create(0.22f, _wastePosition),
        CallFunc::create([this, card]() {
            _wasteTop = card;
            refreshStockTop();
            refreshInputState();
            setLocked(false);
        }),
        nullptr);
    card->runAction(move);
}

void CardGameController::refreshStockTop()
{
    _stockTop = nullptr;
    for (auto it = _stockCards.rbegin(); it != _stockCards.rend(); ++it)
    {
        auto card = *it;
        if (card && card->getModel().area == CardArea::Stock)
        {
            _stockTop = card;
            break;
        }
    }

    // 只更新仍在 Stock 区域的牌，不要更新已移动到 Waste 的牌
    for (auto* card : _stockCards)
    {
        if (!card || card->getModel().area != CardArea::Stock)
        {
            continue;
        }
        const bool isTop = card == _stockTop;
        card->getModel().faceUp = isTop;
        card->refresh();
    }
}

void CardGameController::refreshInputState()
{
    for (auto* card : _tableCards)
    {
        if (card)
        {
            card->setInputEnabled(!_locked && card->getModel().area == CardArea::Table && card->getModel().faceUp);
        }
    }

    for (auto* card : _stockCards)
    {
        if (card)
        {
            card->setInputEnabled(!_locked && card == _stockTop);
        }
    }

    if (_wasteTop)
    {
        _wasteTop->setInputEnabled(false);
    }
}

void CardGameController::setLocked(bool locked)
{
    _locked = locked;
    refreshInputState();
    if (_lockCallback)
    {
        _lockCallback(locked);
    }
}
