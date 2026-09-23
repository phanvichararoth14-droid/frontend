#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <string>
#include <algorithm>
#include <stack>
#include <cstring>
#include <cstdio>
using namespace std;

const char* FILENAME = "D:\\DEVcode\\movie2.txt";

// ==================== STRUCTS ====================
struct Ticket {
    int id;
    char movie[50];
    int seat;
    float price;
};

struct MovieBST {
    int id;
    char movie[50];
    int seat;
    float price;
    MovieBST* left;
    MovieBST* right;
};

// ==================== GLOBALS ====================
vector<Ticket> movies;
MovieBST* bstRoot = NULL;
stack<vector<Ticket> > undoStack;

// ==================== FORWARD DECLARATIONS ====================
void displayMovies();
void displayOne(const Ticket& t);

// ==================== FILE FUNCTIONS ====================
void loadMovies() {
    ifstream file(FILENAME);
    if (!file.is_open()) {
        cout << "Error: Could not open " << FILENAME << "\n";
        return;
    }
    movies.clear();
    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;
        Ticket t;
        sscanf(line.c_str(), "%d,%49[^,],%d,%f",
               &t.id, t.movie, &t.seat, &t.price);
        movies.push_back(t);
    }
    file.close();
}

void saveMovies() {
    ofstream file(FILENAME);
    if (!file.is_open()) {
        cout << "Error: Could not save to " << FILENAME << "\n";
        return;
    }
    for (int i = 0; i < (int)movies.size(); i++) {
        file << movies[i].id << "," << movies[i].movie << ","
             << movies[i].seat << "," << fixed << setprecision(2)
             << movies[i].price << "\n";
    }
    file.close();
}

// ==================== BST FUNCTIONS ====================
MovieBST* bstInsert(MovieBST* root, Ticket t) {
    if (root == NULL) {
        MovieBST* node = new MovieBST();
        node->id = t.id;
        strcpy(node->movie, t.movie);
        node->seat = t.seat;
        node->price = t.price;
        node->left = node->right = NULL;
        return node;
    }
    if (t.id < root->id)
        root->left = bstInsert(root->left, t);
    else
        root->right = bstInsert(root->right, t);
    return root;
}

MovieBST* bstSearch(MovieBST* root, int id) {
    if (root == NULL) return NULL;
    if (root->id == id) return root;
    if (id < root->id)
        return bstSearch(root->left, id);
    else
        return bstSearch(root->right, id);
}

void bstFree(MovieBST* root) {
    if (root == NULL) return;
    bstFree(root->left);
    bstFree(root->right);
    delete root;
}

void buildBST() {
    bstFree(bstRoot);
    bstRoot = NULL;
    for (int i = 0; i < (int)movies.size(); i++)
        bstRoot = bstInsert(bstRoot, movies[i]);
}

// ==================== SEQUENTIAL SEARCH (RECURSIVE) ====================
int searchRecursive(int index, int targetId) {
    if (index >= (int)movies.size()) return -1;
    if (movies[index].id == targetId) return index;
    return searchRecursive(index + 1, targetId);
}

// ==================== BINARY SEARCH ====================
// Requires data sorted by id. Works on a local sorted copy so the
// original 'movies' vector (and its display order) is never disturbed.
int binarySearchById(const vector<Ticket>& sortedArr, int targetId) {
    int low = 0, high = (int)sortedArr.size() - 1;
    while (low <= high) {
        int mid = low + (high - low) / 2;
        if (sortedArr[mid].id == targetId) return mid;
        else if (sortedArr[mid].id < targetId) low = mid + 1;
        else high = mid - 1;
    }
    return -1;
}

// ==================== STACK UNDO ====================
void saveUndo() {
    undoStack.push(movies);
}

void undoLast() {
    if (undoStack.empty()) {
        cout << "Nothing to undo.\n";
        return;
    }
    movies = undoStack.top();
    undoStack.pop();
    saveMovies();
    buildBST();
    cout << "Undo successful!\n";
    displayMovies();
}

// ==================== SORT ALGORITHMS (BY PRICE) ====================

// ---- Quick Sort ----
int partitionByPrice(vector<Ticket>& arr, int low, int high) {
    float pivot = arr[high].price;
    int i = low - 1;
    for (int j = low; j < high; j++) {
        if (arr[j].price <= pivot) {
            i++;
            swap(arr[i], arr[j]);
        }
    }
    swap(arr[i + 1], arr[high]);
    return i + 1;
}

void quickSort(vector<Ticket>& arr, int low, int high) {
    if (low < high) {
        int pi = partitionByPrice(arr, low, high);
        quickSort(arr, low, pi - 1);
        quickSort(arr, pi + 1, high);
    }
}

