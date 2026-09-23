#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <algorithm>
#include <stack>
#include <cstring>
#include <cstdio>
using namespace std;

const char* FILENAME = "D:\\DEVcode\\movie2.txt";

// ==================== STRUCT ====================

struct MovieNode {
    int id;
    char movie[50];
    int seat;
    float price;
    MovieNode* next;
};

// ==================== GLOBALS ====================
MovieNode* head = NULL;
stack<MovieNode*> undoStack;

// ==================== LINKED LIST HELPERS ====================

MovieNode* createNode(int id, const char* movie, int seat, float price) {
    MovieNode* node = new MovieNode();
    node->id    = id;
    strncpy(node->movie, movie, 49);
    node->movie[49] = '\0';
    node->seat  = seat;
    node->price = price;
    node->next  = NULL;
    return node;
}

void appendNode(MovieNode*& listHead, int id, const char* movie, int seat, float price) {
    MovieNode* newNode = createNode(id, movie, seat, price);
    if (listHead == NULL) {
        listHead = newNode;
        return;
    }
    MovieNode* cur = listHead;
    while (cur->next != NULL)
        cur = cur->next;
    cur->next = newNode;
}

MovieNode* copyList(MovieNode* src) {
    MovieNode* newHead = NULL;
    for (MovieNode* cur = src; cur != NULL; cur = cur->next)
        appendNode(newHead, cur->id, cur->movie, cur->seat, cur->price);
    return newHead;
}

void freeList(MovieNode*& listHead) {
    while (listHead != NULL) {
        MovieNode* temp = listHead;
        listHead = listHead->next;
        delete temp;
    }
}

int listSize(MovieNode* listHead) {
    int count = 0;
    for (MovieNode* cur = listHead; cur != NULL; cur = cur->next)
        count++;
    return count;
}

MovieNode* findById(MovieNode* listHead, int id) {
    for (MovieNode* cur = listHead; cur != NULL; cur = cur->next)
        if (cur->id == id) return cur;
    return NULL;
}

// ==================== DISPLAY ====================
void displayMovies() {
    cout << "\n========= MOVIE LIST =========\n";
    cout << left << setw(4)  << "ID"
                 << setw(22) << "Movie"
                 << setw(7)  << "Seats"
                 << "Price\n";
    cout << "--------------------------------------\n";
    for (MovieNode* cur = head; cur != NULL; cur = cur->next) {
        cout << left << setw(4)  << cur->id
                     << setw(22) << cur->movie
                     << setw(7)  << cur->seat
             << "$" << fixed << setprecision(2) << cur->price << "\n";
    }
    cout << "--------------------------------------\n";
}

void displayOne(MovieNode* t) {
    cout << "\n--- Search Result ---\n";
    cout << "ID    : " << t->id    << "\n";
    cout << "Movie : " << t->movie << "\n";
    cout << "Seats : " << t->seat  << "\n";
    cout << "Price : $" << fixed << setprecision(2) << t->price << "\n";
}

// ==================== FILE FUNCTIONS ====================
void loadMovies() {
    ifstream file(FILENAME);
    if (!file.is_open()) {
        cout << "Error: Could not open " << FILENAME << "\n";
        return;
    }
    freeList(head);
    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;
        int   id, seat;
        char  movie[50];
        float price;
        sscanf(line.c_str(), "%d,%49[^,],%d,%f", &id, movie, &seat, &price);
        appendNode(head, id, movie, seat, price);
    }
    file.close();
}

void saveMovies() {
    ofstream file(FILENAME);
    if (!file.is_open()) {
        cout << "Error: Could not save to " << FILENAME << "\n";
        return;
    }
    for (MovieNode* cur = head; cur != NULL; cur = cur->next) {
        file << cur->id << "," << cur->movie << ","
             << cur->seat << "," << fixed << setprecision(2)
             << cur->price << "\n";
    }
    file.close();
}

// ==================== SEQUENTIAL SEARCH (RECURSIVE) ====================
MovieNode* searchRecursive(MovieNode* cur, int targetId) {
    if (cur == NULL) return NULL;
    if (cur->id == targetId) return cur;
    return searchRecursive(cur->next, targetId);
}

