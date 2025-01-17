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
    Card(Rank r, Suit s) : rank(r), suit(s) {}
    string toString() const {
        static const string ranks[] = {
            "", "Ace", "Two", "Three", "Four", "Five", "Six",
            "Seven", "Eight", "Nine", "Ten", "Jack", "Queen", "King"
        };
        static const string suits[] = { "Hearts", "Diamonds", "Clubs", "Spades" };
        return ranks[rank] + " of " + suits[suit];
    }
};

struct Deck {
    vector<Card> cards;
    Deck() {
        for (int s = 0; s < 4; s++) {
            for (int r = 1; r <= 13; r++) {
                cards.push_back(Card((Card::Rank)r, (Card::Suit)s));
            }
        }
    }
    void shuffle() {
        srand((unsigned)time(nullptr));
        for (int i = (int)cards.size() - 1; i > 0; i--) {
            int j = rand() % (i + 1);
            Card tmp = cards[i];
            cards[i] = cards[j];
            cards[j] = tmp;
        }
    }
    Card dealCard() {
        if (cards.empty()) {
            throw out_of_range("No cards left in the deck");
        }
        Card c = cards.back();
        cards.pop_back();
        return c;
    }
    bool isEmpty() const {
        return cards.empty();
    }
};

void waitForMindYourBusiness() {
    bool ok = false;
    while (!ok) {
        string line;
        cin >> ws;
        getline(cin, line);
        if (line == "mind your business!") {
            ok = true;
        }
        else {
            cout << "Invalid response. Please type 'mind your business!': ";
        }
    }
}

struct Player {
    string name;
    vector<Card> hand;
    vector<Card::Rank> collectedRanks;

    Player(const string& n) : name(n) {}
    void addCard(const Card& c) {
        hand.push_back(c);
    }
    bool hasRank(Card::Rank r) const {
        for (auto& card : hand) {
            if (card.rank == r) return true;
        }
        return false;
    }
    void giveCards(Card::Rank r, vector<Card>& target) {
        for (int i = 0; i < (int)hand.size();) {
            if (hand[i].rank == r) {
                target.push_back(hand[i]);
                hand.erase(hand.begin() + i);
            }
            else {
                i++;
            }
        }
    }
    bool canDropFourOfAKind() const {
        int rankCount[14];
        for (int i = 0; i < 14; i++) rankCount[i] = 0;
        for (auto& c : hand) {
            rankCount[c.rank]++;
        }
        for (int r = (int)Card::Ace; r <= (int)Card::King; r++) {
            if (rankCount[r] == 4) return true;
        }
        return false;
    }
    bool dropFourOfAKind(Card::Rank r) {
        int count = 0;
        for (auto& c : hand) {
            if (c.rank == r) count++;
        }
        if (count < 4) return false;
        int removed = 0;
        for (int i = 0; i < (int)hand.size() && removed < 4;) {
            if (hand[i].rank == r) {
                hand.erase(hand.begin() + i);
                removed++;
            }
            else {
                i++;
            }
        }
        collectedRanks.push_back(r); 
        cout << name << " dropped four of rank " << r << "!\n";
        return true;
    }

    void showHand() const {
        cout << name << "'s hand:" << endl;
        for (auto& c : hand) {
            cout << "  " << c.toString() << endl;
        }
    }
};

void drawCard(Player& player, Deck& deck) {
    if (!deck.isEmpty()) {
        Card c = deck.dealCard();
        player.addCard(c);
        cout << "You drew: " << c.toString() << endl;
    }
    else {
        cout << "No cards left in the deck." << endl;
    }
}

Card::Rank parseRank(const string& input) {
    if (input == "A" || input == "a") return Card::Ace;
    if (input == "J" || input == "j") return Card::Jack;
    if (input == "Q" || input == "q") return Card::Queen;
    if (input == "K" || input == "k") return Card::King;
    int val = stoi(input);
    if (val >= 2 && val <= 10) {
        return (Card::Rank)val;
    }
    throw invalid_argument("Invalid rank input");
}

