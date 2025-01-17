#include <iostream>
#include <vector>
#include <string>
#include <ctime>
#include <cstdlib>

using namespace std;

static string rankToStr(int r) {
    static const string rankNames[] = {
        "", "A", "2", "3", "4", "5", "6",
        "7", "8", "9", "10", "J", "Q", "K"
    };
    if (r >= 1 && r <= 13) return rankNames[r];
    return "???";
}

struct Card {
    enum Suit { Hearts, Diamonds, Clubs, Spades };
    enum Rank {
        Ace = 1, Two, Three, Four, Five, Six, Seven,
        Eight, Nine, Ten, Jack, Queen, King
    };
    Rank rank;
    Suit suit;
    Card(Rank r, Suit s) : rank(r), suit(s) {}
    string toString() const {
        static const string suits[] = { "Hearts", "Diamonds", "Clubs", "Spades" };
        return rankToStr(rank) + " of " + suits[suit];
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
        if (cards.empty()) return Card(Card::Ace, Card::Hearts);
        Card c = cards.back();
        cards.pop_back();
        return c;
    }
    bool isEmpty() const {
        return cards.empty();
    }
};

struct Player {
    string name;
    vector<Card> hand;
    vector<Card::Rank> collectedRanks;
    Player(const string& n) : name(n) {}
    void addCard(const Card& c) {
        hand.push_back(c);
    }
    bool hasRank(Card::Rank r) const {
        for (int i = 0; i < (int)hand.size(); i++) {
            if (hand[i].rank == r) return true;
        }
        return false;
    }
    void giveCards(Card::Rank r, vector<Card>& target) {
        for (int i = 0; i < (int)hand.size();) {
            if (hand[i].rank == r) {
                target.push_back(hand[i]);
                hand.erase(hand.begin() + i);
            }
            else i++;
        }
    }
    void showHand() const {
        cout << name << "'s hand:" << endl;
        for (int i = 0; i < (int)hand.size(); i++) {
            cout << "  " << hand[i].toString() << endl;
        }
    }
    bool canDropFourOfAKind() const {
        int rankCount[14];
        for (int i = 0; i < 14; i++) rankCount[i] = 0;
        for (int i = 0; i < (int)hand.size(); i++) {
            rankCount[hand[i].rank]++;
        }
        for (int r = (int)Card::Ace; r <= (int)Card::King; r++) {
            if (rankCount[r] == 4) return true;
        }
        return false;
    }
    bool dropFourOfAKind(Card::Rank r) {
        int countCheck = 0;
        for (int i = 0; i < (int)hand.size(); i++) {
            if (hand[i].rank == r) countCheck++;
        }
        if (countCheck < 4) return false;
        int removed = 0;
        for (int i = 0; i < (int)hand.size() && removed < 4;) {
            if (hand[i].rank == r) {
                hand.erase(hand.begin() + i);
                removed++;
            }
            else i++;
        }
        collectedRanks.push_back(r);
        cout << name << " just dropped four of rank " << rankToStr(r) << "!" << endl;
        return true;
    }
    bool ownsSet(Card::Rank r) const {
        for (int i = 0; i < (int)collectedRanks.size(); i++) {
            if (collectedRanks[i] == r) return true;
        }
        return false;
    }
    void takeSet(Card::Rank r, Player& from) {
        for (int i = 0; i < (int)from.collectedRanks.size(); i++) {
            if (from.collectedRanks[i] == r) {
                collectedRanks.push_back(r);
                from.collectedRanks.erase(from.collectedRanks.begin() + i);
                break;
            }
        }
    }
};

bool all13Collected(const Player& p1, const Player& p2) {
    return (p1.collectedRanks.size() + p2.collectedRanks.size() == 13);
}

Card::Rank parseRank(const string& input) {
    if (input == "A" || input == "a") return Card::Ace;
    if (input == "J" || input == "j") return Card::Jack;
    if (input == "Q" || input == "q") return Card::Queen;
    if (input == "K" || input == "k") return Card::King;
    bool digits = true;
    for (int i = 0; i < (int)input.size(); i++) {
        if (!isdigit(input[i])) digits = false;
    }
    if (digits) {
        int val = 0;
        for (int i = 0; i < (int)input.size(); i++) {
            val = val * 10 + (input[i] - '0');
        }
        if (val >= 2 && val <= 10) return (Card::Rank)val;
    }
    return Card::Ace;
}

void waitForMindYourBusiness() {
    bool ok = false;
    while (!ok) {
        string line;
        cin >> ws;
        getline(cin, line);
        if (line == "mind your business!") ok = true;
        else cout << "Invalid response. Please type 'mind your business!': ";
    }
}

