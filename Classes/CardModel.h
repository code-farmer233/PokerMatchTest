#pragma once

#include <string>

enum class CardSuit
{
    Club,
    Diamond,
    Heart,
    Spade
};

enum class CardArea
{
    Table,
    Stock,
    Waste
};

struct CardModel
{
    int id = 0;
    int rank = 1;
    CardSuit suit = CardSuit::Club;
    CardArea area = CardArea::Table;
    bool faceUp = true;

    CardModel() = default;
    CardModel(int cardId, int cardRank, CardSuit cardSuit, CardArea cardArea)
        : id(cardId), rank(cardRank), suit(cardSuit), area(cardArea)
    {
    }
};

inline bool isRedSuit(CardSuit suit)
{
    return suit == CardSuit::Diamond || suit == CardSuit::Heart;
}

inline std::string rankName(int rank)
{
    switch (rank)
    {
    case 1:
        return "A";
    case 11:
        return "J";
    case 12:
        return "Q";
    case 13:
        return "K";
    default:
        return std::to_string(rank);
    }
}

inline std::string suitName(CardSuit suit)
{
    switch (suit)
    {
    case CardSuit::Club:
        return "club";
    case CardSuit::Diamond:
        return "diamond";
    case CardSuit::Heart:
        return "heart";
    case CardSuit::Spade:
        return "spade";
    }
    return "club";
}
