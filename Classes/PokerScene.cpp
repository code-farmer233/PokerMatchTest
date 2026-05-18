#include "PokerScene.h"

USING_NS_CC;

namespace
{
const Vec2 kWastePosition(620.0f, 330.0f);
const Vec2 kStockPosition(300.0f, 330.0f);
}

Scene* PokerScene::createScene()
{
    return PokerScene::create();
}

bool PokerScene::init()
{
    if (!Scene::init())
    {
        return false;
    }

    FileUtils::getInstance()->addSearchPath("res");

    auto visibleSize = Director::getInstance()->getVisibleSize();
    auto origin = Director::getInstance()->getVisibleOrigin();

    auto background = LayerColor::create(Color4B(34, 118, 74, 255), 1080.0f, 2080.0f);
    addChild(background, -10);

    auto title = Label::createWithSystemFont("Poker Match Test", "Arial", 54);
    title->setPosition(origin + Vec2(visibleSize.width * 0.5f, 1960.0f));
    addChild(title);

    auto tableTitle = Label::createWithSystemFont("Table Cards", "Arial", 36);
    tableTitle->setAnchorPoint(Vec2(0.0f, 0.5f));
    tableTitle->setPosition(80.0f, 1540.0f);
    addChild(tableTitle);

    auto handTitle = Label::createWithSystemFont("Stock / Waste", "Arial", 36);
    handTitle->setAnchorPoint(Vec2(0.0f, 0.5f));
    handTitle->setPosition(80.0f, 620.0f);
    addChild(handTitle);

    _controller.setWastePosition(kWastePosition);
    _controller.setLockCallback([this](bool) {
        updateUndoButton();
    });

    struct CardDef
    {
        int id;
        int rank;
        CardSuit suit;
        Vec2 position;
        int zOrder;
    };

    const std::vector<CardDef> tableCards = {
        {1, 3, CardSuit::Diamond, Vec2(390.0f, 1250.0f), 1},
        {2, 2, CardSuit::Spade, Vec2(650.0f, 1250.0f), 1},
    };

    for (const auto& def : tableCards)
    {
        auto card = createCard(CardModel(def.id, def.rank, def.suit, CardArea::Table), def.position, def.zOrder);
        _controller.addTableCard(card);
    }

    auto club4 = createCard(CardModel(100, 4, CardSuit::Club, CardArea::Waste), kWastePosition, 2);
    _controller.setInitialWasteCard(club4);

    const std::vector<CardDef> stockCards = {
        {200, 13, CardSuit::Spade, kStockPosition + Vec2(12.0f, 12.0f), 2},
        {201, 1, CardSuit::Heart, kStockPosition, 3},
    };

    for (const auto& def : stockCards)
    {
        auto card = createCard(CardModel(def.id, def.rank, def.suit, CardArea::Stock), def.position, def.zOrder);
        _controller.addStockCard(card);
    }

    auto undoLabel = Label::createWithSystemFont("Undo", "Arial", 42);
    _undoItem = MenuItemLabel::create(undoLabel, [this](Ref*) {
        _controller.undo();
        updateUndoButton();
    });
    _undoItem->setPosition(900.0f, 330.0f);

    auto menu = Menu::create(_undoItem, nullptr);
    menu->setPosition(Vec2::ZERO);
    addChild(menu, 20);

    updateUndoButton();
    return true;
}

CardView* PokerScene::createCard(const CardModel& model, const Vec2& position, int zOrder)
{
    auto card = CardView::create(model);
    card->setPosition(position);
    card->setClickCallback([this](CardView* clicked) {
        _controller.handleCardClicked(clicked);
        updateUndoButton();
    });
    addChild(card, zOrder);
    _cards.push_back(card);
    return card;
}

void PokerScene::updateUndoButton()
{
    if (!_undoItem)
    {
        return;
    }
    const bool enabled = _controller.canUndo();
    _undoItem->setEnabled(enabled);
    _undoItem->setOpacity(enabled ? 255 : 120);
}