// ==================== BINARY SEARCH ====================
MovieNode* binarySearchById(int targetId) {
    int n = listSize(head);
    if (n == 0) return NULL;

    MovieNode** arr = new MovieNode*[n];
    int i = 0;
    for (MovieNode* cur = head; cur != NULL; cur = cur->next)
        arr[i++] = cur;
    sort(arr, arr + n, [](MovieNode* a, MovieNode* b){ return a->id < b->id; });

    int low = 0, high = n - 1;
    MovieNode* result = NULL;
    while (low <= high) {
        int mid = low + (high - low) / 2;
        if (arr[mid]->id == targetId) { result = arr[mid]; break; }
        else if (arr[mid]->id < targetId) low  = mid + 1;
        else                              high = mid - 1;
    }
    delete[] arr;
    return result;
}

// ==================== STACK UNDO ====================
void saveUndo() {
    undoStack.push(copyList(head));
}

void undoLast() {
    if (undoStack.empty()) {
        cout << "Nothing to undo.\n";
        return;
    }
    freeList(head);
    head = undoStack.top();
    undoStack.pop();
    saveMovies();
    cout << "Undo successful!\n";
    displayMovies();
}

// ==================== SORT ALGORITHMS (BY PRICE) ====================
// Node pointers are gathered into a temp array, sorted, then relinked.

void sortList(int algorithm) {
    int n = listSize(head);
    if (n < 2) return;

    MovieNode** arr = new MovieNode*[n];
    int i = 0;
    for (MovieNode* cur = head; cur != NULL; cur = cur->next)
        arr[i++] = cur;

    auto partition = [&](int low, int high) -> int {
        float pivot = arr[high]->price;
        int idx = low - 1;
        for (int j = low; j < high; j++)
            if (arr[j]->price <= pivot)
                swap(arr[++idx], arr[j]);
        swap(arr[idx + 1], arr[high]);
        return idx + 1;
    };

    auto quickSortRange = [&](int lo, int hi, auto& self) -> void {
        if (lo >= hi) return;
        int pi = partition(lo, hi);
        self(lo, pi - 1, self);
        self(pi + 1, hi, self);
    };

    switch (algorithm) {
        case 1: // Quick Sort
            quickSortRange(0, n - 1, quickSortRange);
            cout << "Sorted using Quick Sort.\n";
            break;
        case 2: // Insertion Sort
            for (int j = 1; j < n; j++) {
                MovieNode* key = arr[j];
                int k = j - 1;
                while (k >= 0 && arr[k]->price > key->price) {
                    arr[k + 1] = arr[k];
                    k--;
                }
                arr[k + 1] = key;
            }
            cout << "Sorted using Insertion Sort.\n";
            break;
        case 3: // Selection Sort
            for (int j = 0; j < n - 1; j++) {
                int minIdx = j;
                for (int k = j + 1; k < n; k++)
                    if (arr[k]->price < arr[minIdx]->price) minIdx = k;
                if (minIdx != j) swap(arr[j], arr[minIdx]);
            }
            cout << "Sorted using Selection Sort.\n";
            break;
        case 4: { // Heap Sort
            auto heapify = [&](int sz, int root, auto& self) -> void {
                int largest = root, l = 2*root+1, r = 2*root+2;
                if (l < sz && arr[l]->price > arr[largest]->price) largest = l;
                if (r < sz && arr[r]->price > arr[largest]->price) largest = r;
                if (largest != root) { swap(arr[root], arr[largest]); self(sz, largest, self); }
            };
            for (int j = n/2 - 1; j >= 0; j--) heapify(n, j, heapify);
            for (int j = n - 1; j > 0; j--)    { swap(arr[0], arr[j]); heapify(j, 0, heapify); }
            cout << "Sorted using Heap Sort.\n";
            break;
        }
        case 5: // Bubble Sort
            for (int j = 0; j < n - 1; j++) {
                bool swapped = false;
                for (int k = 0; k < n - 1 - j; k++)
                    if (arr[k]->price > arr[k+1]->price) { swap(arr[k], arr[k+1]); swapped = true; }
                if (!swapped) break;
            }
            cout << "Sorted using Bubble Sort.\n";
            break;
        default:
            cout << "Invalid choice!\n";
            delete[] arr;
            return;
    }

    // Relink nodes in sorted order
    head = arr[0];
    for (int j = 0; j < n - 1; j++)
        arr[j]->next = arr[j + 1];
    arr[n - 1]->next = NULL;

    delete[] arr;
}

