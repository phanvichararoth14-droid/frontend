#include <iostream>
#include <fstream>
using namespace std;

// Node structure
struct Node {
    int data;
    Node* next;
};

// Initialize list
Node* Initialize() {
    return NULL;
}

// Create new node
Node* GetNode(int value) {
    Node* p = new Node;
    p->data = value;
    p->next = NULL;
    return p;
}

// Free node
void FreeNode(Node* p) {
    delete p;
}

// Insert at end
void InsertNode(Node*& head, int value) {
    Node* p = GetNode(value);
    if (head == NULL) {
        head = p;
    } else {
        Node* temp = head;
        while (temp->next != NULL)
            temp = temp->next;
        temp->next = p;
    }
}

// Insert at a specific position (0 = head)
void InsertAtPosition(Node*& head, int value, int pos) {
    Node* p = GetNode(value);
    if (pos <= 0 || head == NULL) {
        p->next = head;
        head = p;
        return;
    }
    Node* temp = head;
    for (int i = 0; i < pos - 1 && temp->next != NULL; i++)
        temp = temp->next;
    p->next = temp->next;
    temp->next = p;
}

// Traverse (display)
void Traverse(Node* head) {
    if (head == NULL) {
        cout << "List is empty.\n";
        return;
    }
    Node* temp = head;
    while (temp != NULL) {
        cout << "[" << temp->data << "]";
        if (temp->next != NULL) cout << " -> ";
        temp = temp->next;
    }
    cout << " -> NULL\n";
}

// Count nodes
int CountNode(Node* head) {
    int count = 0;
    while (head != NULL) {
        count++;
        head = head->next;
    }
    return count;
}

// Search node
Node* Search(Node* head, int key) {
    while (head != NULL) {
        if (head->data == key)
            return head;
        head = head->next;
    }
    return NULL;
}

// Update node
void UpdateNode(Node* head, int oldVal, int newVal) {
    Node* p = Search(head, oldVal);
    if (p != NULL) {
        p->data = newVal;
        cout << "Updated " << oldVal << " -> " << newVal << " successfully.\n";
    } else {
        cout << "Value " << oldVal << " not found in list!\n";
    }
}

// Delete node by value
void DelNode(Node*& head, int value) {
    Node *temp = head, *prev = NULL;
    if (temp != NULL && temp->data == value) {
        head = temp->next;
        delete temp;
        cout << "Deleted node with value " << value << ".\n";
        return;
    }
    while (temp != NULL && temp->data != value) {
        prev = temp;
        temp = temp->next;
    }
    if (temp == NULL) {
        cout << "Value " << value << " not found in list!\n";
        return;
    }
    prev->next = temp->next;
    delete temp;
    cout << "Deleted node with value " << value << ".\n";
}

// Sort list ascending (bubble sort on data)
void SortAscending(Node* head) {
    if (head == NULL) return;
    for (Node* i = head; i != NULL; i = i->next) {
        for (Node* j = i->next; j != NULL; j = j->next) {
            if (i->data > j->data) {
                int temp = i->data;
                i->data = j->data;
                j->data = temp;
            }
        }
    }
    cout << "List sorted in ascending order.\n";
}