Card drawCardFromDeck(Player& p, Deck& deck, bool isUser) {
    if (deck.isEmpty()) {
        if (isUser) cout << "No cards left in the deck." << endl;
        else cout << "No cards left in the deck for the computer." << endl;
        return Card(Card::Ace, Card::Hearts);
    }
    Card c = deck.dealCard();
    p.addCard(c);
    if (isUser) cout << "You drew: " << c.toString() << endl;
    else cout << p.name << " draws a card." << endl;
    return c;
}

void dropPromptUser(Player& user) {
    bool done = false;
    while (!done) {
        if (!user.canDropFourOfAKind()) done = true;
        else {
            cout << "You have four of a kind. You must drop them before continuing." << endl;
            cout << "Enter command: drop <rank>" << endl;
            string cmd;
            cin >> cmd;
            if (cmd == "drop") {
                string rankInput;
                cin >> rankInput;
                Card::Rank rr = parseRank(rankInput);
                bool success = user.dropFourOfAKind(rr);
                if (!success) cout << "You do not have four of that rank. Try again." << endl;
            }
            else cout << "Invalid command." << endl;
        }
    }
}

enum UserAskStatus { ASK_OK, ASK_DRAW, ASK_RETRY };

UserAskStatus userAskOnce(Player& active, Player& opponent, Card::Rank& lastAsked) {
    cout << endl << "Your turn. Enter a command: ask <rank>, draw, drop <rank>" << endl;
    string cmd;
    cin >> cmd;
    if (cmd == "ask") {
        string rankInput;
        cin >> rankInput;
        Card::Rank r = parseRank(rankInput);
        if (!active.hasRank(r)) {
            cout << "You cannot ask for a rank you don't have!" << endl;
            return ASK_RETRY;
        }
        lastAsked = r;
        if (opponent.hasRank(r)) {
            cout << opponent.name << " gives you all cards of rank " << rankToStr(r) << "!" << endl;
            opponent.giveCards(r, active.hand);
            dropPromptUser(active);
            return ASK_OK;
        }
        else {
            cout << "Mind your business!" << endl;
            return ASK_DRAW;
        }
    }
    if (cmd == "draw") {
        cout << "You must specify draw before continuing: ";
        return ASK_RETRY;
    }
    if (cmd == "drop") {
        string rankInput;
        cin >> rankInput;
        Card::Rank rr = parseRank(rankInput);
        bool success = active.dropFourOfAKind(rr);
        if (!success) cout << "You do not have four of that rank." << endl;
        return ASK_RETRY;
    }
    cout << "Invalid command." << endl;
    return ASK_RETRY;
}

bool userAskAndContinue(Player& active, Player& opponent, Deck& deck) {
    Card::Rank lastAsked = Card::Ace;
    while (true) {
        UserAskStatus st = userAskOnce(active, opponent, lastAsked);
        if (st == ASK_OK) {
            cout << "Your hand after the move:" << endl;
            active.showHand();
            continue;
        }
        if (st == ASK_DRAW) {
            if (deck.isEmpty()) {
                cout << "No cards left in the deck." << endl;
                cout << "Your hand after the move:" << endl;
                active.showHand();
                return false;
            }
            cout << "Enter 'draw' to draw a card: ";
            bool drawn = false;
            while (!drawn) {
                string cmd;
                cin >> cmd;
                if (cmd == "draw") {
                    Card c = drawCardFromDeck(active, deck, true);
                    if (c.rank == lastAsked) {
                        cout << "Lucky draw! You drew the rank you asked for, so you ask again." << endl;
                        dropPromptUser(active);
                        drawn = true;
                    }
                    else {
                        dropPromptUser(active);
                        cout << "Your hand after the move:" << endl;
                        active.showHand();
                        return false;
                    }
                }
                else {
                    cout << "Invalid command, you must draw a card." << endl;
                }
            }
            continue;
        }
        if (st == ASK_RETRY) continue;
    }
}

bool userTurnPhaseOne(Player& user, Player& comp, Deck& deck) {
    dropPromptUser(user);
    bool cont = userAskAndContinue(user, comp, deck);
    return cont;
}

bool computerAskGiveCards(Player& comp, Player& user, Card::Rank rank) {
    cout << comp.name << " asks for rank " << rankToStr(rank) << "!" << endl;
    if (user.hasRank(rank)) {
        cout << "User, please enter command to give these cards: ";
        while (true) {
            string cmd;
            cin >> cmd;
            if (cmd == "give") {
                string rInput;
                cin >> rInput;
                Card::Rank rParsed = parseRank(rInput);
                if (rParsed == rank) {
                    user.giveCards(rank, comp.hand);
                    cout << "Cards given." << endl;
                    return true;
                }
                else {
                    cout << "Wrong rank to give. Type 'give <rank>' again." << endl;
                }
            }
            else {
                cout << "Invalid command. You must type 'give <rank>'." << endl;
            }
        }
    }
    else {
        cout << "User does not have that rank. Please type 'mind your business!': ";
        waitForMindYourBusiness();
        return false;
    }
}

