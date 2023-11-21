// GatorLib.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

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

    friend std::ostream& operator<<(std::ostream& os, const BookNode& obj) {
        auto pq = obj.ReservationHeap;

        os << "BookID = " << obj.BookID << std::endl;
        os << "Title = \"" << obj.BookName << "\"" << std::endl;
        os << "Author = \"" << obj.AuthorName << "\"" << std::endl;
        os << "Availability = \"" << obj.AvailabilityStatus << "\"" << std::endl;
        std::variant<int, std::string> borrowedBy = obj.BorrowedBy == -1 ? std::variant<int, std::string>("None") : std::variant<int, std::string>(obj.BorrowedBy);

        if (std::holds_alternative<int>(borrowedBy)) {
            os << "BorrowedBy = " << std::get<int>(borrowedBy) << std::endl;
        }
        else if (std::holds_alternative<std::string>(borrowedBy)) {
            os << "BorrowedBy = " << std::get<std::string>(borrowedBy) << std::endl;
        }

        os << "Reservations = [";

        while (!pq.empty()) {
            auto [pNum, timeOfReservation, patronID] = pq.top();
            pq.pop();
            os << patronID;
            if(!pq.empty()) os << ", ";
        }

        os << "]" << std::endl;

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
    BookNode data;
    RBTreeNode * parent, * left, * right;
    int color;

    RBTreeNode() : data(), parent(nullptr), left(nullptr), right(nullptr), color(0) {
    
    }

    RBTreeNode(int bookID, string bookName, string authorName, string availabilityStatus, int borrowedBy, int color = 0) 
        :  RBTreeNode() {
        data = BookNode(bookID, bookName, authorName, availabilityStatus, borrowedBy);
        this->color = color;
    }

    RBTreeNode(int color) : RBTreeNode() {
        this->color = color;
    }
};

class RBTree {
private:
    RBTreeNode* root;
    RBTreeNode* null;

    /*void initNullNode(RBTreeNode* node, RBTreeNode* parent) {
        node = new RBTreeNode(0);
        node->parent = parent;
    }*/

    /*void printPreOrder(RBTreeNode* node) {
        if (node != null) {
            cout << node->data << endl;
            printPreOrder(node->left);
            printPreOrder(node->right);
        }
    }*/

    /*void printInOrder(RBTreeNode* node) {
        if (node != null) {
            printInOrder(node->left);
            cout << node->data << endl;
            printInOrder(node->right);
        }
    }*/

    /*void printPostOrder(RBTreeNode* node) {
        if (node != null) {
            printPostOrder(node->left);
            printPostOrder(node->right);
            cout << node->data << endl;
        }
    }*/

    RBTreeNode* searchTreeHelper(RBTreeNode* node, int key) {
        if (node == null || key == node->data.getKey()) {
            return node;
        }

        if (key < node->data.getKey()) {
            return searchTreeHelper(node->left, key);
        }
        return searchTreeHelper(node->right, key);
    }

    void findClosestTraverse(vector<RBTreeNode*>& arr, RBTreeNode* root, const int& key, int& minDiff) {
        if (root == nullptr || root == null) {
            return;
        }

       //if (abs(root->data.getKey() - key) > minDiff) return;

        if (abs(root->data.getKey() - key) < minDiff) {
            arr.clear();
            arr.push_back(root);
            minDiff = abs(root->data.getKey() - key);
        }

        else if (abs(root->data.getKey() - key) == minDiff) {
            arr.push_back(root);
            minDiff = abs(root->data.getKey() - key);
        }

        findClosestTraverse(arr, root->left, key, minDiff);
        findClosestTraverse(arr, root->right, key, minDiff);
    }

    vector<RBTreeNode*> findClosestHelper(RBTreeNode* root, int key) {
        RBTreeNode* equalNode = searchTreeHelper(root, key);
        
        if (equalNode->data.getKey() == key) {
            return { equalNode };
        }

        vector<RBTreeNode*> arr;
        int minDiff = MAX_INT;

        findClosestTraverse(arr, root, key, minDiff);

        sort(arr.begin(), arr.end(), [](const RBTreeNode* a, const RBTreeNode* b) {
            return a->data < b->data;
        });

        return arr;
    }

    void assignColor(RBTreeNode* cur, int newColor) {
        if (cur->color != newColor) colorFlipCount++;
        cur->color = newColor;
    }

