#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <algorithm>
#include <vector>
#include <set>
#include <stack>
#include <cstring>
#include <cstdio>
#include <ctime>
using namespace std;

const char* FILENAME = "D:\\DEVcode\\movie2.txt";
const char  ROWS[]   = {'A','B','C','D','E'};
const int   COLS     = 10;

// ==================== STRUCTS ====================

struct Seat {
    char row;
    int  col;
    bool booked;
};

struct MovieNode {
    int       id;
    char      movie[50];
    float     price;
    Seat      seats[5][10];   // seats[row 0-4][col 0-9]
    MovieNode* next;
};

// ==================== GLOBALS ====================
MovieNode* head = NULL;
stack<MovieNode*> undoStack;

// ==================== SEAT HELPERS ====================

void initSeats(MovieNode* node) {
    for (int r = 0; r < 5; r++)
        for (int c = 0; c < COLS; c++) {
            node->seats[r][c].row    = ROWS[r];
            node->seats[r][c].col    = c + 1;
            node->seats[r][c].booked = false;
        }
}

int countAvailable(MovieNode* node) {
    int count = 0;
    for (int r = 0; r < 5; r++)
        for (int c = 0; c < COLS; c++)
            if (!node->seats[r][c].booked) count++;
    return count;
}

// Convert "A5" -> row index 0, col index 4  (returns false if invalid)
bool parseSeat(const string& s, int& row, int& col) {
    if (s.size() < 2) return false;
    char rowCh = toupper(s[0]);
    int  colNum = 0;
    for (int i = 1; i < (int)s.size(); i++) {
        if (!isdigit(s[i])) return false;
        colNum = colNum * 10 + (s[i] - '0');
    }
    for (int r = 0; r < 5; r++)
        if (ROWS[r] == rowCh) { row = r; col = colNum - 1; return col >= 0 && col < COLS; }
    return false;
}

// ==================== LINKED LIST HELPERS ====================

MovieNode* createNode(int id, const char* title, float price) {
    MovieNode* node = new MovieNode();
    node->id = id;
    strncpy(node->movie, title, 49);
    node->movie[49] = '\0';
    node->price = price;
    node->next  = NULL;
    initSeats(node);
    return node;
}

void appendNodeRaw(MovieNode*& listHead, MovieNode* n) {
    if (!listHead) { listHead = n; return; }
    MovieNode* cur = listHead;
    while (cur->next) cur = cur->next;
    cur->next = n;
}

MovieNode* copyList(MovieNode* src) {
    MovieNode* newHead = NULL;
    for (MovieNode* cur = src; cur != NULL; cur = cur->next) {
        MovieNode* node = new MovieNode(*cur);
        node->next = NULL;
        appendNodeRaw(newHead, node);
    }
    return newHead;
}

void freeList(MovieNode*& listHead) {
    while (listHead) { MovieNode* t = listHead; listHead = listHead->next; delete t; }
}

int listSize(MovieNode* listHead) {
    int n = 0; for (MovieNode* c = listHead; c; c = c->next) n++; return n;
}

MovieNode* findById(MovieNode* listHead, int id) {
    for (MovieNode* c = listHead; c; c = c->next) if (c->id == id) return c;
    return NULL;
}

// ==================== DISPLAY ====================

void printSeatMap(MovieNode* node) {
    cout << "\n";
    cout << "  +============== SEAT MAP: " << node->movie << " ==============+\n";
    cout << "  |                                                   |\n";
    cout << "  |          +-------- [ SCREEN ] --------+          |\n";
    cout << "  |                                                   |\n";
    cout << "  |       ";
    for (int c = 1; c <= COLS; c++) cout << setw(3) << c;
    cout << "     |\n";
    cout << "  |     " << string(32, '-') << "     |\n";
    for (int r = 0; r < 5; r++) {
        cout << "  |  " << ROWS[r] << "  |";
        for (int c = 0; c < COLS; c++) {
            if (node->seats[r][c].booked)
                cout << "  X";
            else
                cout << "  O";
        }
        cout << "  |\n";
    }
    cout << "  |     " << string(32, '-') << "     |\n";
    cout << "  |    Legend:  [ O ] = Available    [ X ] = Booked  |\n";
    cout << "  +===================================================+\n\n";
}

