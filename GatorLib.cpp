#include <iostream>
#include <string>
#include <ctime>
#include <queue>
#include <limits>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <tuple>
#include <variant>
#include <chrono>

#define HeapNode tuple<int, double, int>
#define WAIT_LIST_MAX_SIZE 20
#define MAX_INT numeric_limits<int>::max()

using namespace std;

class BookNode {
public:
    int BookID;
    string BookName;
    string AuthorName;
    string AvailabilityStatus;
    int BorrowedBy;
    priority_queue<HeapNode, vector<HeapNode>, greater<HeapNode>> ReservationHeap;

    BookNode() : BookID(0), BookName(), AuthorName(), AvailabilityStatus("Yes"), BorrowedBy(-1) {

    }

    BookNode(int bookID, string bookName, string authorName, string availabilityStatus, int borrowedBy)
        : BookID(bookID), BookName(bookName), AuthorName(authorName), AvailabilityStatus(availabilityStatus), BorrowedBy(borrowedBy) {

    }

    int getKey() const {
        return BookID;
    }

    friend ostream& operator<<(ostream& os, const BookNode& obj) {
        auto pq = obj.ReservationHeap;

        os << "BookID = " << obj.BookID << endl;
        os << "Title = \"" << obj.BookName << "\"" << endl;
        os << "Author = \"" << obj.AuthorName << "\"" << endl;
        os << "Availability = \"" << obj.AvailabilityStatus << "\"" << endl;
        variant<int, string> borrowedBy = obj.BorrowedBy == -1 ? variant<int, string>("None") : variant<int, string>(obj.BorrowedBy);

        if (holds_alternative<int>(borrowedBy)) {
            os << "BorrowedBy = " << get<int>(borrowedBy) << endl;
        }
        else if (holds_alternative<string>(borrowedBy)) {
            os << "BorrowedBy = " << get<string>(borrowedBy) << endl;
        }

        os << "Reservations = [";

        while (!pq.empty()) {
            auto [pNum, timeOfReservation, patronID] = pq.top();
            pq.pop();
            os << patronID;
            if(!pq.empty()) os << ", ";
        }

        os << "]" << endl;

        return os;
    }

    bool operator==(const BookNode& other) const {
        return BookID == other.BookID;
    }

    bool operator<(const BookNode& other) const {
        return BookID < other.BookID;
    }

    bool operator>(const BookNode& other) const {
        return BookID > other.BookID;
    }

    bool operator<=(const BookNode& other) const {
        return BookID <= other.BookID;
    }


};
class RBTreeNode {
public:
    BookNode bookData;
    RBTreeNode * parent, * left, * right;
    int color;

    RBTreeNode() : bookData(), parent(nullptr), left(nullptr), right(nullptr), color(0) {
    
    }

    RBTreeNode(int bookID, string bookName, string authorName, string availabilityStatus, int borrowedBy, int color = 0) 
        :  RBTreeNode() {
        bookData = BookNode(bookID, bookName, authorName, availabilityStatus, borrowedBy);
        this->color = color;
    }

    RBTreeNode(int color) : RBTreeNode() {
        this->color = color;
    }
};

class RBTree {
private:
    RBTreeNode *root, *null;

    RBTreeNode* _search(RBTreeNode* node, int key) {
        if (node == null || key == node->bookData.getKey()) {
            return node;
        }

        if (key < node->bookData.getKey()) {
            return _search(node->left, key);
        }

        return _search(node->right, key);
    }

    void _findClosestTraverse(vector<RBTreeNode*>& arr, RBTreeNode* root, const int& key, int& minDiff) {
        if (root == nullptr || root == null) {
            return;
        }

        if (abs(root->bookData.getKey() - key) < minDiff) {
            arr.clear();
            arr.push_back(root);
            minDiff = abs(root->bookData.getKey() - key);
        }

        else if (abs(root->bookData.getKey() - key) == minDiff) {
            arr.push_back(root);
            minDiff = abs(root->bookData.getKey() - key);
        }

        _findClosestTraverse(arr, root->left, key, minDiff);
        _findClosestTraverse(arr, root->right, key, minDiff);
    }