    void deleteFix(RBTreeNode* x) {
        RBTreeNode* s;
        while (x != root && x->color == 0) {
            if (x == x->parent->left) {
                s = x->parent->right;
                if (s->color == 1) {
                    //s->color = 0;
                    assignColor(s, 0);
                    //x->parent->color = 1;
                    assignColor(x->parent, 1);
                    leftRotate(x->parent);
                    s = x->parent->right;
                }

                if (s->left->color == 0 && s->right->color == 0) {
                    //s->color = 1;
                    assignColor(s, 1);
                    x = x->parent;
                }
                else {
                    if (s->right->color == 0) {
                        //s->left->color = 0;
                        assignColor(s->left, 0);
                        //s->color = 1;
                        assignColor(s, 1);
                        rightRotate(s);
                        s = x->parent->right;
                    }

                    //s->color = x->parent->color;
                    assignColor(s, x->parent->color);
                    //x->parent->color = 0;
                    assignColor(x->parent, 0);
                    //s->right->color = 0;
                    assignColor(s->right, 0);
                    leftRotate(x->parent);
                    x = root;
                }
            }
            else {
                s = x->parent->left;
                if (s->color == 1) {
                    //s->color = 0;
                    assignColor(s, 0);
                    //x->parent->color = 1;
                    assignColor(x->parent, 1);
                    rightRotate(x->parent);
                    s = x->parent->left;
                }

                if (s->right->color == 0) {
                    //s->color = 1;
                    assignColor(s, 1);
                    x = x->parent;
                }
                else {
                    if (s->left->color == 0) {
                        //s->right->color = 0;
                        assignColor(s->right, 0);
                        //s->color = 1;
                        assignColor(s, 1);
                        leftRotate(s);
                        s = x->parent->left;
                    }

                    //s->color = x->parent->color;
                    assignColor(s, x->parent->color);
                    //x->parent->color = 0;
                    assignColor(x->parent, 0);
                    //s->left->color = 0;
                    assignColor(s->left, 0);
                    rightRotate(x->parent);
                    x = root;
                }
            }
        }
        //x->color = 0;
        assignColor(x, 0);
    }

    void rbTransplant(RBTreeNode* u, RBTreeNode* v) {
        if (u->parent == nullptr) {
            root = v;
        }
        else if (u == u->parent->left) {
            u->parent->left = v;
        }
        else {
            u->parent->right = v;
        }
        v->parent = u->parent;
    }

    void deleteNodeHelper(RBTreeNode* node, int key) {
        RBTreeNode* z = null;
        RBTreeNode *x, *y;
        while (node != null) {
            if (node->data.getKey() == key) {
                z = node;
            }

            if (node->data.getKey() <= key) {
                node = node->right;
            }
            else {
                node = node->left;
            }
        }

        if (z == null) {
            cout << "Key not found in the tree" << endl;
            return;
        }

        y = z;
        int y_original_color = y->color;
        if (z->left == null) {
            x = z->right;
            rbTransplant(z, z->right);
        }
        else if (z->right == null) {
            x = z->left;
            rbTransplant(z, z->left);
        }
        else {
            y = minimum(z->right);
            y_original_color = y->color;
            x = y->right;
            if (y->parent == z) {
                x->parent = y;
            }
            else {
                rbTransplant(y, y->right);
                y->right = z->right;
                if(y->right) y->right->parent = y;
            }

            rbTransplant(z, y);
            y->left = z->left;
            if(y->left) y->left->parent = y;
            //y->color = z->color;
            assignColor(y, z->color);
        }

        delete z;
        
        if (y_original_color == 0) {
            deleteFix(x);
        }
    }

    void insertFix(RBTreeNode* k) {
        RBTreeNode *u;
        while (k->parent->color == 1) {
            if (k->parent == k->parent->parent->right) {
                u = k->parent->parent->left;
                if (u->color == 1) {
                    //u->color = 0;
                    assignColor(u, 0);
                    //k->parent->color = 0;
                    assignColor(k->parent, 0);
                    //k->parent->parent->color = 1;
                    assignColor(k->parent->parent, 1);
                    k = k->parent->parent;
                }
                else {
                    if (k == k->parent->left) {
                        k = k->parent;
                        rightRotate(k);
                    }
                    //k->parent->color = 0;
                    assignColor(k->parent, 0);
                    //k->parent->parent->color = 1;
                    assignColor(k->parent->parent, 1);
                    leftRotate(k->parent->parent);
                }
            }
            else {
                u = k->parent->parent->right;

                if (u->color == 1) {
                    //u->color = 0;
                    assignColor(u, 0);
                    //k->parent->color = 0;
                    assignColor(k->parent, 0);
                    //k->parent->parent->color = 1;
                    assignColor(k->parent->parent, 1);
                    k = k->parent->parent;
                }
                else {
                    if (k == k->parent->right) {
                        k = k->parent;
                        leftRotate(k);
                    }
                    //k->parent->color = 0;
                    assignColor(k->parent, 0);
                    //k->parent->parent->color = 1;
                    assignColor(k->parent->parent, 1);
                    rightRotate(k->parent->parent);
                }
            }
            if (k == root) {
                break;
            }
        }
        //root->color = 0;
        assignColor(root, 0);
    }

