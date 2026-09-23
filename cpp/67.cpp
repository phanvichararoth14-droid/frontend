#include <iostream>
#include <fstream>
using namespace std;
// ========================================
//        DATA STRUCTURE - nodeType
// ========================================
struct nodeType {
    int info;        
    nodeType* prev;
    nodeType* next;
};
// ========================================
// a) Create Data Structure
// ========================================
// Initialize List
nodeType* InitializeList() {
    nodeType* headList = NULL;
    return headList;
}
// GetNode
nodeType* GetNode(int value) {
    nodeType* newNode = new nodeType;
    newNode->info = value;
    newNode->prev = NULL;
    newNode->next = NULL;
    return newNode;
}
// FreeNode
void FreeNode(nodeType* newNode) {
    delete newNode;
}
// ========================================
// b) Create Algorithms
// ========================================
// InsertNode (at end)
void InsertNode(nodeType*& headList, int value) {
    nodeType* newNode = GetNode(value);
    if (headList == NULL) {
        headList = newNode;
    } else {
        nodeType* temp = headList;
        while (temp->next != NULL)
            temp = temp->next;
        temp->next = newNode;
        newNode->prev = temp;
    }
}
// InsertNode at position (0 = head)
void InsertAtPosition(nodeType*& headList, int value, int pos) {
    nodeType* newNode = GetNode(value);
    if (pos <= 0 || headList == NULL) {
        newNode->next = headList;
        if (headList != NULL)
            headList->prev = newNode;
        headList = newNode;
        return;
    }
    nodeType* temp = headList;
    for (int i = 0; i < pos - 1 && temp->next != NULL; i++)
        temp = temp->next;
    newNode->next = temp->next;
    newNode->prev = temp;
    if (temp->next != NULL)
        temp->next->prev = newNode;
    temp->next = newNode;
}
// Traverse (display list)
void Traverse(nodeType* headList) {
    if (headList == NULL) {
        cout << "List is empty.\n";
        return;
    }
    nodeType* temp = headList;
    cout << "NULL <-> ";
    while (temp != NULL) {
        cout << "[" << temp->info << "]";
        if (temp->next != NULL) cout << " <-> ";
        temp = temp->next;
    }
    cout << " <-> NULL\n";
}
// CountNode
int CountNode(nodeType* headList) {
    int count = 0;
    nodeType* temp = headList;
    while (temp != NULL) {
        count++;
        temp = temp->next;
    }
    return count;
}
// ========================================
// Helper: copy list into array (for search)
// ========================================
int ListToArray(nodeType* headList, int a[], int maxSize) {
    int n = 0;
    nodeType* temp = headList;
    while (temp != NULL && n < maxSize) {
        a[n++] = temp->info;
        temp = temp->next;
    }
    return n;
}
// ========================================
// Sequential Search
// ========================================
int Sequential(int a[], int n) {
    int i;
    int Item;
    cout << "  Enter Item to search: ";
    cin >> Item;
    for (i = 0; i < n; i++) {
        if (Item == a[i])
            return i;   // index starts from 0
    }
    return -1;
}
// ========================================
// Binary Search (list must be sorted first)
// ========================================
int Binary(int a[], int n) {
    int Item;
    cout << "  Enter Item to search: ";
    cin >> Item;
    int Left = 0;
    int Right = n - 1;
    do {
        int Mid = (Left + Right) / 2;
        if (Item == a[Mid])
            return Mid;
        if (Item < a[Mid])
            Right = Mid - 1;
        else
            Left = Mid + 1;
    } while (Left <= Right);
    return -1;
}
// ========================================
// SearchNode - sub-menu
// ========================================
void SearchNode(nodeType* headList) {
    if (headList == NULL) {
        cout << "List is empty.\n";
        return;
    }
    int a[100];
    int n = ListToArray(headList, a, 100);
    cout << "\n  -- Search Method --\n";
    cout << "  1. Sequential Search\n";
    cout << "  2. Binary Search (requires sorted list)\n";
    cout << "  Choice: ";
    int searchChoice;
    cin >> searchChoice;
    cout << "\n";
    int result = -1;
    if (searchChoice == 1) {
        cout << "  [ Sequential Search ]\n";
        result = Sequential(a, n);
    } else if (searchChoice == 2) {
        cout << "  [ Binary Search ]\n";
        cout << "  Note: Binary search requires the list to be sorted!\n";
        result = Binary(a, n);
    } else {
        cout << "  Invalid choice.\n";
        return;
    }
    if (result != -1)
        cout << "  Found! Value " << a[result] << " is at position " << result << " in the list.\n";
    else
        cout << "  Not found in list.\n";
}
// UpdateNode
void UpdateNode(nodeType* headList, int oldVal, int newVal) {
    nodeType* temp = headList;
    while (temp != NULL) {
        if (temp->info == oldVal) {
            temp->info = newVal;
            cout << "Updated " << oldVal << " -> " << newVal << " successfully.\n";
            return;
        }
        temp = temp->next;
    }
    cout << "Value " << oldVal << " not found in list!\n";
}
// DeleteNode
void DeleteNode(nodeType*& headList, int value) {
    nodeType* temp = headList;
    while (temp != NULL && temp->info != value)
        temp = temp->next;

    if (temp == NULL) {
        cout << "Value " << value << " not found in list!\n";
        return;
    }
    if (temp->prev != NULL)
        temp->prev->next = temp->next;
    else
        headList = temp->next;
    if (temp->next != NULL)
        temp->next->prev = temp->prev;
    delete temp;
    cout << "Deleted node with value " << value << ".\n";
}
// ========================================
// SortNode - sub-menu (Bubble Sort)
// ========================================
void SortNode(nodeType* headList) {
    if (headList == NULL) {
        cout << "List is empty.\n";
        return;
    }

    cout << "\n  -- Sort Method --\n";
    cout << "  1. Bubble Sort (ascending)\n";
    cout << "  2. Bubble Sort (descending)\n";
    cout << "  Choice: ";
    int sortChoice;
    cin >> sortChoice;
    cout << "\n";

    if (sortChoice != 1 && sortChoice != 2) {
        cout << "  Invalid choice.\n";
        return;
    }

    int a[100];
    int n = ListToArray(headList, a, 100);

    int i, j, temp;
    for (j = 1; j < n; j++)
        for (i = 0; i < n - j; i++)
            if (sortChoice == 1 ? a[i] > a[i+1] : a[i] < a[i+1]) {
                temp   = a[i];
                a[i]   = a[i+1];
                a[i+1] = temp;
            }

    nodeType* curr = headList;
    for (int k = 0; k < n; k++) {
        curr->info = a[k];
        curr = curr->next;
    }

    if (sortChoice == 1)
        cout << "  List sorted in ascending order (Bubble Sort).\n";
    else
        cout << "  List sorted in descending order (Bubble Sort).\n";
}

