#include <iostream>
#include <vector>
#include <string>
#include <ctime>     
#include <cstdlib>   

using namespace std;

struct Card {
    enum Suit { Hearts, Diamonds, Clubs, Spades };
    enum Rank {
        Ace = 1, Two, Three, Four, Five, Six, Seven, Eight, Nine, Ten,
        Jack, Queen, King
    };

    Rank rank;
    Suit suit;

    Card(Rank rank, Suit suit) : rank(rank), suit(suit) {}

    string toString() const {
        const string ranks[] = { "", "Ace", "Two", "Three", "Four", "Five", "Six", "Seven",
            "Eight", "Nine", "Ten", "Jack", "Queen", "King" };
        const string suits[] = { "Hearts", "Diamonds", "Clubs", "Spades" };
        return ranks[rank] + " of " + suits[suit];
    }
};

struct Deck {
    vector<Card> cards;

    Deck() {
        for (int suit = 0; suit < 4; ++suit) {
            for (int rank = 1; rank <= 13; ++rank) {
                cards.emplace_back(static_cast<Card::Rank>(rank), static_cast<Card::Suit>(suit));
            }
        }
    }

    void shuffle() {
        srand(time(nullptr)); 
        for (size_t i = cards.size() - 1; i > 0; --i) {
            size_t j = rand() % (i + 1);
            Card temp = cards[i];
            cards[i] = cards[j];
            cards[j] = temp;
        }
    }

    Card dealCard() {
        if (cards.empty()) {
            throw out_of_range("No cards left in the deck");
        }
        Card card = cards.back();
        cards.pop_back();
        return card;
    }

    bool isEmpty() const {
        return cards.empty();
    }
};

int main() {
   
    //srand(time(nullptr));
    Deck deck;
    deck.shuffle();

    cout << "Dealing cards from the deck:\n";
    try {
        for (int i = 0; i < 52; ++i) {
            Card card = deck.dealCard();
            cout << card.toString() << endl;
        }
    }
    catch (const out_of_range& e) {
        cerr << e.what() << endl;
    }

    return 0;
}