    /*void print(RBTreeNode* root, string indent, bool last) {
        if (root != null) {
            cout << indent;
            if (last) {
                cout << "R----";
                indent += "   ";
            }
            else {
                cout << "L----";
                indent += "|  ";
            }

            string sColor = root->color ? "RED" : "BLACK";
            cout << root->data << "(" << sColor << ")" << endl;
            print(root->left, indent, false);
            print(root->right, indent, true);
        }
    }*/


public:
    int colorFlipCount;

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

    /*void preorder() {
        printPreOrder(this->root);
    }*/

    /*void inorder() {
        printInOrder(this->root);
    }*/

    /*void postorder() {
        printPostOrder(this->root);
    }*/

    /*void preorder(RBTreeNode* root) {
        printPreOrder(root);
    }*/

    /*void inorder(RBTreeNode* root) {
        printInOrder(root);
    }*/

    /*void postorder(RBTreeNode* root) {
        printPostOrder(root);
    }*/

    RBTreeNode* searchTree(int key) {
        return searchTreeHelper(root, key);
    }

    vector<RBTreeNode*> findClosest(int key) {
        return findClosestHelper(root, key);
    }

    RBTreeNode* minimum(RBTreeNode* node) {
        while (node->left != null) {
            node = node->left;
        }
        return node;
    }

    RBTreeNode* maximum(RBTreeNode* node) {
        while (node->right != null) {
            node = node->right;
        }
        return node;
    }

    RBTreeNode* successor(RBTreeNode* x) {
        if (x->right != null) {
            return minimum(x->right);
        }

        RBTreeNode* y = x->parent;
        while (y != null && x == y->right) {
            x = y;
            y = y->parent;
        }
        return y;
    }

    RBTreeNode* predecessor(RBTreeNode* x) {
        if (x->left != null) {
            return maximum(x->left);
        }

        RBTreeNode* y = x->parent;
        while (y != null && x == y->left) {
            x = y;
            y = y->parent;
        }

        return y;
    }

    void leftRotate(RBTreeNode* x) {
        RBTreeNode* y = x->right;
        x->right = y->left;
        if (y->left != null) {
            y->left->parent = x;
        }
        y->parent = x->parent;
        if (x->parent == nullptr) {
            this->root = y;
        }
        else if (x == x->parent->left) {
            x->parent->left = y;
        }
        else {
            x->parent->right = y;
        }
        y->left = x;
        x->parent = y;
    }

    void rightRotate(RBTreeNode* x) {
        RBTreeNode* y = x->left;
        x->left = y->right;
        if (y->right != null) {
            y->right->parent = x;
        }
        y->parent = x->parent;
        if (x->parent == nullptr) {
            this->root = y;
        }
        else if (x == x->parent->right) {
            x->parent->right = y;
        }
        else {
            x->parent->left = y;
        }
        y->right = x;
        x->parent = y;
    }

    void insert(const BookNode& data) {
        RBTreeNode* node = new RBTreeNode();
        node->parent = nullptr;
        node->data = data;
        node->left = null;
        node->right = null;
        node->color = 1;

        RBTreeNode* y = nullptr;
        RBTreeNode* x = this->root;

        while (x != null) {
            y = x;
            if (node->data < x->data) {
                x = x->left;
            }
            else {
                x = x->right;
            }
        }

        node->parent = y;
        if (y == nullptr) {
            root = node;
        }
        else if (node->data < y->data) {
            y->left = node;
        }
        else {
            y->right = node;
        }

        if (node->parent == nullptr) {
            node->color = 0;
            return;
        }

        if (node->parent->parent == nullptr) {
            return;
        }

        insertFix(node);
    }

    RBTreeNode* getRoot() const {
        return this->root;
    }

    RBTreeNode* getNull() const {
        return this->null;
    }

    void deleteNode(int key) {
        deleteNodeHelper(this->root, key);
    }

    /*void printTree() {
        if (root) {
            print(this->root, "", true);
        }
    }*/

    void printInRange(RBTreeNode* root, int l, int r, ofstream &os) {
        if (root == nullptr || root == null) return;

        if (root->data.BookID > l) {
            printInRange(root->left, l, r, os);
        }

        if (root->data.BookID >= l && root->data.BookID <= r) {
            cout << root->data << endl;
            os << root->data << endl;
        }

        if (root->data.BookID < r) {
            printInRange(root->right, l, r, os);
        }
    }
};