// Sort list descending
void SortDescending(Node* head) {
    if (head == NULL) return;
    for (Node* i = head; i != NULL; i = i->next) {
        for (Node* j = i->next; j != NULL; j = j->next) {
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
void ReverseList(Node*& head) {
    Node *prev = NULL, *curr = head, *next = NULL;
    while (curr != NULL) {
        next = curr->next;
        curr->next = prev;
        prev = curr;
        curr = next;
    }
    head = prev;
    cout << "List reversed.\n";
}

// Free entire list
void FreeList(Node*& head) {
    Node* temp;
    while (head != NULL) {
        temp = head;
        head = head->next;
        delete temp;
    }
    cout << "List cleared.\n";
}

// Save to file
void WriteFile(Node* head) {
    ofstream fout("data.txt");
    if (!fout) {
        cout << "Error opening file for writing!\n";
        return;
    }
    Node* temp = head;
    while (temp != NULL) {
        fout << temp->data << " ";
        temp = temp->next;
    }
    fout.close();
    cout << "List saved to data.txt\n";
}

// Read from file
Node* ReadFile() {
    ifstream fin("data.txt");
    if (!fin) {
        cout << "Error: data.txt not found!\n";
        return NULL;
    }
    Node* head = NULL;
    int x;
    while (fin >> x)
        InsertNode(head, x);
    fin.close();
    cout << "List loaded from data.txt\n";
    return head;
}

// Print separator
void printLine() {
    cout << "----------------------------------------\n";
}

// Print menu
void printMenu() {
    printLine();
    cout << "  LINKED LIST MENU\n";
    printLine();
    cout << "  1. Display list\n";
    cout << "  2. Count nodes\n";
    cout << "  3. Search value\n";
    cout << "  4. Insert node (at end)\n";
    cout << "  5. Insert node (at position)\n";
    cout << "  6. Update node\n";
    cout << "  7. Delete node\n";
    cout << "  8. Sort ascending\n";
    cout << "  9. Sort descending\n";
    cout << " 10. Reverse list\n";
    cout << " 11. Save to file\n";
    cout << " 12. Load from file\n";
    cout << " 13. Clear list\n";
    cout << "  0. Exit\n";
    printLine();
    cout << "  Choice: ";
}
int main() {
    Node* head = Initialize();
    int n, val, choice;

    cout << "========================================\n";
    cout << "       LINKED LIST PROGRAM (C++)\n";
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
        InsertNode(head, val);
    }

    cout << "\nInitial list: ";
    Traverse(head);

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
                Traverse(head);
                break;

            case 2:
                cout << "Number of nodes: " << CountNode(head) << "\n";
                break;

            case 3: {
                cout << "Value to search: ";
                cin >> val;
                Node* found = Search(head, val);
                if (found)
                    cout << "Found: " << found->data << " is in the list.\n";
                else
                    cout << "Value " << val << " not found.\n";
                break;
            }

            case 4:
                cout << "Value to insert at end: ";
                cin >> val;
                InsertNode(head, val);
                cout << "Inserted " << val << " at end.\n";
                cout << "List: ";
                Traverse(head);
                break;

            case 5: {
                int pos;
                cout << "Value to insert: ";
                cin >> val;
                cout << "Position (0 = head): ";
                cin >> pos;
                InsertAtPosition(head, val, pos);
                cout << "Inserted " << val << " at position " << pos << ".\n";
                cout << "List: ";
                Traverse(head);
                break;
            }

            case 6: {
                int oldVal, newVal;
                cout << "Old value: ";
                cin >> oldVal;
                cout << "New value: ";
                cin >> newVal;
                UpdateNode(head, oldVal, newVal);
                cout << "List: ";
                Traverse(head);
                break;
            }

            case 7:
                cout << "Value to delete: ";
                cin >> val;
                DelNode(head, val);
                cout << "List: ";
                Traverse(head);
                break;

            case 8:
                SortAscending(head);
                cout << "List: ";
                Traverse(head);
                break;

            case 9:
                SortDescending(head);
                cout << "List: ";
                Traverse(head);
                break;

            case 10:
                ReverseList(head);
                cout << "List: ";
                Traverse(head);
                break;

            case 11:
                WriteFile(head);
                break;

            case 12: {
                Node* loaded = ReadFile();
                if (loaded != NULL) {
                    FreeList(head);
                    head = loaded;
                    cout << "List: ";
                    Traverse(head);
                }
                break;
            }

            case 13:
                FreeList(head);
                break;

            case 0:
                cout << "Goodbye!\n";
                break;

            default:
                cout << "Invalid choice. Please enter 0-13.\n";
        }

    } while (choice != 0);

    FreeList(head);
    return 0;
}