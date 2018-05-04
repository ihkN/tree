#include <iostream>
#include <cmath>
#include <sstream>
#include <cassert>
#include <algorithm>

using namespace std;

template <class T>
class Tree {
private:

    struct Node {
        T value;
        Node *left, *right, *parent;

        explicit Node(const T&val);;
    };

    Node *root;

    static bool is_left_child(const Node *node) {
        if(node->parent == nullptr) return false;
        else return node->parent->left == node;
    }

    static bool is_right_child(const Node *node) {
        if(node->parent == nullptr) return false;
        else return node->parent->right == node;
    }

    Node** get_parent_ptr(Node *node) {
        if(node->parent == nullptr) return &root;
        else if(is_left_child(node)) return &node->parent->left;
        else return &node->parent->right;
    }

    static string format_label(const Node *node) {
        ostringstream out;
        if(node) {
            out << node->value;
            return out.str();
        }
        else return "";
    }

    static unsigned int get_height(const Node *node)
    {
        if(!node) return 0;

        unsigned int left_height = 0, right_height = 0;

        if(node->left) left_height = get_height(node->left);
        if(node->right) right_height = get_height(node->right);

        return max(left_height, right_height) + 1;
    }

    static unsigned long get_width(const Node *node) {
        if(!node) return 0;

        unsigned long right_width = 0;
        unsigned long left_width = 0;

        if(node->left) left_width = get_width(node->left);
        if(node->right) right_width = get_width(node->right);

        return left_width + format_label(node).length() + right_width;
    }

    static void dump_spaces(ostream &out, unsigned long count) {
        for(unsigned i = 0; i < count; ++i) out.put(' ');
    }

    static void dump_line(ostream &out, const Node *node, unsigned line) {
        if(!node) return;

        if(line == 1) {
            dump_spaces(out, get_width(node->left));
            out << format_label(node);
            dump_spaces(out, get_width(node->right));
        }
        else {
            dump_line(out, node->left, line-1);
            dump_spaces(out, format_label(node).length());
            dump_line(out, node->right, line-1);
        }
    }

    static void dump_node(ostream &out, const Node *node) {
        for(unsigned line = 1, height = get_height(node); line <= height; ++line)
        {
            dump_line(out, node, line);
            out.put('\n');
        }
        out.flush();
    }

    bool is_valid(Node *node) {
        if(node == 0) return true;
        if(*get_parent_ptr(node) != node) return false;
        if(!is_valid(node->left)) return false;
        if(!is_valid(node->right)) return false;

        return true;
    }

    static void dispose(Node *node) {
        if(node) {
            dispose(node->left);
            dispose(node->right);
            delete node;
        }
    }

    bool search(Node* node, const T &value) {
        if(node == nullptr) return false;

        if(value < node->value) return search(node->left, value);
        else if(value > node->value) return search(node->right, value);
        else return true;
    }

    Node* rotate_left(Node *old_root) {
        Node
                **root_ptr = get_parent_ptr(old_root),
                *parent = old_root->parent,
                *new_root = old_root->right,
                *old_roots_left_child = old_root->left,
                *new_roots_left_child = new_root->left,
                *new_roots_right_child = new_root->right;

        *root_ptr = new_root;
        new_root->left = old_root;
        new_root->right =  new_roots_right_child;
        old_root->left = old_roots_left_child;
        old_root->right = new_roots_left_child;

        new_root->parent = parent;
        old_root->parent = new_root;

        if(new_roots_left_child) new_roots_left_child->parent = old_root;
        if(new_roots_right_child) new_roots_right_child->parent = new_root;
        if(old_roots_left_child) old_roots_left_child->parent = old_root;

        assert(is_valid(new_root));
        return new_root;
    }