    vector<RBTreeNode*> _findClosest(RBTreeNode* root, int key) {
        RBTreeNode* equalNode = _search(root, key);
        
        if (equalNode->bookData.getKey() == key) {
            return { equalNode };
        }

        vector<RBTreeNode*> arr;
        int minDiff = MAX_INT;

        _findClosestTraverse(arr, root, key, minDiff);

        sort(arr.begin(), arr.end(), [](const RBTreeNode* a, const RBTreeNode* b) {
            return a->bookData < b->bookData;
        });

        return arr;
    }

    void _assignColor(RBTreeNode* cur, int newColor) {
        if (cur->color != newColor) colorFlipCount++;
        cur->color = newColor;
    }

    void _transplantRedBlack(RBTreeNode* a, RBTreeNode* b) {
        if (a->parent == nullptr) {
            root = b;
        }

        else if (a == a->parent->left) {
            a->parent->left = b;
        }
        else {
            a->parent->right = b;
        }

        b->parent = a->parent;
    }

    void _balanceDelete(RBTreeNode* cur) {
        RBTreeNode* p;
        while (cur != root && cur->color == 0) {
            if (cur == cur->parent->left) {
                p = cur->parent->right;

                if (p->color == 1) {
                    _assignColor(p, 0);
                    _assignColor(cur->parent, 1);
                    rotateLeft(cur->parent);

                    p = cur->parent->right;
                }

                if (p->left->color == 0 && p->right->color == 0) {
                    _assignColor(p, 1);
                    cur = cur->parent;
                }

                else {
                    if (p->right->color == 0) {
                        _assignColor(p->left, 0);
                        _assignColor(p, 1);
                        rotateRight(p);

                        p = cur->parent->right;
                    }

                    _assignColor(p, cur->parent->color);
                    _assignColor(cur->parent, 0);
                    _assignColor(p->right, 0);
                    rotateLeft(cur->parent);

                    cur = root;
                }
            }

            else {
                p = cur->parent->left;

                if (p->color == 1) {
                    _assignColor(p, 0);
                    _assignColor(cur->parent, 1);
                    rotateRight(cur->parent);

                    p = cur->parent->left;
                }

                if (p->right->color == 0) {
                    _assignColor(p, 1);

                    cur = cur->parent;
                }

                else {
                    if (p->left->color == 0) {
                        _assignColor(p->right, 0);
                        _assignColor(p, 1);
                        rotateLeft(p);

                        p = cur->parent->left;
                    }

                    _assignColor(p, cur->parent->color);
                    _assignColor(cur->parent, 0);
                    _assignColor(p->left, 0);
                    rotateRight(cur->parent);

                    cur = root;
                }
            }
        }

        _assignColor(cur, 0);
    }

    void _nodeDelete(RBTreeNode* node, int key) {
        RBTreeNode *delNode = null, *a, *b;

        while (node != null) {
            if (node->bookData.getKey() == key) {
                delNode = node;
            }

            if (node->bookData.getKey() <= key) {
                node = node->right;
            }
            else {
                node = node->left;
            }
        }

        if (delNode == null) {
            cout << "node doesn't exist" << endl;
            return;
        }

        b = delNode;
        int b_original_color = b->color;

        if (delNode->left == null) {
            a = delNode->right;
            _transplantRedBlack(delNode, delNode->right);
        }

        else if (delNode->right == null) {
            a = delNode->left;
            _transplantRedBlack(delNode, delNode->left);
        }

        else {
            b = findMin(delNode->right);
            b_original_color = b->color;
            a = b->right;

            if (b->parent == delNode) {
                a->parent = b;
            }

            else {
                _transplantRedBlack(b, b->right);
                b->right = delNode->right;
                if(b->right) b->right->parent = b;
            }

            _transplantRedBlack(delNode, b);
            b->left = delNode->left;
            if(b->left) b->left->parent = b;
            _assignColor(b, delNode->color);
        }

        delete delNode;
        
        if (b_original_color == 0) {
            _balanceDelete(a);
        }
    }