void displayMovies() {
    cout << "\n";
    cout << "  +============================================+\n";
    cout << "  |              NOW SHOWING                  |\n";
    cout << "  +============================================+\n";
    cout << "  | " << left << setw(4) << "ID"
                   << setw(22) << "Movie"
                   << setw(14) << "Seats Avail"
                   << "Price     |\n";
    cout << "  +--------------------------------------------+\n";
    for (MovieNode* cur = head; cur; cur = cur->next) {
        cout << "  | " << left << setw(4)  << cur->id
                       << setw(22) << cur->movie
                       << setw(14) << countAvailable(cur)
             << "$" << fixed << setprecision(2) << cur->price
             << "     |\n";
    }
    cout << "  +============================================+\n\n";
}

void displayOne(MovieNode* t) {
    // Build seats string first so setw pads the whole thing
    string seatsStr = to_string(countAvailable(t)) + " / 50 available";

    cout << "\n";
    cout << "  +----------------------------------+\n";
    cout << "  |          MOVIE DETAILS           |\n";
    cout << "  +----------------------------------+\n";
    cout << "  | ID    : " << left << setw(24) << t->id     << "|\n";
    cout << "  | Movie : " << left << setw(24) << t->movie  << "|\n";
    cout << "  | Seats : " << left << setw(24) << seatsStr  << "|\n";
    cout << "  | Price : $" << fixed << setprecision(2) << left << setw(23) << t->price << "|\n";
    cout << "  +----------------------------------+\n\n";
}

// ==================== RECEIPT ====================

void printReceipt(MovieNode* node,
                  const vector<pair<int,int>>& pickedSeats,
                  const string& customerName) {
    time_t now = time(0);
    char timeBuf[32];
    strftime(timeBuf, sizeof(timeBuf), "%Y-%m-%d %H:%M:%S", localtime(&now));

    float total = pickedSeats.size() * node->price;

    cout << "\n";
    cout << "  +==========================================+\n";
    cout << "  |          MOVIE TICKET RECEIPT            |\n";
    cout << "  +==========================================+\n";
    cout << "  | Date     : " << left << setw(30) << timeBuf      << "|\n";
    cout << "  | Customer : " << left << setw(30) << customerName  << "|\n";
    cout << "  +------------------------------------------+\n";
    cout << "  | Movie    : " << left << setw(30) << node->movie   << "|\n";
    cout << "  | Price    : $" << fixed << setprecision(2)
         << left << setw(29) << node->price << "|\n";
    cout << "  +------------------------------------------+\n";
    cout << "  | Seats Booked:                            |\n";

    // Print seat labels in groups of 5 per line for readability
    string seatLine = "";
    for (int i = 0; i < (int)pickedSeats.size(); i++) {
        string label = "";
        label += ROWS[pickedSeats[i].first];
        label += to_string(pickedSeats[i].second + 1);
        seatLine += label;
        if (i < (int)pickedSeats.size() - 1) seatLine += "  ";
        // Flush every 5 seats onto a new line
        if ((i + 1) % 5 == 0 || i == (int)pickedSeats.size() - 1) {
            cout << "  |   " << left << setw(39) << seatLine << "|\n";
            seatLine = "";
        }
    }

    cout << "  +------------------------------------------+\n";
    cout << "  | Quantity : " << left << setw(30) << pickedSeats.size() << "|\n";
    cout << "  | TOTAL    : $" << fixed << setprecision(2)
         << left << setw(29) << total << "|\n";
    cout << "  +==========================================+\n";
    cout << "  |       Thank you for your purchase!       |\n";
    cout << "  +==========================================+\n\n";
}

// ==================== FILE FUNCTIONS ====================

void loadMovies() {
    ifstream file(FILENAME);
    if (!file.is_open()) { cout << "  [Info] No save file found. Starting fresh.\n"; return; }
    freeList(head);
    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;
        int fc = line.find(',');
        int sc = line.find(',', fc + 1);
        int tc = line.find(',', sc + 1);
        if (fc == (int)string::npos || sc == (int)string::npos) continue;

        int    id    = stoi(line.substr(0, fc));
        string title = line.substr(fc + 1, sc - fc - 1);
        float  price;
        string bookedStr = "";
        if (tc == (int)string::npos) {
            price = stof(line.substr(sc + 1));
        } else {
            price     = stof(line.substr(sc + 1, tc - sc - 1));
            bookedStr = line.substr(tc + 1);
        }

        char buf[50]; strncpy(buf, title.c_str(), 49); buf[49] = '\0';
        MovieNode* node = createNode(id, buf, price);

        // Parse booked seats
        string tok;
        for (char ch : bookedStr) {
            if (ch == ' ' || ch == '\r') {
                if (!tok.empty()) {
                    int r, c;
                    if (parseSeat(tok, r, c)) node->seats[r][c].booked = true;
                    tok.clear();
                }
            } else tok += ch;
        }
        if (!tok.empty()) { int r,c; if (parseSeat(tok,r,c)) node->seats[r][c].booked = true; }

        appendNodeRaw(head, node);
    }
    file.close();
}

