#include<iostream>
#include<fstream>
using namespace std;

struct nodeType{
    int info;
    nodeType *next;
};

nodeType* initializeList(){
    return nullptr;
}

nodeType* getNode(){
    nodeType *p = new nodeType;
    return p;
}

void freeNode(nodeType *p){
    delete p;
}

nodeType* createList(nodeType *plist, int n){
    nodeType *p, *ptr;
    int node;
    cout<<"Enter node 0: "; cin>>node;
    p = getNode(); p->info = node; p->next = nullptr;
    plist = p; ptr = plist;
    for(int i=1;i<n;i++){
        cout<<"Enter node "<<i<<": "; cin>>node;
        p = getNode(); p->info = node; p->next = nullptr;
        ptr->next = p; ptr = p;
    }
    return plist;
}

void traverse(nodeType *plist){
    nodeType *p = plist;
    cout<<"List: ";
    while(p!=nullptr){ cout<<p->info<<" -> "; p=p->next; }
    cout<<"NULL\n";
}

int countNode(nodeType *plist){
    int count = 0;
    for(nodeType *p=plist;p!=nullptr;p=p->next) count++;
    return count;
}

nodeType* searchPos(nodeType *plist, int node){
    for(nodeType *p=plist;p!=nullptr;p=p->next)
        if(p->info==node) return p;
    return nullptr;
}

void sequentialSearch(nodeType *plist, int node){
    int pos = 0;
    for(nodeType *p=plist;p!=nullptr;p=p->next){
        if(p->info==node){ cout<<"Found '"<<node<<"' at position "<<pos<<".\n"; return; }
        pos++;
    }
    cout<<"Not found!\n";
}

void binarySearch(nodeType *plist, int node){
    int n = countNode(plist);
    int arr[100];
    int i = 0;
    for(nodeType *p=plist;p!=nullptr;p=p->next) arr[i++]=p->info;
    for(int a=0;a<n-1;a++)
        for(int b=0;b<n-a-1;b++)
            if(arr[b]>arr[b+1]){ int tmp=arr[b]; arr[b]=arr[b+1]; arr[b+1]=tmp; }
    int left=0, right=n-1;
    while(left<=right){
        int mid=(left+right)/2;
        if(arr[mid]==node){ cout<<"Found '"<<node<<"' at position "<<mid<<" (sorted array).\n"; return; }
        else if(arr[mid]<node) left=mid+1;
        else right=mid-1;
    }
    cout<<"Not found!\n";
}

void sortNode(nodeType *plist){
    int tmp;
    for(nodeType *p=plist;p!=nullptr;p=p->next)
        for(nodeType *ptr=p->next;ptr!=nullptr;ptr=ptr->next)
            if(p->info>ptr->info){ tmp=p->info; p->info=ptr->info; ptr->info=tmp; }
}

nodeType* insertNode(nodeType *plist, int node){
    nodeType *p = getNode(); p->info = node; p->next = nullptr;
    if(plist==nullptr){ plist=p; }
    else{ nodeType *ptr=plist; while(ptr->next!=nullptr) ptr=ptr->next; ptr->next=p; }
    return plist;
}

bool updateNode(nodeType *plist, int oldNode, int newNode){
    nodeType *target = searchPos(plist, oldNode);
    if(target==nullptr) return false;
    target->info = newNode;
    return true;
}

nodeType* deleteNode(nodeType *plist, int node){
    nodeType *p=plist, *prev=nullptr;
    while(p!=nullptr && p->info!=node){ prev=p; p=p->next; }
    if(p==nullptr){ cout<<"Node not found!\n"; return plist; }
    if(prev==nullptr) plist=p->next;
    else prev->next=p->next;
    freeNode(p);
    return plist;
}

void saveToFile(nodeType *plist){
    ofstream f("list1.txt");
    for(nodeType *p=plist;p!=nullptr;p=p->next) f<<p->info<<" ";
    f.close();
}

nodeType* loadFromFile(nodeType *plist){
    ifstream f("list1.txt");
    int node;
    while(f>>node) plist=insertNode(plist,node);
    f.close();
    return plist;
}

int main(){
    nodeType *plist = initializeList();
    int n, option;
    cout<<"GROUP 12\n";
    cout<<" - Bros Sophary\n - Phan Vichararoth\n - Maeng Chansreyha\n";
    do{
        cout<<"===== MENU =====\n";
        cout<<"1. Create\n2. Insert\n3. Display\n4. Search\n5. Sort\n6. Delete\n7. Update\n8. Count\n9. Save\n10. Load\n0. Exit\n";
        cout<<"Choose Option: "; cin>>option;
        switch(option){
            case 1:{
                cout<<"Enter number of nodes: "; cin>>n;
                plist=createList(plist,n);
                cout<<"List created.\n";
                break;
            }
            case 2:{
                int node;
                cout<<"Enter node: "; cin>>node;
                plist=insertNode(plist,node);
                cout<<"Inserted successfully.\n";
                break;
            }
            case 3:{
                traverse(plist);
                break;
            }
            case 4:{
                int node;
                int s;
                cout<<"--- Search Menu ---\n";
                cout<<"1. Sequential Search\n2. Binary Search\n";
                cout<<"Choose: "; cin>>s;
                cout<<"Enter node: "; cin>>node;
                if(s==1) sequentialSearch(plist,node);
                else if(s==2) binarySearch(plist,node);
                else cout<<"Invalid choice!\n";
                break;
            }
            case 5:{
                sortNode(plist);
                cout<<"Sorting complete (Bubble Sort).\n";
                break;
            }
            case 6:{
                int node;
                cout<<"Enter node to delete: "; cin>>node;
                plist=deleteNode(plist,node);
                cout<<"Deleted successfully.\n";
                break;
            }
            case 7:{
                int oldNode, newNode;
                cout<<"Enter node to update: "; cin>>oldNode;
                cout<<"Enter new node: "; cin>>newNode;
                if(updateNode(plist,oldNode,newNode)) cout<<"Updated successfully.\n";
                else cout<<"Node not found!\n";
                break;
            }
            case 8:{
                cout<<"Total nodes: "<<countNode(plist)<<"\n";
                break;
            }
            case 9:{
                saveToFile(plist);
                cout<<"Data saved to file.\n";
                break;
            }
            case 10:{
                plist=loadFromFile(initializeList());
                cout<<"Data loaded from file.\n";
                traverse(plist);
                break;
            }
            case 0:{ break; }
            default:{ cout<<"Invalid option!\n"; break; }
        }
    }while(option!=0);
    return 0;
}