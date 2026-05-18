#pragma once

#include "CardView.h"
#include "cocos2d.h"
#include <functional>
#include <vector>

class CardGameController
{
public:
    using LockCallback = std::function<void(bool)>;

    void setWastePosition(const cocos2d::Vec2& position) { _wastePosition = position; }
    void setLockCallback(const LockCallback& callback) { _lockCallback = callback; }

    void addTableCard(CardView* card);
    void addStockCard(CardView* card);
    void setInitialWasteCard(CardView* card);

    void handleCardClicked(CardView* card);
    void undo();
    bool canUndo() const { return !_history.empty() && !_locked; }

private:
    enum class ActionType
    {
        MatchTableCard,
        DrawStockCard
    };

    struct MoveRecord
    {
        ActionType actionType = ActionType::MatchTableCard;
        CardView* card = nullptr;
        cocos2d::Vec2 fromPosition;
        CardArea fromArea = CardArea::Table;
        bool fromFaceUp = true;
        CardView* previousWasteTop = nullptr;
        CardView* previousStockTop = nullptr;
        int previousLocalZ = 0;
    };

    bool canMatch(const CardView* tableCard) const;
    void moveCardToWaste(CardView* card, ActionType actionType);
    void refreshStockTop();
    void refreshInputState();
    void setLocked(bool locked);

    cocos2d::Vec2 _wastePosition;
    std::vector<CardView*> _tableCards;
    std::vector<CardView*> _stockCards;
    std::vector<MoveRecord> _history;
    CardView* _wasteTop = nullptr;
    CardView* _stockTop = nullptr;
    LockCallback _lockCallback;
    bool _locked = false;
};