void saveMovies() {
    ofstream file(FILENAME);
    if (!file.is_open()) { cout << "  Error: Could not save file.\n"; return; }
    for (MovieNode* cur = head; cur; cur = cur->next) {
        file << cur->id << "," << cur->movie << ","
             << fixed << setprecision(2) << cur->price << ",";
        bool first = true;
        for (int r = 0; r < 5; r++)
            for (int c = 0; c < COLS; c++)
                if (cur->seats[r][c].booked) {
                    if (!first) file << " ";
                    file << ROWS[r] << (c + 1);
                    first = false;
                }
        file << "\n";
    }
    file.close();
}

// ==================== SEQUENTIAL SEARCH (RECURSIVE) ====================
MovieNode* searchRecursive(MovieNode* cur, int targetId) {
    if (!cur) return NULL;
    if (cur->id == targetId) return cur;
    return searchRecursive(cur->next, targetId);
}

// ==================== BINARY SEARCH ====================
MovieNode* binarySearchById(int targetId) {
    int n = listSize(head);
    if (!n) return NULL;
    MovieNode** arr = new MovieNode*[n];
    int i = 0;
    for (MovieNode* cur = head; cur; cur = cur->next) arr[i++] = cur;
    sort(arr, arr + n, [](MovieNode* a, MovieNode* b){ return a->id < b->id; });
    int low = 0, high = n - 1; MovieNode* result = NULL;
    while (low <= high) {
        int mid = low + (high - low) / 2;
        if (arr[mid]->id == targetId) { result = arr[mid]; break; }
        else if (arr[mid]->id < targetId) low = mid + 1;
        else high = mid - 1;
    }
    delete[] arr; return result;
}

// ==================== STACK UNDO ====================
void saveUndo() { undoStack.push(copyList(head)); }

void undoLast() {
    if (undoStack.empty()) { cout << "  Nothing to undo.\n"; return; }
    freeList(head);
    head = undoStack.top(); undoStack.pop();
    saveMovies();
    cout << "  Undo successful!\n";
    displayMovies();
}

// ==================== SORT ALGORITHMS (BY PRICE) ====================
void sortList(int algorithm) {
    int n = listSize(head); if (n < 2) return;
    MovieNode** arr = new MovieNode*[n];
    int i = 0; for (MovieNode* cur = head; cur; cur = cur->next) arr[i++] = cur;

    auto part = [&](int lo, int hi) -> int {
        float pv = arr[hi]->price; int idx = lo - 1;
        for (int j = lo; j < hi; j++) if (arr[j]->price <= pv) swap(arr[++idx], arr[j]);
        swap(arr[idx+1], arr[hi]); return idx+1;
    };
    auto qs = [&](int lo, int hi, auto& self) -> void {
        if (lo >= hi) return; int pi = part(lo,hi); self(lo,pi-1,self); self(pi+1,hi,self);
    };

    switch (algorithm) {
        case 1: qs(0,n-1,qs); cout << "  Sorted using Quick Sort.\n"; break;
        case 2:
            for (int j=1;j<n;j++){MovieNode* k=arr[j];int m=j-1;
                while(m>=0&&arr[m]->price>k->price){arr[m+1]=arr[m];m--;}arr[m+1]=k;}
            cout << "  Sorted using Insertion Sort.\n"; break;
        case 3:
            for (int j=0;j<n-1;j++){int mi=j;
                for(int k=j+1;k<n;k++) if(arr[k]->price<arr[mi]->price) mi=k;
                if(mi!=j) swap(arr[j],arr[mi]);}
            cout << "  Sorted using Selection Sort.\n"; break;
        case 4: {
            auto hf=[&](int sz,int root,auto& self)->void{
                int lg=root,l=2*root+1,r=2*root+2;
                if(l<sz&&arr[l]->price>arr[lg]->price) lg=l;
                if(r<sz&&arr[r]->price>arr[lg]->price) lg=r;
                if(lg!=root){swap(arr[root],arr[lg]);self(sz,lg,self);}};
            for(int j=n/2-1;j>=0;j--) hf(n,j,hf);
            for(int j=n-1;j>0;j--){swap(arr[0],arr[j]);hf(j,0,hf);}
            cout << "  Sorted using Heap Sort.\n"; break;
        }
        case 5:
            for(int j=0;j<n-1;j++){bool sw=false;
                for(int k=0;k<n-1-j;k++) if(arr[k]->price>arr[k+1]->price){swap(arr[k],arr[k+1]);sw=true;}
                if(!sw) break;}
            cout << "  Sorted using Bubble Sort.\n"; break;
        default: cout << "  Invalid choice!\n"; delete[] arr; return;
    }
    head = arr[0];
    for (int j=0;j<n-1;j++) arr[j]->next=arr[j+1];
    arr[n-1]->next=NULL; delete[] arr;
}