// ---- Insertion Sort ----
void insertionSort(vector<Ticket>& arr) {
    for (int i = 1; i < (int)arr.size(); i++) {
        Ticket key = arr[i];
        int j = i - 1;
        while (j >= 0 && arr[j].price > key.price) {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = key;
    }
}

// ---- Selection Sort ----
void selectionSort(vector<Ticket>& arr) {
    int n = (int)arr.size();
    for (int i = 0; i < n - 1; i++) {
        int minIdx = i;
        for (int j = i + 1; j < n; j++) {
            if (arr[j].price < arr[minIdx].price)
                minIdx = j;
        }
        if (minIdx != i) swap(arr[i], arr[minIdx]);
    }
}

// ---- Heap Sort ----
void heapify(vector<Ticket>& arr, int n, int i) {
    int largest = i;
    int l = 2 * i + 1;
    int r = 2 * i + 2;

    if (l < n && arr[l].price > arr[largest].price)
        largest = l;
    if (r < n && arr[r].price > arr[largest].price)
        largest = r;

    if (largest != i) {
        swap(arr[i], arr[largest]);
        heapify(arr, n, largest);
    }
}

void heapSort(vector<Ticket>& arr) {
    int n = (int)arr.size();
    for (int i = n / 2 - 1; i >= 0; i--)
        heapify(arr, n, i);
    for (int i = n - 1; i > 0; i--) {
        swap(arr[0], arr[i]);
        heapify(arr, i, 0);
    }
}

// ==================== DISPLAY ====================
void displayMovies() {
    cout << "\n========= MOVIE LIST =========\n";
    cout << left << setw(4)  << "ID"
                 << setw(22) << "Movie"
                 << setw(7)  << "Seats"
                 << "Price\n";
    cout << "--------------------------------------\n";
    for (int i = 0; i < (int)movies.size(); i++) {
        cout << left << setw(4)  << movies[i].id
                     << setw(22) << movies[i].movie
                     << setw(7)  << movies[i].seat
             << "$" << fixed << setprecision(2) << movies[i].price << "\n";
    }
    cout << "--------------------------------------\n";
}

void displayOne(const Ticket& t) {
    cout << "\n--- Search Result ---\n";
    cout << "ID    : " << t.id << "\n";
    cout << "Movie : " << t.movie << "\n";
    cout << "Seats : " << t.seat << "\n";
    cout << "Price : $" << fixed << setprecision(2) << t.price << "\n";
}

// ==================== BOOK TICKET ====================
void bookTicket() {
    int id, qty;
    cout << "Enter movie ID: ";
    cin >> id;

    // use recursive (sequential) search
    int idx = searchRecursive(0, id);
    if (idx == -1) {
        cout << "Movie not found.\n";
        return;
    }
    cout << "How many tickets for " << movies[idx].movie << "? ";
    cin >> qty;
    if (qty > movies[idx].seat) {
        cout << "Only " << movies[idx].seat << " seats available.\n";
        return;
    }
    saveUndo();                   // save state for undo
    movies[idx].seat -= qty;
    cout << "Booked " << qty << " ticket(s). Total: $"
         << fixed << setprecision(2) << qty * movies[idx].price << "\n";
    saveMovies();
    buildBST();                   // sync BST
}

// ==================== SEARCH MENU ====================
void searchMovie() {
    int subChoice, id;
    cout << "\n--- Search Options ---\n"
         << "1. Sequential Search\n"
         << "2. Binary Search\n"
         << "3. BST Search\n"
         << "Choice: ";
    cin >> subChoice;

    cout << "Enter movie ID to search: ";
    cin >> id;

    switch (subChoice) {
        case 1: {
            int idx = searchRecursive(0, id);
            if (idx != -1) displayOne(movies[idx]);
            else cout << "Movie not found.\n";
            break;
        }
        case 2: {
            // Binary search needs sorted-by-id data; sort a local copy only
            vector<Ticket> sortedById = movies;
            sort(sortedById.begin(), sortedById.end(),
                 [](const Ticket& a, const Ticket& b) { return a.id < b.id; });
            int idx = binarySearchById(sortedById, id);
            if (idx != -1) displayOne(sortedById[idx]);
            else cout << "Movie not found.\n";
            break;
        }
        case 3: {
            MovieBST* result = bstSearch(bstRoot, id);
            if (result != NULL) {
                Ticket t;
                t.id = result->id;
                strcpy(t.movie, result->movie);
                t.seat = result->seat;
                t.price = result->price;
                displayOne(t);
            } else {
                cout << "Movie not found.\n";
            }
            break;
        }
        default:
            cout << "Invalid choice!\n";
    }
}

// ==================== SORT MENU ====================
void sortMoviesMenu() {
    int subChoice;
    cout << "\n--- Sort Options (by Price) ---\n"
         << "1. Quick Sort\n"
         << "2. Insertion Sort\n"
         << "3. Selection Sort\n"
         << "4. Heap Sort\n"
         << "Choice: ";
    cin >> subChoice;

    switch (subChoice) {
        case 1:
            if (!movies.empty()) quickSort(movies, 0, (int)movies.size() - 1);
            cout << "Sorted using Quick Sort.\n";
            break;
        case 2:
            insertionSort(movies);
            cout << "Sorted using Insertion Sort.\n";
            break;
        case 3:
            selectionSort(movies);
            cout << "Sorted using Selection Sort.\n";
            break;
        case 4:
            heapSort(movies);
            cout << "Sorted using Heap Sort.\n";
            break;
        default:
            cout << "Invalid choice!\n";
            return;
    }
    saveMovies();
    buildBST();
    cout << "After sorting:\n";
    displayMovies();
}

// ==================== EDIT ====================
void editMovie() {
    int id;
    cout << "Enter movie ID to edit: ";
    cin >> id;
    for (int i = 0; i < (int)movies.size(); i++) {
        if (movies[i].id == id) {
            cout << "Enter new seats: ";
            cin >> movies[i].seat;
            cout << "Enter new price: ";
            cin >> movies[i].price;
            cout << "Movie updated.\n";
            saveMovies();
            buildBST();           // sync BST
            return;
        }
    }
    cout << "Movie not found.\n";
}

// ==================== MAIN ====================
int main() {
    int choice;

    loadMovies();
    buildBST();
    displayMovies();

    do {
        cout << "\nChoose option:\n"
             << "1. Book ticket\n"
             << "2. Search movie\n"
             << "3. Sort movies\n"
             << "4. Edit movie\n"
             << "5. Undo last action\n"
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
                undoLast();
                break;
            case 0:
                cout << "Goodbye!\n";
                bstFree(bstRoot);
                break;
            default:
                cout << "Invalid choice!\n";
        }
    } while (choice != 0);

    return 0;
}