// ==================== SORT MENU ====================
void sortMoviesMenu() {
    int subChoice;
    cout << "\n--- Sort Options (by Price) ---\n"
         << "1. Quick Sort\n"
         << "2. Insertion Sort\n"
         << "3. Selection Sort\n"
         << "4. Heap Sort\n"
         << "5. Bubble Sort\n"
         << "Choice: ";
    cin >> subChoice;

    sortList(subChoice);
    saveMovies();
    cout << "After sorting:\n";
    displayMovies();
}

// ==================== BOOK TICKET ====================
void bookTicket() {
    int id, qty;
    cout << "Enter movie ID: ";
    cin >> id;

    MovieNode* node = searchRecursive(head, id);
    if (node == NULL) {
        cout << "Movie not found.\n";
        return;
    }
    cout << "How many tickets for " << node->movie << "? ";
    cin >> qty;
    if (qty > node->seat) {
        cout << "Only " << node->seat << " seats available.\n";
        return;
    }
    saveUndo();
    node->seat -= qty;
    cout << "Booked " << qty << " ticket(s). Total: $"
         << fixed << setprecision(2) << qty * node->price << "\n";
    saveMovies();
}

// ==================== SEARCH MENU ====================
void searchMovie() {
    int subChoice, id;
    cout << "\n--- Search Options ---\n"
         << "1. Sequential Search (Recursive)\n"
         << "2. Binary Search\n"
         << "Choice: ";
    cin >> subChoice;

    cout << "Enter movie ID to search: ";
    cin >> id;

    switch (subChoice) {
        case 1: {
            MovieNode* result = searchRecursive(head, id);
            if (result) displayOne(result);
            else        cout << "Movie not found.\n";
            break;
        }
        case 2: {
            MovieNode* result = binarySearchById(id);
            if (result) displayOne(result);
            else        cout << "Movie not found.\n";
            break;
        }
        default:
            cout << "Invalid choice!\n";
    }
}

// ==================== EDIT ====================
void editMovie() {
    int id;
    cout << "Enter movie ID to edit: ";
    cin >> id;

    MovieNode* node = findById(head, id);
    if (node == NULL) {
        cout << "Movie not found.\n";
        return;
    }
    cout << "Enter new seats: ";
    cin >> node->seat;
    cout << "Enter new price: ";
    cin >> node->price;
    cout << "Movie updated.\n";
    saveMovies();
}

// ==================== DELETE ====================
void deleteMovie() {
    int id;
    cout << "Enter movie ID to delete: ";
    cin >> id;

    if (head == NULL) { cout << "List is empty.\n"; return; }

    MovieNode* prev = NULL;
    MovieNode* cur  = head;
    while (cur != NULL && cur->id != id) {
        prev = cur;
        cur  = cur->next;
    }
    if (cur == NULL) {
        cout << "Movie not found.\n";
        return;
    }

    cout << "\nMovie found:\n";
    displayOne(cur);
    char confirm;
    cout << "Are you sure you want to delete this movie? (y/n): ";
    cin >> confirm;
    if (confirm != 'y' && confirm != 'Y') {
        cout << "Delete cancelled.\n";
        return;
    }

    if (prev == NULL)
        head = cur->next;
    else
        prev->next = cur->next;

    delete cur;
    cout << "Movie deleted successfully.\n";
    saveMovies();
}

// ==================== CLEANUP UNDO STACK ====================
void freeUndoStack() {
    while (!undoStack.empty()) {
        MovieNode* snap = undoStack.top();
        undoStack.pop();
        freeList(snap);
    }
}

// ==================== MAIN ====================
int main() {
    int choice;

    loadMovies();
    displayMovies();

    do {
        cout << "\nChoose option:\n"
             << "1. Book ticket\n"
             << "2. Search movie\n"
             << "3. Sort movies\n"
             << "4. Edit movie\n"
             << "5. Delete movie\n"
             << "6. Undo last action\n"
             << "0. Exit\n"
             << "Choice: ";
        cin >> choice;

        switch (choice) {
            case 1:
                bookTicket();
                break;
            case 2:
                searchMovie();
                break;
            case 3:
                saveUndo();
                sortMoviesMenu();
                break;
            case 4:
                saveUndo();
                editMovie();
                cout << "After editing:\n";
                displayMovies();
                break;
            case 5:
                saveUndo();
                deleteMovie();
                cout << "After deletion:\n";
                displayMovies();
                break;
            case 6:
                undoLast();
                break;
            case 0:
                cout << "Goodbye!\n";
                break;
            default:
                cout << "Invalid choice!\n";
        }
    } while (choice != 0);

    freeList(head);
    freeUndoStack();

    return 0;
}