class GatorLibrary {
private:
    RBTree tree;
public:
    void PrintBook(int bookID, ofstream &os) {
        RBTreeNode* node = tree.searchTree(bookID);

        if (node != tree.getNull()) {
            cout << node->data << endl;
            os << node->data << endl;
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
        RBTreeNode* node = tree.searchTree(bookID);

        if (node) {
            //cout << node->data.AvailabilityStatus << endl;
            if (node->data.AvailabilityStatus == "Yes") {
                node->data.AvailabilityStatus = "No";
                node->data.BorrowedBy = patronID;

                cout << "Book " << bookID << " Borrowed by Patron " << patronID << endl;
                os << "Book " << bookID << " Borrowed by Patron " << patronID << endl;
            }

            else {
                if (node->data.ReservationHeap.size() < 20) {
                    auto now = std::chrono::system_clock::now();
                    auto duration = now.time_since_epoch();
                    auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
                    cout << millis << endl;

                    node->data.ReservationHeap.push({ patronPriority, millis, patronID});
                    cout << "Book " << bookID << " Reserved by Patron " << patronID << endl;
                    os << "Book " << bookID << " Reserved by Patron " << patronID << endl;
                }
            }
        }
    }

    void ReturnBook(int patronID, int bookID, ofstream &os) {
        RBTreeNode* node = tree.searchTree(bookID);

        if (node) {

            if (node->data.BorrowedBy != patronID) {
                cout << "Book " << bookID << " Reserved by Patron " << node->data.BorrowedBy << endl;
                os << "Book " << bookID << " Reserved by Patron " << node->data.BorrowedBy << endl;
            }


            else {
                node->data.AvailabilityStatus = "Yes";
                node->data.BorrowedBy = -1;

                cout << "Book " << bookID << " Returned by Patron " << patronID << endl;
                os << "Book " << bookID << " Returned by Patron " << patronID << endl;

                if (!node->data.ReservationHeap.empty()) {
                    auto [patronPriority, timeOfReservation, nextPatronID] = node->data.ReservationHeap.top();
                    node->data.ReservationHeap.pop();

                    node->data.AvailabilityStatus = "No";
                    node->data.BorrowedBy = nextPatronID;

                    cout << endl << "Book " << bookID << " Allotted to Patron " << nextPatronID << endl;
                    os << endl << "Book " << bookID << " Allotted to Patron " << nextPatronID << endl;
                }
            }
        }
    }

    void DeleteBook(int bookID, ofstream &os) {
        RBTreeNode* node = tree.searchTree(bookID);

        if (node) {
            if (node->data.ReservationHeap.empty()) {
                cout << "Book " << bookID << " is no longer available" << endl;
                os << "Book " << bookID << " is no longer available" << endl;
            }

            else {
                cout << "Book " << bookID << " is no longer available. ";  
                os << "Book " << bookID << " is no longer available. ";
                
                bool plural = node->data.ReservationHeap.size() > 1;

                if (plural) {
                    cout << "Reservations made by Patrons ";
                    os << "Reservations made by Patrons ";
                }

                else {
                    cout << "Reservation made by Patron ";
                    os << "Reservation made by Patron ";
                }
                while (!node->data.ReservationHeap.empty()) {
                    auto [patronPriority, timeOfReservation, nextPatronID] = node->data.ReservationHeap.top();
                    node->data.ReservationHeap.pop();

                    cout << nextPatronID;
                    os << nextPatronID;

                    if (!node->data.ReservationHeap.empty()) {
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

            tree.deleteNode(bookID);
        }
    }

    void FindClosestBook(int targetID, ofstream &os) {
        vector<RBTreeNode*> res = tree.findClosest(targetID);

        for (auto& node : res) {
            cout << node->data << endl;
            os << node->data << endl;
        }
    }

    void ColorFlipCount(ofstream &os) {
        cout << "Colour Flip Count: " << tree.colorFlipCount << endl;
        os << "Colour Flip Count: " << tree.colorFlipCount << endl;
    }

    static std::string trim(const std::string& str) {
        std::string result = str;

        result.erase(result.begin(), std::find_if(result.begin(), result.end(), [](unsigned char ch) {
            return !std::isspace(ch);
            }));

        result.erase(std::find_if(result.rbegin(), result.rend(), [](unsigned char ch) {
            return !std::isspace(ch);
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

                    /*cout << "before insert : "  << endl;
                    cout << bookID << endl;
                    cout << bookName << endl;
                    cout << authorName << endl;
                    cout << availabilityStatus << endl;*/

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

                    //cout << bookID << endl;
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

                    //cout << bookID1 << endl;
                    //cout << bookID2 << endl;

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

                    //cout << patronID << endl;
                    //cout << bookID << endl;
                    //cout << patronPriority << endl;

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


                    //cout << patronID << endl;
                    //cout << bookID << endl;

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

                    //cout << bookID << endl;

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

                    //cout << targetID << endl;

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


    /*int n;
    cin >> n;
    cin.get();

    for (int i = 0; i < n; i++) {
        getline(cin, command);
        operations.push_back(command);
    }*/
    while (getline(inputFile, command)) {
        operations.push_back(command);
    }

    //ofstream outputFile("output_file.txt");

    library.ExecuteOperations(operations, outputFile);

    

    return 0;
}