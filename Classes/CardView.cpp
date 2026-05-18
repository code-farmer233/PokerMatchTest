#include "CardView.h"

USING_NS_CC;

namespace
{
constexpr float kCardWidth = 182.0f;
constexpr float kCardHeight = 282.0f;
}

CardView* CardView::create(const CardModel& model)
{
    auto card = new (std::nothrow) CardView();
    if (card && card->initWithModel(model))
    {
        card->autorelease();
        return card;
    }
    CC_SAFE_DELETE(card);
    return nullptr;
}

bool CardView::initWithModel(const CardModel& model)
{
    if (!Node::init())
    {
        return false;
    }

    _model = model;
    setContentSize(Size(kCardWidth, kCardHeight));
    setIgnoreAnchorPointForPosition(false);
    setAnchorPoint(Vec2::ANCHOR_MIDDLE);

    _background = Sprite::create("res/card_general.png");
    _background->setPosition(kCardWidth * 0.5f, kCardHeight * 0.5f);
    addChild(_background);

    _smallNumber = Sprite::create(numberPath(false));
    _smallNumber->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);
    _smallNumber->setPosition(18.0f, kCardHeight - 18.0f);
    addChild(_smallNumber);

    _smallSuit = Sprite::create(suitPath());
    _smallSuit->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);
    _smallSuit->setScale(0.72f);
    _smallSuit->setPosition(22.0f, kCardHeight - 72.0f);
    addChild(_smallSuit);

    _bigNumber = Sprite::create(numberPath(true));
    _bigNumber->setPosition(kCardWidth * 0.5f, kCardHeight * 0.55f);
    _bigNumber->setScale(0.62f);
    addChild(_bigNumber);

    _bigSuit = Sprite::create(suitPath());
    _bigSuit->setPosition(kCardWidth * 0.5f, 58.0f);
    _bigSuit->setScale(1.25f);
    addChild(_bigSuit);

    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = [this](Touch* touch, Event*) {
        if (!_inputEnabled || !_model.faceUp)
        {
            return false;
        }

        const auto local = convertToNodeSpace(touch->getLocation());
        return Rect(0.0f, 0.0f, kCardWidth, kCardHeight).containsPoint(local);
    };
    listener->onTouchEnded = [this](Touch*, Event*) {
        if (_clickCallback)
        {
            _clickCallback(this);
        }
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

    refresh();
    return true;
}

void CardView::setClickCallback(const ClickCallback& callback)
{
    _clickCallback = callback;
}

void CardView::setInputEnabled(bool enabled)
{
    _inputEnabled = enabled;
}

void CardView::refresh()
{
    const bool faceUp = _model.faceUp;
    _smallNumber->setVisible(faceUp);
    _smallSuit->setVisible(faceUp);
    _bigNumber->setVisible(faceUp);
    _bigSuit->setVisible(faceUp);
    setOpacity(faceUp ? 255 : 180);
}

void CardView::playInvalidFeedback()
{
    stopActionByTag(1001);
    auto origin = getPosition();
    auto shake = Sequence::create(
        MoveTo::create(0.04f, origin + Vec2(-12.0f, 0.0f)),
        MoveTo::create(0.04f, origin + Vec2(12.0f, 0.0f)),
        MoveTo::create(0.04f, origin),
        nullptr);
    shake->setTag(1001);
    runAction(shake);
}

std::string CardView::numberPath(bool big) const
{
    const auto sizeName = big ? "big" : "small";
    const auto colorName = isRedSuit(_model.suit) ? "red" : "black";
    return "res/number/" + std::string(sizeName) + "_" + colorName + "_" + rankName(_model.rank) + ".png";
}

std::string CardView::suitPath() const
{
    return "res/suits/" + suitName(_model.suit) + ".png";
}