void sortMoviesMenu() {
    int sub;
    cout << "\n  --- Sort by Price ---\n"
         << "  1. Quick Sort\n  2. Insertion Sort\n  3. Selection Sort\n"
         << "  4. Heap Sort\n  5. Bubble Sort\n  Choice: ";
    cin >> sub;
    sortList(sub); saveMovies();
    cout << "\n  After sorting:\n";
    displayMovies();
}

// ==================== BOOK TICKET ====================
//
// Flow:
//   1. Show movie list
//   2. Choose movie ID -> validate
//   3. Show movie info (no seat map yet)
//   4. Ask how many tickets
//   5. Ask customer name
//   6. Show seat map
//   7. Pick seats one by one (map refreshes after each pick)
//   8. Confirm -> save undo snapshot BEFORE marking, then mark & save
//   9. Print receipt
//
void bookTicket() {
    // Step 1: already shown before menu; just ask for ID
    int id;
    cout << "  Enter movie ID: ";
    cin >> id;

    MovieNode* node = searchRecursive(head, id);
    if (!node) { cout << "  Movie not found.\n"; return; }

    int avail = countAvailable(node);
    if (avail == 0) { cout << "  Sorry, no seats are available for this movie.\n"; return; }

    // Step 3: show movie info only (no seat map yet)
    displayOne(node);

    // Step 4: ask how many tickets
    int qty;
    cout << "  How many tickets? (1-" << avail << "): ";
    cin >> qty;
    if (qty <= 0 || qty > avail) {
        cout << "  Invalid number of tickets.\n";
        return;
    }

    // Step 5: ask customer name
    string customerName;
    cout << "  Enter customer name: ";
    cin.ignore();
    getline(cin, customerName);
    if (customerName.empty()) customerName = "Guest";

    // Step 6: NOW show the seat map
    cout << "\n  Please choose " << qty << " seat(s) from the map below:\n";
    printSeatMap(node);

    // Step 7: pick seats
    vector<pair<int,int>> chosen;
    for (int i = 0; i < qty; ) {
        cout << "  Seat " << (i + 1) << " of " << qty
             << " (e.g. A1, B5, C10): ";
        string s; cin >> s;

        int r, c;
        if (!parseSeat(s, r, c)) {
            cout << "  Invalid format. Use a row letter (A-E) + column number (1-10). Try again.\n";
            continue;
        }
        if (node->seats[r][c].booked) {
            cout << "  Seat " << ROWS[r] << (c + 1) << " is already booked. Choose another.\n";
            continue;
        }
        // Prevent duplicate picks in this session
        bool dup = false;
        for (auto& p : chosen)
            if (p.first == r && p.second == c) { dup = true; break; }
        if (dup) {
            cout << "  You already selected " << ROWS[r] << (c + 1) << ". Choose a different seat.\n";
            continue;
        }

        chosen.push_back({r, c});
        // Temporarily mark so the map shows it as picked
        node->seats[r][c].booked = true;
        cout << "  >> Seat " << ROWS[r] << (c + 1) << " selected.\n";
        i++;

        // Refresh map after each pick (only if more seats remain)
        if (i < qty) {
            cout << "\n  Updated seat map (" << (qty - i) << " seat(s) left to pick):\n";
            printSeatMap(node);
        }
    }

    // Step 8: Confirm
    cout << "\n  You selected: ";
    for (int i = 0; i < (int)chosen.size(); i++) {
        cout << ROWS[chosen[i].first] << (chosen[i].second + 1);
        if (i < (int)chosen.size() - 1) cout << ", ";
    }
    cout << "\n  Total: $" << fixed << setprecision(2) << chosen.size() * node->price << "\n";
    cout << "\n  Confirm booking? (y/n): ";
    char confirm; cin >> confirm;

    if (confirm != 'y' && confirm != 'Y') {
        // Roll back the temporary marks
        for (auto& p : chosen) node->seats[p.first][p.second].booked = false;
        cout << "  Booking cancelled. Seats released.\n";
        return;
    }

    // Seats are already marked; save undo of the *pre-booking* state is tricky here
    // because we marked seats during picking. We handle it by saving undo BEFORE
    // we persist to file — the in-memory undo snapshot already has the booked state,
    // which is correct for undo (undoing brings back the pre-booking state that was
    // pushed before the flow started). Since we couldn't save before picking without
    // disrupting the UX, we push a copy-before-this-booking snapshot instead:
    // Re-create what head looked like before booking by temporarily unmarking, snapshot, re-mark.
    for (auto& p : chosen) node->seats[p.first][p.second].booked = false;
    saveUndo();   // snapshot of pre-booking state
    for (auto& p : chosen) node->seats[p.first][p.second].booked = true;

    saveMovies();

    // Step 9: Print receipt
    printReceipt(node, chosen, customerName);
}

