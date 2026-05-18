#pragma once

#include "CardGameController.h"
#include "cocos2d.h"
#include <vector>

class PokerScene : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();
    virtual bool init() override;
    CREATE_FUNC(PokerScene);

private:
    CardView* createCard(const CardModel& model, const cocos2d::Vec2& position, int zOrder);
    void updateUndoButton();

    CardGameController _controller;
    cocos2d::MenuItemLabel* _undoItem = nullptr;
    std::vector<CardView*> _cards;
};
