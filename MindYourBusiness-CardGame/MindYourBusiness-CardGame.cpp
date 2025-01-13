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
        const string ranks[] = { "", "Ace", "Two", "Three", "Four", "Five", "Six",
      "Seven", "Eight", "Nine", "Ten", "Jack", "Queen", "King" };
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
            swap(cards[i], cards[j]);
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

struct Player {
    string name;
    vector<Card> hand;

    Player(const string& name) : name(name) {}

    void addCard(const Card& card) {
        hand.push_back(card);
    }

    bool hasRank(Card::Rank rank) const {
        for (const auto& card : hand) {
            if (card.rank == rank) {
                return true;
            }
        }
        return false;
    }

    void giveCards(Card::Rank rank, vector<Card>& target) {
        for (auto it = hand.begin(); it != hand.end();) {
            if (it->rank == rank) {
                target.push_back(*it);
                it = hand.erase(it);
            }
            else {
                ++it;
            }
        }
    }

    void showHand() const {
        cout << name << "'s hand:" << endl;
        for (const auto& card : hand) {
            cout << "  " << card.toString() << endl;
        }
    }
};

Card::Rank parseRank(const string& input) {
    if (input == "A" || input == "a") return Card::Ace;
    if (input == "J" || input == "j") return Card::Jack;
    if (input == "Q" || input == "q") return Card::Queen;
    if (input == "K" || input == "k") return Card::King;
    int rank = stoi(input);
    if (rank >= 2 && rank <= 10) return static_cast<Card::Rank>(rank);
    throw invalid_argument("Invalid rank input");
}

void drawCard(Player& player, Deck& deck) {
    if (!deck.isEmpty()) {
        player.addCard(deck.dealCard());
        cout << "Command: draw\n";
    }
    else {
        cout << "No cards left in the deck to draw.\n";
    }
}

void initializeGame(Deck& deck, Player& user, Player& computer) {
    cout << "Command: deal\n";
    deck.shuffle();

    for (int i = 0; i < 6; ++i) {
        user.addCard(deck.dealCard());
        computer.addCard(deck.dealCard());
    }

    cout << "Initial hands:\n";
    user.showHand();
}

void processTurn(Player& active, Player& opponent, Deck& deck, bool isUser) {
    if (isUser) {
        cout << "\nYour turn. Enter a command (e.g., ask <rank>, draw): ";
        string command;
        cin >> command;

        if (command == "ask") {
            string rankInput;
            cin >> rankInput;

            try {
                Card::Rank rank = parseRank(rankInput);

                if (opponent.hasRank(rank)) {
                    cout << opponent.name << " gives you all cards of rank " << rankInput << "!\n";
                    opponent.giveCards(rank, active.hand);
                }
                else {
                    cout << opponent.name << " has no cards of that rank. Drawing a card from the deck.\n";
                    drawCard(active, deck);
                }
            }
            catch (const invalid_argument&) {
                cout << "Invalid rank input. Try again.\n";
            }
        }
        else if (command == "draw") {
            drawCard(active, deck);
        }
        else {
            cout << "Invalid command. Try again.\n";
        }
    }
    else {
        cout << "\n" << active.name << "'s turn.\n";
        if (!active.hand.empty()) {
            Card::Rank rank = active.hand[rand() % active.hand.size()].rank;
            cout << active.name << " asks for " << rank << "s!\n";

            if (opponent.hasRank(rank)) {
                cout << "give " << rank << "s\n";
                opponent.giveCards(rank, active.hand);
            }
            else {
                cout << opponent.name << " has no cards of that rank. Drawing a card from the deck.\n";
                drawCard(active, deck);
            }
        }
    }
}

int main() {
    Deck deck;
    Player user("User");
    Player computer("Computer");

    initializeGame(deck, user, computer);

    bool userTurnFlag = true;

    while (!deck.isEmpty()) {
        processTurn(userTurnFlag ? user : computer, userTurnFlag ? computer : user, deck, userTurnFlag);
        userTurnFlag = !userTurnFlag;

        cout << "\nUpdated hands:\n";
        user.showHand();
    }

    cout << "\nGame over!\n";
    return 0;
}