    void insert_node(Node *parent, Node *root_node, const T &value)
    {
        if(root_node == nullptr) {
            Node *new_node = new Node(value);
            new_node->parent = nullptr;

            if (parent == nullptr) {
                root = new_node;
                new_node->parent = parent;
            } else if (parent->value > value) {
                parent->left = new_node;
            } else if (parent->value < value) {
                parent->right = new_node;
            }

            //First Element not set
            root_node = new_node;
            root_node->parent = parent;

            assert(is_valid(root_node));
            return;
        }

        if(root_node->value > value) insert_node(root_node, root_node->left, value);
        else insert_node(root_node, root_node->right, value);
    }

    Node* rotate_right(Node*A){
        Node
                **root_ptr = get_parent_ptr(A),
                *parent = A->parent,
                *B = A->left,
                *X = B->left,
                *Y = B->right,
                *Z = A->right;

        *root_ptr = B;
        B->left = X;
        B->right = A;
        A->left = Y;
        A->right = Z;

        B->parent = parent;
        A->parent = B;
        if(Z)
            Z->parent = A;
        if(X)
            X->parent = B;
        if(Y)
            Y->parent = A;

        return B;
    }

    Node* get_min(Node* node)
    {
        if(node->left == nullptr) return node;
        else return get_min(node->left);
    }

    Node* get_max(Node* node)
    {
        if(node->right == nullptr) return node;
        else return get_max(node->right);
    }

public:
    Tree():root(nullptr){}

    ~Tree() {
        dispose(root);
    }

    Tree(const Tree &other) {
        if(other.is_empty()) {
            root = 0;
        }
        else {
            root = new Node(other.root->value);

            try {
                Node *now = root,
                     *other_now = other.root;

                while(other_now) {
                    if(other_now->left && !now->left)  {
                        now->left = new Node(other_now->left->value);
                        now->left->parent = now;
                        now = now->left;
                        other_now = other_now->left;
                    }
                    else if(other_now->right && !now->right) {
                        now->right = new Node(other_now->right->value);
                        now->right->parent = now;
                        now = now->right;
                        other_now = other_now->right;
                    }
                    else {
                        now = now->parent;
                        other_now = other_now->parent;
                    }
                    is_valid(root);
                }
            }
            catch (...) {
                this->~Tree();
                throw;
            }
        }
    }

    void swap(Tree &other) {
        std::swap(root, other.root);
    }

    Tree&operator=(const Tree &other) {
        Tree temp(other);
        swap(temp);
        return *this;
    }

    bool is_empty() const {
        return !root;
    }

    void dump(ostream &out) {
        dump_node(out, root);
    }

    bool contains(const T &t) {
        return search(root, t);
    }

    void insert(const T &value) {
        insert_node(nullptr, root, value);
    }

    void rotateLeft() {
        root = rotate_left(root);
    }

    void rotateRight() {
        root = rotate_right(root);
    }

    Node* get_minimum() {
        return get_min(root);
    }

    Node* get_maximum() {
        return get_max(root);
    }

};

template<class T>
Tree<T>::Node::Node(const T &val):
        parent(nullptr), left(nullptr), right(nullptr), value(val) {}


int main() {
    auto *tree = new Tree<int>();

    tree->insert(7);
    tree->insert(5);
    tree->insert(4);
    tree->insert(6);
    tree->insert(9);
    tree->insert(10);
    tree->insert(12);

    cout << endl << "===BINARY TREE===" << endl;

    ostream stream(nullptr);
    stream.rdbuf(std::cout.rdbuf());
    tree->dump(stream);

    cout << endl << "===TESTING AREA===" << endl;

    cout << "contains(19) : " << tree->contains(19) << endl;
    cout << "contains(5) : " << tree->contains(5) << endl;

    cout << "get_min: " << tree->get_minimum()->value << endl;
    cout << "get_max: " << tree->get_maximum()->value << endl;

    cout << endl << "===LEFT ROTATED===" << endl;
    tree->rotateLeft();

    stream.clear();
    stream.rdbuf(std::cout.rdbuf());
    tree->dump(stream);

    cout << endl << "===RIGHT ROTATED===" << endl;
    tree->rotateRight();

    stream.clear();
    stream.rdbuf(std::cout.rdbuf());
    tree->dump(stream);

    return 1;
}
