#include <iostream>
#include <fstream>
using namespace std;

// ========================================
//        DATA STRUCTURE - nodeType
// ========================================
struct nodeType {
    int data;
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
    newNode->data = value;
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
        cout << "[" << temp->data << "]";
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

// SearchNode
nodeType* SearchNode(nodeType* headList, int key) {
    nodeType* temp = headList;
    while (temp != NULL) {
        if (temp->data == key)
            return temp;
        temp = temp->next;
    }
    return NULL;
}

// UpdateNode
void UpdateNode(nodeType* headList, int oldVal, int newVal) {
    nodeType* p = SearchNode(headList, oldVal);
    if (p != NULL) {
        p->data = newVal;
        cout << "Updated " << oldVal << " -> " << newVal << " successfully.\n";
    } else {
        cout << "Value " << oldVal << " not found in list!\n";
    }
}

// DeleteNode
void DeleteNode(nodeType*& headList, int value) {
    nodeType* temp = headList;

    // Search for the node
    while (temp != NULL && temp->data != value)
        temp = temp->next;

    if (temp == NULL) {
        cout << "Value " << value << " not found in list!\n";
        return;
    }

    if (temp->prev != NULL)
        temp->prev->next = temp->next;
    else
        headList = temp->next; // deleting head

    if (temp->next != NULL)
        temp->next->prev = temp->prev;

    free(temp);
    cout << "Deleted node with value " << value << ".\n";
}

// SortNode ascending (bubble sort)
void SortNode(nodeType* headList) {
    if (headList == NULL) return;
    for (nodeType* i = headList; i != NULL; i = i->next) {
        for (nodeType* j = i->next; j != NULL; j = j->next) {
            if (i->data > j->data) {
                int temp = i->data;
                i->data = j->data;
                j->data = temp;
            }
        }
    }
    cout << "List sorted in ascending order.\n";
}

// SortNode descending
void SortNodeDesc(nodeType* headList) {
    if (headList == NULL) return;
    for (nodeType* i = headList; i != NULL; i = i->next) {
        for (nodeType* j = i->next; j != NULL; j = j->next) {
            if (i->data < j->data) {
                int temp = i->data;
                i->data = j->data;
                j->data = temp;
            }
        }
    }
    cout << "List sorted in descending order.\n";
}

// Reverse the list
void ReverseList(nodeType*& headList) {
    nodeType* temp = NULL;
    nodeType* curr = headList;
    while (curr != NULL) {
        temp = curr->prev;
        curr->prev = curr->next;
        curr->next = temp;
        curr = curr->prev;
    }
    if (temp != NULL)
        headList = temp->prev;
    cout << "List reversed.\n";
}

// FreeList (clear all)
void FreeList(nodeType*& headList) {
    nodeType* temp;
    while (headList != NULL) {
        temp = headList;
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
        fout << temp->data << " ";
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
    cout << "  3.  SearchNode\n";
    cout << "  4.  InsertNode (at end)\n";
    cout << "  5.  InsertNode (at position)\n";
    cout << "  6.  UpdateNode\n";
    cout << "  7.  DeleteNode\n";
    cout << "  8.  SortNode (ascending)\n";
    cout << "  9.  SortNode (descending)\n";
    cout << " 10.  Reverse list\n";
    cout << " 11.  Save to file\n";
    cout << " 12.  Load from file\n";
    cout << " 13.  FreeList (clear)\n";
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

    cout << "========================================\n";
    cout << "     DOUBLY LINKED LIST PROGRAM (DLL)\n";
    cout << "========================================\n";

    // --- Input Phase ---
    cout << "\nHow many nodes do you want to enter? ";
    while (!(cin >> n) || n <= 0) {
        cin.clear();
        cin.ignore(1000, '\n');
        cout << "Please enter a positive integer: ";
    }

    cout << "\nEnter " << n << " values:\n";
    for (int i = 0; i < n; i++) {
        cout << "  Node [" << (i + 1) << "]: ";
        while (!(cin >> val)) {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "  Invalid input. Node [" << (i + 1) << "]: ";
        }
        InsertNode(headList, val);
    }

    cout << "\nInitial list: ";
    Traverse(headList);

    // --- Menu Phase ---
    do {
        printMenu();
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

            case 3: {
                cout << "Value to search: ";
                cin >> val;
                nodeType* found = SearchNode(headList, val);
                if (found)
                    cout << "Found: " << found->data << " is in the list.\n";
                else
                    cout << "Value " << val << " not found.\n";
                break;
            }

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
                SortNodeDesc(headList);
                cout << "List: ";
                Traverse(headList);
                break;

            case 10:
                ReverseList(headList);
                cout << "List: ";
                Traverse(headList);
                break;

            case 11:
                WriteFile(headList);
                break;

            case 12: {
                nodeType* loaded = ReadFile();
                if (loaded != NULL) {
                    FreeList(headList);
                    headList = loaded;
                    cout << "List: ";
                    Traverse(headList);
                }
                break;
            }

            case 13:
                FreeList(headList);
                break;

            case 0:
                cout << "Goodbye!\n";
                break;

            default:
                cout << "Invalid choice. Please enter 0-13.\n";
        }

    } while (choice != 0);

    FreeList(headList);
    return 0;
}