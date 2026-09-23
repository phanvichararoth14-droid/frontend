#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <algorithm>
#include <vector>
#include <set>
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
    int        id;
    char       movie[50];
    float      price;
    Seat       seats[5][10];
    MovieNode* next;
};
// ==================== GLOBALS ====================
MovieNode* head = NULL;
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
// ==================== LINKED LIST ====================
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
void appendNode(MovieNode*& listHead, MovieNode* n) {
    if (!listHead) { listHead = n; return; }
    MovieNode* cur = listHead;
    while (cur->next) cur = cur->next;
    cur->next = n;
}
void freeList(MovieNode*& listHead) {
    while (listHead) {
        MovieNode* t = listHead;
        listHead = listHead->next;
        delete t;
    }
}
int listSize(MovieNode* listHead) {
    int n = 0;
    for (MovieNode* c = listHead; c; c = c->next) n++;
    return n;
}
MovieNode* findById(MovieNode* listHead, int id) {
    for (MovieNode* c = listHead; c; c = c->next)
        if (c->id == id) return c;
    return NULL;
}
// ==================== SEARCH ====================
// Sequential search — recursive, walks the linked list node by node
MovieNode* searchRecursive(MovieNode* cur, int targetId) {
    if (!cur) return NULL;
    if (cur->id == targetId) return cur;
    return searchRecursive(cur->next, targetId);
}
// Binary search — copies linked list pointers into array, sorts, then searches
MovieNode* binarySearchById(int targetId) {
    int n = listSize(head);
    if (!n) return NULL;
    MovieNode** arr = new MovieNode*[n];
    int i = 0;
    for (MovieNode* cur = head; cur; cur = cur->next) arr[i++] = cur;
    sort(arr, arr + n, [](MovieNode* a, MovieNode* b){ return a->id < b->id; });
    int low = 0, high = n - 1;
    MovieNode* result = NULL;
    while (low <= high) {
        int mid = low + (high - low) / 2;
        if      (arr[mid]->id == targetId) { result = arr[mid]; break; }
        else if (arr[mid]->id  < targetId) low  = mid + 1;
        else                               high = mid - 1;
    }
    delete[] arr;
    return result;
}
// ==================== FILE I/O ====================
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
        char buf[50];
        strncpy(buf, title.c_str(), 49); buf[49] = '\0';
        MovieNode* node = createNode(id, buf, price);
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
        if (!tok.empty()) {
            int r, c;
            if (parseSeat(tok, r, c)) node->seats[r][c].booked = true;
        }
        appendNode(head, node);
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
// ==================== SORT (BY PRICE) ====================
void sortList(int algorithm) {
    int n = listSize(head);
    if (n < 2) return;
    // Copy linked list node pointers into a temporary array for sorting
    MovieNode** arr = new MovieNode*[n];
    int i = 0;
    for (MovieNode* cur = head; cur; cur = cur->next) arr[i++] = cur;
    auto part = [&](int lo, int hi) -> int {
        float pv = arr[hi]->price; int idx = lo - 1;
        for (int j = lo; j < hi; j++) if (arr[j]->price <= pv) swap(arr[++idx], arr[j]);
        swap(arr[idx+1], arr[hi]); return idx+1;
    };
    auto qs = [&](int lo, int hi, auto& self) -> void {
        if (lo >= hi) return;
        int pi = part(lo, hi);
        self(lo, pi-1, self);
        self(pi+1, hi, self);
    };
    switch (algorithm) {
        case 1:
            qs(0, n-1, qs);
            cout << "  Sorted using Quick Sort.\n"; break;
        case 2:
            for (int j=1;j<n;j++) {
                MovieNode* k=arr[j]; int m=j-1;
                while(m>=0 && arr[m]->price>k->price) { arr[m+1]=arr[m]; m--; }
                arr[m+1]=k;
            }
            cout << "  Sorted using Insertion Sort.\n"; break;
        case 3:
            for (int j=0;j<n-1;j++) {
                int mi=j;
                for(int k=j+1;k<n;k++) if(arr[k]->price<arr[mi]->price) mi=k;
                if(mi!=j) swap(arr[j],arr[mi]);
            }
            cout << "  Sorted using Selection Sort.\n"; break;
        case 4: {
            auto hf=[&](int sz,int root,auto& self)->void {
                int lg=root,l=2*root+1,r=2*root+2;
                if(l<sz&&arr[l]->price>arr[lg]->price) lg=l;
                if(r<sz&&arr[r]->price>arr[lg]->price) lg=r;
                if(lg!=root){swap(arr[root],arr[lg]);self(sz,lg,self);}
            };
            for(int j=n/2-1;j>=0;j--) hf(n,j,hf);
            for(int j=n-1;j>0;j--) { swap(arr[0],arr[j]); hf(j,0,hf); }
            cout << "  Sorted using Heap Sort.\n"; break;
        }
        case 5:
            for(int j=0;j<n-1;j++) {
                bool sw=false;
                for(int k=0;k<n-1-j;k++)
                    if(arr[k]->price>arr[k+1]->price) { swap(arr[k],arr[k+1]); sw=true; }
                if(!sw) break;
            }
            cout << "  Sorted using Bubble Sort.\n"; break;
        default:
            cout << "  Invalid choice!\n"; delete[] arr; return;
    }
    // Re-link the sorted array back into the linked list
    head = arr[0];
    for (int j = 0; j < n-1; j++) arr[j]->next = arr[j+1];
    arr[n-1]->next = NULL;
    delete[] arr;
}
void sortMoviesMenu() {
    int sub;
    cout << "\n  --- Sort by Price ---\n"
         << "  1. Quick Sort\n"
         << "  2. Insertion Sort\n"
         << "  3. Selection Sort\n"
         << "  4. Heap Sort\n"
         << "  5. Bubble Sort\n"
         << "  Choice: ";
    cin >> sub;
    sortList(sub);
    saveMovies();
}
// ==================== DISPLAY ====================
void displayMovies() {
    cout << "\n";
    cout << "  +=====================================================+\n";
    cout << "  |                   NOW SHOWING                     |\n";
    cout << "  +======+========================+==========+=========+\n";
    cout << "  |  ID  |  Movie                 |  Avail   |  Price  |\n";
    cout << "  +------+------------------------+----------+---------+\n";
    for (MovieNode* cur = head; cur; cur = cur->next) {
        string avStr = to_string(countAvailable(cur)) + " / 50";
        cout << "  |  "   << left  << setw(4)  << cur->id
             << "|  "     << left  << setw(22) << cur->movie
             << "|  "     << left  << setw(8)  << avStr
             << "|  $"    << fixed << setprecision(2) << left << setw(5) << cur->price
             << "  |\n";
    }
    cout << "  +======+========================+==========+=========+\n\n";
}
void displayOne(MovieNode* t) {
    string seatsStr = to_string(countAvailable(t)) + " / 50 available";
    cout << "\n";
    cout << "  +------------------------------------+\n";
    cout << "  |           MOVIE DETAILS            |\n";
    cout << "  +------------------------------------+\n";
    cout << "  |  ID    : " << left << setw(26) << t->id    << "|\n";
    cout << "  |  Movie : " << left << setw(26) << t->movie << "|\n";
    cout << "  |  Seats : " << left << setw(26) << seatsStr << "|\n";
    cout << "  |  Price : $" << fixed << setprecision(2)
         << left << setw(25) << t->price                    << "|\n";
    cout << "  +------------------------------------+\n\n";
}
// ==================== SEAT MAP ====================
//
//  Available  -> shows seat label  e.g.  A1  B10
//  Your pick  -> [*]
//  Booked     -> [X]
//
void printSeatMap(MovieNode* node,
                  const vector<pair<int,int>>& selectedSeats,
                  int qty) {
    set<pair<int,int>> selSet(selectedSeats.begin(), selectedSeats.end());
    int picked    = (int)selectedSeats.size();
    int remaining = qty - picked;
    cout << "\n";
    cout << "  +----------------------------------------------------+\n";
    // Title
    string title = string(node->movie);
    if ((int)title.size() > 33) title = title.substr(0, 30) + "...";
    string titleLine = "  Seat Map: " + title;
    cout << "  |  " << left << setw(50) << titleLine << "|\n";
    // Progress (only when actively booking)
    if (qty > 0) {
        string prog = "  Picked " + to_string(picked) + " of "
                    + to_string(qty) + ", " + to_string(remaining) + " left";
        cout << "  |  " << left << setw(50) << prog << "|\n";
    }
    cout << "  +----------+------------------------------------------+\n";
    cout << "  |          |     +-------- SCREEN --------+           |\n";
    cout << "  |          |                                           |\n";
    // Column number header — each cell is 4 wide
    cout << "  |          |";
    for (int c = 1; c <= COLS; c++) cout << setw(4) << c;
    cout << "   |\n";
    cout << "  |   ROW    +";
    cout << string(41, '-') << "|\n";
    // Seat rows
    for (int r = 0; r < 5; r++) {
        cout << "  |    " << ROWS[r] << "     |";
        for (int c = 0; c < COLS; c++) {
            if (selSet.count({r, c})) {
                cout << " [*]";
            } else if (node->seats[r][c].booked) {
                cout << " [X]";
            } else {
                // e.g. "A1 ", "A10"
                string lbl(1, ROWS[r]);
                lbl += to_string(c + 1);
                while ((int)lbl.size() < 3) lbl += " ";
                cout << " " << lbl;
            }
        }
        cout << "   |\n";
    }
    cout << "  |          +";
    cout << string(41, '-') << "|\n";
    cout << "  |  Legend: label = Available  [*] = Your pick  [X] = Booked  |\n";
    // Running picks list
    if (!selectedSeats.empty()) {
        string picks = "  Your picks: ";
        for (int i = 0; i < picked; i++) {
            picks += ROWS[selectedSeats[i].first];
            picks += to_string(selectedSeats[i].second + 1);
            if (i < picked - 1) picks += ", ";
        }
        cout << "  |  " << left << setw(50) << picks << "|\n";
    }
    cout << "  +----------------------------------------------------+\n\n";
}
void printSeatMapPlain(MovieNode* node) {
    vector<pair<int,int>> empty;
    printSeatMap(node, empty, 0);
}
// ==================== RECEIPT ====================
void printReceipt(MovieNode* node,
                  const vector<pair<int,int>>& pickedSeats,
                  const string& customerName) {
    time_t now = time(0);
    char timeBuf[32];
    strftime(timeBuf, sizeof(timeBuf), "%Y-%m-%d %H:%M:%S", localtime(&now));
    float total = (float)pickedSeats.size() * node->price;
    cout << "\n";
    cout << "  +==========================================+\n";
    cout << "  |          MOVIE TICKET RECEIPT            |\n";
    cout << "  +==========================================+\n";
    cout << "  |  Date     : " << left << setw(29) << timeBuf      << "|\n";
    cout << "  |  Customer : " << left << setw(29) << customerName << "|\n";
    cout << "  +------------------------------------------+\n";
    cout << "  |  Movie    : " << left << setw(29) << node->movie  << "|\n";
    cout << "  |  Price    : $" << fixed << setprecision(2)
         << left << setw(28) << node->price                        << "|\n";
    cout << "  +------------------------------------------+\n";
    cout << "  |  Seats Booked:                           |\n";
    string line = "";
    for (int i = 0; i < (int)pickedSeats.size(); i++) {
        string lbl(1, ROWS[pickedSeats[i].first]);
        lbl += to_string(pickedSeats[i].second + 1);
        line += lbl;
        if (i < (int)pickedSeats.size() - 1) line += "  ";
        if ((i + 1) % 8 == 0 || i == (int)pickedSeats.size() - 1) {
            cout << "  |    " << left << setw(38) << line << "|\n";
            line = "";
        }
    }
    cout << "  +------------------------------------------+\n";
    cout << "  |  Quantity : " << left << setw(29) << pickedSeats.size() << "|\n";
    cout << "  |  TOTAL    : $" << fixed << setprecision(2)
         << left << setw(28) << total                               << "|\n";
    cout << "  +==========================================+\n";
    cout << "  |      Thank you for your purchase!        |\n";
    cout << "  +==========================================+\n\n";
}
// ==================== BOOK TICKET ====================
void bookTicket() {
    int id;
    cout << "  Enter movie ID: ";
    cin >> id;
    // Traverse linked list to find the movie
    MovieNode* node = searchRecursive(head, id);
    if (!node) { cout << "  Movie not found.\n"; return; }
    int avail = countAvailable(node);
    if (avail == 0) { cout << "  No seats available for this movie.\n"; return; }
    displayOne(node);
    int qty;
    cout << "  How many tickets? (1-" << avail << "): ";
    cin >> qty;
    if (qty <= 0 || qty > avail) { cout << "  Invalid number of tickets.\n"; return; }
    string customerName;
    cout << "  Enter customer name: ";
    cin.ignore();
    getline(cin, customerName);
    if (customerName.empty()) customerName = "Guest";
    // Show initial empty seat map
    vector<pair<int,int>> chosen;
    cout << "\n  Type a seat label (e.g. A1, B5, C10) and press Enter.\n";
    cout << "  Commands: 'undo' = remove last pick  |  'cancel' = abort booking\n";
    printSeatMap(node, chosen, qty);
    // Seat picking loop
    while ((int)chosen.size() < qty) {
        cout << "  >> Seat " << (chosen.size() + 1) << " of " << qty << ": ";
        string s; cin >> s;
        if (s == "cancel" || s == "CANCEL") {
            for (auto& p : chosen) node->seats[p.first][p.second].booked = false;
            cout << "  Booking cancelled. All seats released.\n";
            return;
        }
        if (s == "undo" || s == "UNDO") {
            if (chosen.empty()) {
                cout << "  Nothing to undo.\n";
            } else {
                auto last = chosen.back();
                node->seats[last.first][last.second].booked = false;
                chosen.pop_back();
                cout << "  Removed " << ROWS[last.first] << (last.second + 1) << ".\n";
                printSeatMap(node, chosen, qty);
            }
            continue;
        }
        int r, c;
        if (!parseSeat(s, r, c)) {
            cout << "  Invalid format. Use row letter A-E + column 1-10 (e.g. A5, C10).\n";
            continue;
        }
        if (node->seats[r][c].booked) {
            cout << "  Seat " << ROWS[r] << (c+1) << " is already booked. Pick another.\n";
            continue;
        }
        bool dup = false;
        for (auto& p : chosen) if (p.first == r && p.second == c) { dup = true; break; }
        if (dup) {
            cout << "  You already picked " << ROWS[r] << (c+1) << ". Choose a different seat.\n";
            continue;
        }
        // Mark seat and refresh map
        node->seats[r][c].booked = true;
        chosen.push_back({r, c});
        cout << "  Seat " << ROWS[r] << (c+1) << " selected!\n";
        printSeatMap(node, chosen, qty);
    }
    // Confirmation
    cout << "  ----------------------------------------\n";
    cout << "  Seats  : ";
    for (int i = 0; i < (int)chosen.size(); i++) {
        cout << ROWS[chosen[i].first] << (chosen[i].second + 1);
        if (i < (int)chosen.size() - 1) cout << ", ";
    }
    cout << "\n  Total  : $" << fixed << setprecision(2)
         << (float)chosen.size() * node->price << "\n";
    cout << "  ----------------------------------------\n";
    cout << "  Confirm booking? (y/n): ";
    char confirm; cin >> confirm;
    if (confirm != 'y' && confirm != 'Y') {
        for (auto& p : chosen) node->seats[p.first][p.second].booked = false;
        cout << "  Booking cancelled. Seats released.\n";
        return;
    }
    saveMovies();
    printReceipt(node, chosen, customerName);
}
// ==================== SEARCH MENU ====================
void searchMovie() {
    int sub, id;
    cout << "\n  --- Search Options ---\n"
         << "  1. Sequential Search (Recursive)\n"
         << "  2. Binary Search\n"
         << "  Choice: ";
    cin >> sub;
    cout << "  Enter movie ID: ";
    cin >> id;
    MovieNode* result = (sub == 1) ? searchRecursive(head, id)
                                   : binarySearchById(id);
    if (result) {
        displayOne(result);
        char showMap;
        cout << "  View seat map? (y/n): ";
        cin >> showMap;
        if (showMap == 'y' || showMap == 'Y') printSeatMapPlain(result);
    } else {
        cout << "  Movie not found.\n";
    }
}
// ==================== EDIT ====================
void editMovie() {
    int id;
    cout << "  Enter movie ID to edit: ";
    cin >> id;
    MovieNode* node = findById(head, id);
    if (!node) { cout << "  Movie not found.\n"; return; }
    cout << "  Enter new price (current: $"
         << fixed << setprecision(2) << node->price << "): ";
    cin >> node->price;
    char reset;
    cout << "  Reset all seats to available? (y/n): ";
    cin >> reset;
    if (reset == 'y' || reset == 'Y') { initSeats(node); cout << "  All seats reset.\n"; }
    cout << "  Movie updated.\n";
    saveMovies();
}
// ==================== DELETE ====================
void deleteMovie() {
    int id;
    cout << "  Enter movie ID to delete: ";
    cin >> id;
    if (!head) { cout << "  List is empty.\n"; return; }
    MovieNode* prev = NULL;
    MovieNode* cur  = head;
    while (cur && cur->id != id) { prev = cur; cur = cur->next; }
    if (!cur) { cout << "  Movie not found.\n"; return; }
    displayOne(cur);
    char confirm;
    cout << "  Are you sure? (y/n): ";
    cin >> confirm;
    if (confirm != 'y' && confirm != 'Y') { cout << "  Delete cancelled.\n"; return; }
    // Unlink the node from the linked list
    if (!prev) head = cur->next;
    else       prev->next = cur->next;
    delete cur;

    cout << "  Movie deleted.\n";
    saveMovies();
}
// ==================== MAIN ====================
int main() {
    int choice;
    loadMovies();
    displayMovies();
    do {
        cout << "  +--------------------------+\n";
        cout << "  |        MAIN MENU         |\n";
        cout << "  +--------------------------+\n";
        cout << "  |  1. Book Ticket          |\n";
        cout << "  |  2. Search Movie         |\n";
        cout << "  |  3. Sort Movies          |\n";
        cout << "  |  4. Edit Movie           |\n";
        cout << "  |  5. Delete Movie         |\n";
        cout << "  |  0. Exit                 |\n";
        cout << "  +--------------------------+\n";
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
                displayMovies();
                sortMoviesMenu();
                cout << "\n  After sorting:\n";
                displayMovies();
                break;
            case 4:
                displayMovies();
                editMovie();
                cout << "\n  After editing:\n";
                displayMovies();
                break;
            case 5:
                displayMovies();
                deleteMovie();
                cout << "\n  After deletion:\n";
                displayMovies();
                break;
            case 0:
                cout << "  Goodbye!\n";
                break;
            default:
                cout << "  Invalid choice. Enter 0-5.\n";
        }
        cout << "\n";
    } while (choice != 0);

    freeList(head);
    return 0;
}