void initializeGame(Deck& deck, Player& user, Player& computer) {
    cout << "Command: deal" << endl;
    deck.shuffle();
    for (int i = 0; i < 6; i++) {
        user.addCard(deck.dealCard());
        computer.addCard(deck.dealCard());
    }
    cout << "Initial hands:" << endl;
    user.showHand();
}

void processUserTurn(Player& user, Player& comp, Deck& deck) {
    cout << "\nYour turn. Enter a command: ask <rank>, draw" << endl;
    string cmd;
    cin >> cmd;
    if (cmd == "ask") {
        string rankInput;
        cin >> rankInput;
        try {
            Card::Rank r = parseRank(rankInput);
            if (comp.hasRank(r)) {
                cout << comp.name << " gives you all cards of rank " << rankInput << "!" << endl;
                comp.giveCards(r, user.hand);
            }
            else {
                cout << "Mind your business!" << endl;
                cout << "Enter 'draw' to draw a card: ";
                bool done = false;
                while (!done) {
                    string sub;
                    cin >> sub;
                    if (sub == "draw") {
                        drawCard(user, deck);
                        done = true;
                    }
                    else {
                        cout << "Invalid command, type 'draw': ";
                    }
                }
            }
        }
        catch (...) {
            cout << "Invalid rank input." << endl;
        }
    }
    else if (cmd == "draw") {
        drawCard(user, deck);
    }
    else {
        cout << "Invalid command." << endl;
    }
    cout << "\nYour updated hand:" << endl;
    user.showHand();
}

void processComputerTurn(Player& comp, Player& user, Deck& deck) {
    cout << "\n" << comp.name << "'s turn." << endl;
    if (!comp.hand.empty()) {
        Card::Rank r = comp.hand[rand() % comp.hand.size()].rank;
        cout << comp.name << " asks for rank " << r << "!" << endl;
        if (user.hasRank(r)) {
            cout << "User, please type 'mind your business!' or 'give <rank>': ";
            while (true) {
                string tmp;
                cin >> ws;
                getline(cin, tmp);
                if (tmp == "mind your business!") {
                    cout << "Ok, the computer draws a card." << endl;
                    if (!deck.isEmpty()) {
                        Card c = deck.dealCard();
                        comp.addCard(c);
                        cout << comp.name << " draws: " << c.toString() << endl;
                    }
                    else {
                        cout << "No cards left in the deck for the computer." << endl;
                    }
                    break;
                }
                else {
                    if (tmp.rfind("give", 0) == 0) {
                        string rnk = tmp.substr(5);
                        try {
                            Card::Rank rr = parseRank(rnk);
                            if (rr == r) {
                                user.giveCards(rr, comp.hand);
                                cout << "Cards given to computer." << endl;
                            }
                            else {
                                cout << "Wrong rank given. Try again or 'mind your business!'." << endl;
                                continue;
                            }
                        }
                        catch (...) {
                            cout << "Invalid rank to give. Try again or 'mind your business!'." << endl;
                            continue;
                        }
                        break;
                    }
                    else {
                        cout << "Invalid command. Type 'give <rank>' or 'mind your business!': ";
                    }
                }
            }
        }
        else {
            cout << "User does not have that rank. Please type 'mind your business!': ";
            waitForMindYourBusiness();
            cout << comp.name << " draws a card." << endl;
            if (!deck.isEmpty()) {
                Card c = deck.dealCard();
                comp.addCard(c);
                cout << "Computer drew: " << c.toString() << endl;
            }
            else {
                cout << "No cards left in the deck for the computer." << endl;
            }
        }
    }
}

int main() {
    srand((unsigned)time(nullptr));
    Deck deck;
    Player user("User"), computer("Computer");
    initializeGame(deck, user, computer);
    bool userTurnFlag = true;
    while (!deck.isEmpty()) {
        if (userTurnFlag) {
            processUserTurn(user, computer, deck);
        }
        else {
            processComputerTurn(computer, user, deck);
        }
        userTurnFlag = !userTurnFlag;
    }
    cout << "\nGame over!" << endl;
    return 0;
}