    void _balanceInsert(RBTreeNode* cur) {
        RBTreeNode *a;

        while (cur->parent->color == 1) {

            if (cur->parent == cur->parent->parent->right) {
                a = cur->parent->parent->left;

                if (a->color == 1) {
                    _assignColor(a, 0);
                    _assignColor(cur->parent, 0);
                    _assignColor(cur->parent->parent, 1);
                    cur = cur->parent->parent;
                }

                else {
                    if (cur == cur->parent->left) {
                        cur = cur->parent;
                        rotateRight(cur);
                    }

                    _assignColor(cur->parent, 0);
                    _assignColor(cur->parent->parent, 1);
                    rotateLeft(cur->parent->parent);
                }
            }

            else {
                a = cur->parent->parent->right;

                if (a->color == 1) {
                    _assignColor(a, 0);
                    _assignColor(cur->parent, 0);
                    _assignColor(cur->parent->parent, 1);
                    cur = cur->parent->parent;
                }

                else {
                    if (cur == cur->parent->right) {
                        cur = cur->parent;
                        rotateLeft(cur);
                    }

                    _assignColor(cur->parent, 0);
                    _assignColor(cur->parent->parent, 1);
                    rotateRight(cur->parent->parent);
                }
            }

            if (cur == root) {
                break;
            }
        }
        _assignColor(root, 0);
    }


public:
    int colorFlipCount = 0;

    RBTree() : null(new RBTreeNode(0)), colorFlipCount(0) {
        root = null;
    }

    void postOrderDelete(RBTreeNode* node) {
        if (node == nullptr || node == null) {
            return;
        }
        postOrderDelete(node->left);
        postOrderDelete(node->right);
        delete node;
    }

    ~RBTree() {
        postOrderDelete(root);
        delete null;
    }

    RBTreeNode* getRoot() const {
        return this->root;
    }

    RBTreeNode* getNull() const {
        return this->null;
    }

    RBTreeNode* search(int key) {
        return _search(root, key);
    }

    vector<RBTreeNode*> findClosest(int key) {
        return _findClosest(root, key);
    }

    RBTreeNode* findMax(RBTreeNode* node) {
        for (; node->right != null; node = node->right);
        return node;
    }

    RBTreeNode* findMin(RBTreeNode* node) {
        for (; node->left != null; node = node->left);
        return node;
    }

    void insert(const BookNode& bookData) {
        RBTreeNode* node = new RBTreeNode();

        node->parent = nullptr;
        node->bookData = bookData;
        node->left = null;
        node->right = null;
        node->color = 1;

        RBTreeNode *b = nullptr, *a = this->root;

        while (a != null) {
            b = a;

            if (node->bookData < a->bookData) {
                a = a->left;
            }

            else {
                a = a->right;
            }
        }

        node->parent = b;

        if (b == nullptr) {
            root = node;
        }

        else if (node->bookData < b->bookData) {
            b->left = node;
        }

        else {
            b->right = node;
        }

        if (node->parent == nullptr) {
            node->color = 0;
            return;
        }

        if (node->parent->parent == nullptr) {
            return;
        }

        _balanceInsert(node);
    }

    void rotateLeft(RBTreeNode* cur) {

        RBTreeNode* b = cur->right;
        cur->right = b->left;

        if (b->left != null) {
            b->left->parent = cur;
        }

        b->parent = cur->parent;

        if (cur->parent == nullptr) {
            this->root = b;
        }

        else if (cur == cur->parent->left) {
            cur->parent->left = b;
        }

        else {
            cur->parent->right = b;
        }

        b->left = cur;
        cur->parent = b;
    }

