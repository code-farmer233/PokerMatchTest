#pragma once

#include "CardModel.h"
#include "cocos2d.h"
#include <functional>

class CardView : public cocos2d::Node
{
public:
    using ClickCallback = std::function<void(CardView*)>;

    static CardView* create(const CardModel& model);

    const CardModel& getModel() const { return _model; }
    CardModel& getModel() { return _model; }

    void setClickCallback(const ClickCallback& callback);
    void setInputEnabled(bool enabled);
    bool isInputEnabled() const { return _inputEnabled; }

    void refresh();
    void playInvalidFeedback();

private:
    bool initWithModel(const CardModel& model);
    std::string numberPath(bool big) const;
    std::string suitPath() const;

    CardModel _model;
    cocos2d::Sprite* _background = nullptr;
    cocos2d::Sprite* _smallNumber = nullptr;
    cocos2d::Sprite* _smallSuit = nullptr;
    cocos2d::Sprite* _bigNumber = nullptr;
    cocos2d::Sprite* _bigSuit = nullptr;
    ClickCallback _clickCallback;
    bool _inputEnabled = true;
};