// Reverse the list
void ReverseList(nodeType*& headList) {
    nodeType* temp = NULL;
    nodeType* curr = headList;
    while (curr != NULL) {
        temp       = curr->prev;
        curr->prev = curr->next;
        curr->next = temp;
        curr       = curr->prev;
    }
    if (temp != NULL)
        headList = temp->prev;
    cout << "List reversed.\n";
}

// FreeList (clear all)
void FreeList(nodeType*& headList) {
    nodeType* temp;
    while (headList != NULL) {
        temp     = headList;
        headList = headList->next;
        delete temp;
    }
    cout << "List cleared.\n";
}

// Save to file
void WriteFile(nodeType* headList) {
    ofstream fout("data.txt");
    if (!fout) {
        cout << "Error opening file for writing!\n";
        return;
    }
    nodeType* temp = headList;
    while (temp != NULL) {
        fout << temp->info << " ";
        temp = temp->next;
    }
    fout.close();
    cout << "List saved to data.txt\n";
}

// Read from file
nodeType* ReadFile() {
    ifstream fin("data.txt");
    if (!fin) {
        cout << "Error: data.txt not found!\n";
        return NULL;
    }
    nodeType* headList = NULL;
    int x;
    while (fin >> x)
        InsertNode(headList, x);
    fin.close();
    cout << "List loaded from data.txt\n";
    return headList;
}