    void rotateRight(RBTreeNode* cur) {
        RBTreeNode* b = cur->left;

        cur->left = b->right;

        if (b->right != null) {
            b->right->parent = cur;
        }

        b->parent = cur->parent;

        if (cur->parent == nullptr) {
            this->root = b;
        }

        else if (cur == cur->parent->right) {
            cur->parent->right = b;
        }

        else {
            cur->parent->left = b;
        }

        b->right = cur;
        cur->parent = b;
    }

    void nodeDelete(int key) {
        _nodeDelete(this->root, key);
    }

    void printInRange(RBTreeNode* root, int l, int r, ofstream &os) {
        if (root == nullptr || root == null) return;

        if (root->bookData.BookID > l) {
            printInRange(root->left, l, r, os);
        }

        if (root->bookData.BookID >= l && root->bookData.BookID <= r) {
            cout << root->bookData << endl;
            os << root->bookData << endl;
        }

        if (root->bookData.BookID < r) {
            printInRange(root->right, l, r, os);
        }
    }
};

class GatorLibrary {
private:
    RBTree tree;
public:
    void PrintBook(int bookID, ofstream &os) {
        RBTreeNode* node = tree.search(bookID);

        if (node != tree.getNull()) {
            cout << node->bookData << endl;
            os << node->bookData << endl;
            return;
        }

        cout << "Book " << bookID << " not found in the Library" << endl;
        os << "Book " << bookID << " not found in the Library" << endl;
    }

    void PrintBooks(int bookID1, int bookID2, ofstream &os) {
        tree.printInRange(tree.getRoot(), bookID1, bookID2, os);
    }

    void InsertBook(int bookID, string bookName, string authorName, string availabilityStatus, int borrowedBy, ofstream &os) {
        BookNode book(
            bookID,
            bookName,
            authorName,
            availabilityStatus,
            borrowedBy
        );

        tree.insert(book);
    }

    void BorrowBook(int patronID, int bookID, int patronPriority, ofstream &os) {
        RBTreeNode* node = tree.search(bookID);

        if (node) {
            if (node->bookData.AvailabilityStatus == "Yes") {
                node->bookData.AvailabilityStatus = "No";
                node->bookData.BorrowedBy = patronID;

                cout << "Book " << bookID << " Borrowed by Patron " << patronID << endl;
                os << "Book " << bookID << " Borrowed by Patron " << patronID << endl;
            }

            else {
                if (node->bookData.ReservationHeap.size() < 20) {
                    auto now = chrono::system_clock::now();
                    auto duration = now.time_since_epoch();
                    auto millis = chrono::duration_cast<chrono::milliseconds>(duration).count();
                    cout << millis << endl;

                    node->bookData.ReservationHeap.push({ patronPriority, millis, patronID});
                    cout << "Book " << bookID << " Reserved by Patron " << patronID << endl;
                    os << "Book " << bookID << " Reserved by Patron " << patronID << endl;
                }
            }
        }
    }

    void ReturnBook(int patronID, int bookID, ofstream &os) {
        RBTreeNode* node = tree.search(bookID);

        if (node) {

            if (node->bookData.BorrowedBy != patronID) {
                cout << "Book " << bookID << " Reserved by Patron " << node->bookData.BorrowedBy << endl;
                os << "Book " << bookID << " Reserved by Patron " << node->bookData.BorrowedBy << endl;
            }


            else {
                node->bookData.AvailabilityStatus = "Yes";
                node->bookData.BorrowedBy = -1;

                cout << "Book " << bookID << " Returned by Patron " << patronID << endl;
                os << "Book " << bookID << " Returned by Patron " << patronID << endl;

                if (!node->bookData.ReservationHeap.empty()) {
                    auto [patronPriority, timeOfReservation, nextPatronID] = node->bookData.ReservationHeap.top();
                    node->bookData.ReservationHeap.pop();

                    node->bookData.AvailabilityStatus = "No";
                    node->bookData.BorrowedBy = nextPatronID;

                    cout << endl << "Book " << bookID << " Allotted to Patron " << nextPatronID << endl;
                    os << endl << "Book " << bookID << " Allotted to Patron " << nextPatronID << endl;
                }
            }
        }
    }