// ==================== SEARCH MENU ====================
void searchMovie() {
    int sub, id;
    cout << "\n  --- Search Options ---\n"
         << "  1. Sequential Search (Recursive)\n"
         << "  2. Binary Search\n  Choice: ";
    cin >> sub;
    cout << "  Enter movie ID: "; cin >> id;
    MovieNode* result = (sub == 1) ? searchRecursive(head, id) : binarySearchById(id);
    if (result) {
        displayOne(result);
        char showMap;
        cout << "  View seat map for this movie? (y/n): ";
        cin >> showMap;
        if (showMap == 'y' || showMap == 'Y') printSeatMap(result);
    } else {
        cout << "  Movie not found.\n";
    }
}

// ==================== EDIT ====================
void editMovie() {
    int id; cout << "  Enter movie ID to edit: "; cin >> id;
    MovieNode* node = findById(head, id);
    if (!node) { cout << "  Movie not found.\n"; return; }

    cout << "  Enter new price (current: $" << fixed << setprecision(2) << node->price << "): ";
    cin >> node->price;

    char reset;
    cout << "  Reset all seats to available? (y/n): "; cin >> reset;
    if (reset == 'y' || reset == 'Y') { initSeats(node); cout << "  All seats reset.\n"; }

    cout << "  Movie updated.\n"; saveMovies();
}

// ==================== DELETE ====================
void deleteMovie() {
    int id; cout << "  Enter movie ID to delete: "; cin >> id;
    if (!head) { cout << "  List is empty.\n"; return; }
    MovieNode* prev = NULL; MovieNode* cur = head;
    while (cur && cur->id != id) { prev = cur; cur = cur->next; }
    if (!cur) { cout << "  Movie not found.\n"; return; }
    cout << "\n  Movie found:\n"; displayOne(cur);
    char confirm; cout << "  Are you sure you want to delete? (y/n): "; cin >> confirm;
    if (confirm != 'y' && confirm != 'Y') { cout << "  Delete cancelled.\n"; return; }
    if (!prev) head = cur->next; else prev->next = cur->next;
    delete cur; cout << "  Movie deleted.\n"; saveMovies();
}

// ==================== CLEANUP ====================
void freeUndoStack() {
    while (!undoStack.empty()) {
        MovieNode* s = undoStack.top(); undoStack.pop(); freeList(s);
    }
}

// ==================== MAIN ====================
int main() {
    int choice;
    loadMovies();
    displayMovies();

    do {
        cout << "  +---------------------------+\n";
        cout << "  |         MAIN MENU         |\n";
        cout << "  +---------------------------+\n";
        cout << "  | 1. Book Ticket            |\n";
        cout << "  | 2. Search Movie           |\n";
        cout << "  | 3. Sort Movies            |\n";
        cout << "  | 4. Edit Movie             |\n";
        cout << "  | 5. Delete Movie           |\n";
        cout << "  | 6. Undo Last Action       |\n";
        cout << "  | 0. Exit                   |\n";
        cout << "  +---------------------------+\n";
        cout << "  Choice: ";
        cin >> choice;
        cout << "\n";

        switch (choice) {
            case 1:
                displayMovies();
                bookTicket();
                break;
            case 2:
                displayMovies();
                searchMovie();
                break;
            case 3:
                saveUndo();
                sortMoviesMenu();
                break;
            case 4:
                saveUndo();
                displayMovies();
                editMovie();
                cout << "\n  After editing:\n";
                displayMovies();
                break;
            case 5:
                saveUndo();
                displayMovies();
                deleteMovie();
                cout << "\n  After deletion:\n";
                displayMovies();
                break;
            case 6:
                undoLast();
                break;
            case 0:
                cout << "  Goodbye!\n";
                break;
            default:
                cout << "  Invalid choice. Please enter 0-6.\n";
        }
        cout << "\n";
    } while (choice != 0);

    freeList(head);
    freeUndoStack();
    return 0;
}