bool computerContinueAsking(Player& comp, Player& user, Deck& deck) {
    Card::Rank lastRank = (Card::Rank)0;
    while (true) {
        if (comp.hand.empty()) {
            cout << comp.name << " has no cards to ask for." << endl;
            return true;
        }
        Card::Rank candidate = comp.hand[rand() % comp.hand.size()].rank;
        int tries = 0;
        while (candidate == lastRank && comp.hand.size() > 1 && tries < 20) {
            candidate = comp.hand[rand() % comp.hand.size()].rank;
            tries++;
        }
        bool success = computerAskGiveCards(comp, user, candidate);
        if (!success) {
            if (deck.isEmpty()) {
                cout << "No cards left in the deck for the computer." << endl;
                return false;
            }
            Card c = drawCardFromDeck(comp, deck, false);
            if (c.rank == candidate) {
                cout << "Computer drew the same rank. Continues turn." << endl;
                lastRank = candidate;
            }
            else {
                return false;
            }
        }
        else {
            lastRank = candidate;
        }
    }
}

void processTurnPhaseOne(Player& active, Player& opponent, Deck& deck, bool isUser) {
    if (isUser) {
        bool cont = userTurnPhaseOne(active, opponent, deck);
        (void)cont;
    }
    else {
        bool cont = computerContinueAsking(active, opponent, deck);
        (void)cont;
    }
}

void processSecondPhaseUser(Player& user, Player& comp) {
    cout << "Enter: askset <rank>" << endl;
    string cmd;
    cin >> cmd;
    if (cmd == "askset") {
        string rankInput;
        cin >> rankInput;
        Card::Rank r = parseRank(rankInput);
        if (comp.ownsSet(r)) {
            cout << "You received the entire set of rank " << rankToStr(r) << "!" << endl;
            user.takeSet(r, comp);
        }
        else {
            cout << "Mind your business!" << endl;
        }
    }
    else {
        cout << "Invalid command." << endl;
    }
}

void processSecondPhaseComputer(Player& comp, Player& user) {
    Card::Rank rr = (Card::Rank)(1 + rand() % 13);
    cout << comp.name << " asks for the entire set of rank " << rankToStr(rr) << "!" << endl;
    if (user.ownsSet(rr)) {
        cout << comp.name << " takes that set from you." << endl;
        comp.takeSet(rr, user);
    }
    else {
        cout << "Please type 'mind your business!': ";
        waitForMindYourBusiness();
    }
}

void runGame(Deck& deck, Player& user, Player& computer) {
    bool userTurn = true;
    bool secondPhase = false;
    while (true) {
        if (!secondPhase) {
            if (deck.isEmpty() && user.hand.empty() && computer.hand.empty()) break;
            processTurnPhaseOne(userTurn ? user : computer,
                userTurn ? computer : user,
                deck,
                userTurn);
            if (all13Collected(user, computer)) {
                secondPhase = true;
                cout << "All 13 sets are now distributed. The second phase of the game begins!" << endl;
            }
        }
        else {
            if (user.collectedRanks.size() == 13 || computer.collectedRanks.size() == 13) break;
            if (userTurn) processSecondPhaseUser(user, computer);
            else processSecondPhaseComputer(computer, user);
        }
        userTurn = !userTurn;
        if (user.collectedRanks.size() == 13 || computer.collectedRanks.size() == 13) break;
    }
    cout << endl << "Game over!" << endl;
    cout << "User has collected " << user.collectedRanks.size() << " sets." << endl;
    cout << "Computer has collected " << computer.collectedRanks.size() << " sets." << endl;
    if (user.collectedRanks.size() == 13) cout << "User wins by collecting all sets!" << endl;
    else if (computer.collectedRanks.size() == 13) cout << "Computer wins by collecting all sets!" << endl;
    else cout << "No one collected all 13 sets." << endl;
}

int main() {
    Deck deck;
    Player user("User"), computer("Computer");
    cout << "Command: deal" << endl;
    deck.shuffle();
    for (int i = 0; i < 6; i++) {
        user.addCard(deck.dealCard());
        computer.addCard(deck.dealCard());
    }
    cout << "Initial hands:" << endl;
    user.showHand();
    runGame(deck, user, computer);
    return 0;
}