    void DeleteBook(int bookID, ofstream &os) {
        RBTreeNode* node = tree.search(bookID);

        if (node) {
            if (node->bookData.ReservationHeap.empty()) {
                cout << "Book " << bookID << " is no longer available" << endl;
                os << "Book " << bookID << " is no longer available" << endl;
            }

            else {
                cout << "Book " << bookID << " is no longer available. ";  
                os << "Book " << bookID << " is no longer available. ";
                
                bool plural = node->bookData.ReservationHeap.size() > 1;

                if (plural) {
                    cout << "Reservations made by Patrons ";
                    os << "Reservations made by Patrons ";
                }

                else {
                    cout << "Reservation made by Patron ";
                    os << "Reservation made by Patron ";
                }
                while (!node->bookData.ReservationHeap.empty()) {
                    auto [patronPriority, timeOfReservation, nextPatronID] = node->bookData.ReservationHeap.top();
                    node->bookData.ReservationHeap.pop();

                    cout << nextPatronID;
                    os << nextPatronID;

                    if (!node->bookData.ReservationHeap.empty()) {
                        cout << ", ";
                        os << ", ";
                    }
                }
                if (plural) {
                    cout << " have been cancelled!" << endl;
                    os << " have been cancelled!" << endl;
                }

                else {
                    cout << " has been cancelled!" << endl;
                    os << " has been cancelled!" << endl;
                }
                
            }

            tree.nodeDelete(bookID);
        }
    }

    void FindClosestBook(int targetID, ofstream &os) {
        vector<RBTreeNode*> res = tree.findClosest(targetID);

        for (auto& node : res) {
            cout << node->bookData << endl;
            os << node->bookData << endl;
        }
    }

    void ColorFlipCount(ofstream &os) {
        cout << "Colour Flip Count: " << tree.colorFlipCount << endl;
        os << "Colour Flip Count: " << tree.colorFlipCount << endl;
    }

    static string trim(const string& str) {
        string result = str;

        result.erase(result.begin(), find_if(result.begin(), result.end(), [](unsigned char ch) {
            return !isspace(ch);
            }));

        result.erase(find_if(result.rbegin(), result.rend(), [](unsigned char ch) {
            return !isspace(ch);
            }).base(), result.end());

        return result;
    }

    static vector<string> tokenize(istringstream& ss) {
        string token;
        vector<string> tokens;

        while (getline(ss, token, ',')) {
            tokens.push_back(trim(token));
        }

        return tokens;
    }