// ========================================
//           MENU
// ========================================
void printLine() {
    cout << "----------------------------------------\n";
}

void printMenu() {
    printLine();
    cout << "  DOUBLY LINKED LIST (DLL) MENU\n";
    printLine();
    cout << "  1.  Traverse (Display list)\n";
    cout << "  2.  CountNode\n";
    cout << "  3.  SearchNode (Sequential / Binary)\n";
    cout << "  4.  InsertNode (at end)\n";
    cout << "  5.  InsertNode (at position)\n";
    cout << "  6.  UpdateNode\n";
    cout << "  7.  DeleteNode\n";
    cout << "  8.  SortNode (ascending / descending)\n";
    cout << "  9.  Reverse list\n";
    cout << " 10.  Save to file\n";
    cout << " 11.  Load from file\n";
    cout << " 12.  FreeList (clear)\n";
    cout << "  0.  Exit\n";
    printLine();
    cout << "  Choice: ";
}

// ========================================
//           MAIN
// ========================================
int main() {
    nodeType* headList = InitializeList();
    int n, val, choice;
    cout << "Group\n";
    cout << " - Bros Sophary\n - Phan Vichararoth\n - Maeng Chansreyha\n";
    cout << "========================================\n";
    cout << "     DOUBLY LINKED LIST PROGRAM (DLL)\n";
    cout << "========================================\n";
    cout << "\nHow many nodes do you want to enter? ";
    while (!(cin >> n) || n <= 0) {
        cin.clear();
        cin.ignore(1000, '\n');
        cout << "Please enter a positive integer: ";
    }
    cout << "\nEnter " << n << " values:\n";
    for (int i = 0; i < n; i++) {
        cout << "  Node [" << i << "]: ";
        while (!(cin >> val)) {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "  Invalid input. Node [" << i << "]: ";
        }
        InsertNode(headList, val);
    }
    cout << "\nInitial list: ";
    Traverse(headList);
    printMenu();
    do {
        while (!(cin >> choice)) {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "  Invalid. Choice: ";
        }
        cout << "\n";
        switch (choice) {
            case 1:
                cout << "List: ";
                Traverse(headList);
                break;
            case 2:
                cout << "Number of nodes: " << CountNode(headList) << "\n";
                break;
            case 3:
                SearchNode(headList);
                break;
            case 4:
                cout << "Value to insert at end: ";
                cin >> val;
                InsertNode(headList, val);
                cout << "Inserted " << val << " at end.\n";
                cout << "List: ";
                Traverse(headList);
                break;
            case 5: {
                int pos;
                cout << "Value to insert: ";
                cin >> val;
                cout << "Position (0 = head): ";
                cin >> pos;
                InsertAtPosition(headList, val, pos);
                cout << "Inserted " << val << " at position " << pos << ".\n";
                cout << "List: ";
                Traverse(headList);
                break;
            }
            case 6: {
                int oldVal, newVal;
                cout << "Old value: ";
                cin >> oldVal;
                cout << "New value: ";
                cin >> newVal;
                UpdateNode(headList, oldVal, newVal);
                cout << "List: ";
                Traverse(headList);
                break;
            }
            case 7:
                cout << "Value to delete: ";
                cin >> val;
                DeleteNode(headList, val);
                cout << "List: ";
                Traverse(headList);
                break;
            case 8:
                SortNode(headList);
                cout << "List: ";
                Traverse(headList);
                break;
            case 9:
                ReverseList(headList);
                cout << "List: ";
                Traverse(headList);
                break;
            case 10:
                WriteFile(headList);
                break;
            case 11: {
                nodeType* loaded = ReadFile();
                if (loaded != NULL) {
                    FreeList(headList);
                    headList = loaded;
                    cout << "List: ";
                    Traverse(headList);
                }
                break;
            }
            case 12:
                FreeList(headList);
                break;
            case 0:
                cout << "Goodbye!\n";
                break;
            default:
                cout << "Invalid choice. Please enter 0-12.\n";
        }
        if (choice != 0)
            cout << "\n  Choice: ";
    } while (choice != 0);
    FreeList(headList);
    return 0;
}