    void  ExecuteOperations(const vector<string>& operations, ofstream& os)
    {
        for (string operation : operations) {
            size_t n = operation.size();
            string command;
            size_t i = 0;

            for (; i < n && operation[i] != '('; i++) {
                command.push_back(operation[i]);
            }

            if (command == "InsertBook") {

                operation.pop_back();
                istringstream paramsStream(operation.substr(i + 1));

                vector<string> tokens = tokenize(paramsStream);;

                if (tokens.size() >= 4) {
                    int bookID = stoi(tokens[0]);
                    string bookName = tokens[1].substr(1, tokens[1].size() - 2);
                    string authorName = tokens[2].substr(1, tokens[2].size() - 2);
                    string availabilityStatus; // = tokens[3].substr(1, tokens[3].size() - 2);
                    
                    for (size_t i = 1; i < tokens[3].size() && tokens[3][i] != '"'; i++) {
                        availabilityStatus.push_back(tokens[3][i]);
                    }

                    InsertBook(bookID, bookName, authorName, availabilityStatus, -1, os);
                }

                else {
                    cout << command << ": Invalid number of parameters for " << command << endl;
                }
            }

            else if (command == "PrintBook") {
                operation.pop_back();
                istringstream paramsStream(operation.substr(i + 1));

                vector<string> tokens = tokenize(paramsStream);;

                if (tokens.size() == 1) {
                    int bookID = stoi(tokens[0]);

                    PrintBook(bookID, os);
                }

                else {
                    cout << command << ": Invalid number of parameters for " << command <<  endl;
                    exit(1);
                }
            }

            else if (command == "PrintBooks") {
                operation.pop_back();
                istringstream paramsStream(operation.substr(i + 1));

                vector<string> tokens = tokenize(paramsStream);;

                if (tokens.size() == 2) {
                    int bookID1 = stoi(tokens[0]);
                    int bookID2 = stoi(tokens[1]);

                    PrintBooks(bookID1, bookID2, os);
                }

                else {
                    cout << command << ": Invalid number of parameters for " << command << endl;
                    exit(1);
                }
            }

            else if (command == "BorrowBook") {
                operation.pop_back();
                istringstream paramsStream(operation.substr(i + 1));

                vector<string> tokens = tokenize(paramsStream);;

                if (tokens.size() == 3) {
                    int patronID = stoi(tokens[0]);
                    int bookID = stoi(tokens[1]);
                    int patronPriority = stoi(tokens[2]);

                    BorrowBook(patronID, bookID, patronPriority, os);
                }

                else {
                    cout << command << ": Invalid number of parameters for " << command << endl;
                    exit(1);
                }
            }

            else if (command == "ReturnBook") {
                operation.pop_back();
                istringstream paramsStream(operation.substr(i + 1));

                vector<string> tokens = tokenize(paramsStream);;

                if (tokens.size() == 2) {
                    int patronID = stoi(tokens[0]);
                    int bookID = stoi(tokens[1]);

                    ReturnBook(patronID, bookID, os);
                }

                else {
                    cout << command << ": Invalid number of parameters for " << command << endl;
                    exit(1);
                }
            }

            else if (command == "DeleteBook") {
                operation.pop_back();
                istringstream paramsStream(operation.substr(i + 1));

                vector<string> tokens = tokenize(paramsStream);;

                if (tokens.size() == 1) {
                    int bookID = stoi(tokens[0]);

                    DeleteBook(bookID, os);
                }

                else {
                    cout << command << ": Invalid number of parameters for " << command << endl;
                    exit(1);
                }
            }

            else if (command == "FindClosestBook") {
                operation.pop_back();
                istringstream paramsStream(operation.substr(i + 1));

                vector<string> tokens = tokenize(paramsStream);;

                if (tokens.size() == 1) {
                    int targetID = stoi(tokens[0]);

                    FindClosestBook(targetID, os);
                }

                else {
                    cout << command << ": Invalid number of parameters for " << command << endl;
                    exit(1);
                }
            }

            else if (command == "ColorFlipCount") {
                operation.pop_back();
                istringstream paramsStream(operation.substr(i + 1));

                vector<string> tokens = tokenize(paramsStream);;

                ColorFlipCount(os);
            }

            else if (command == "Quit") {
                cout << "Program Terminated!!" << endl;
                os << "Program Terminated!!" << endl;
                exit(1);
            }

            else {
                cout << "Invalid command" << endl;
                exit(1);
            }

            cout << endl;
            os << endl;
        }
    }
};

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cerr << "Usage: ./gatorLibrary <file_name>" << endl;
        return 1;
    }

    string inputFileName = argv[1];
    ifstream inputFile(inputFileName);

    if (!inputFile.is_open()) {
        cerr << "Error opening input file. " << inputFileName << endl;
        return 1;
    }

    ofstream outputFile(inputFileName + "_output_file.txt");

    if(!outputFile.is_open()) {
        cerr << "Error opening output file." << endl;
        return 1;
    }

    GatorLibrary library;

    string command;
    vector<string> operations;

    while (getline(inputFile, command)) {
        operations.push_back(command);
    }

    library.ExecuteOperations(operations, outputFile);

    

    return 0;
}