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
        if(node->parent == 0) return false;
        else return node->parent->left == node;
    }

    static bool is_right_child(const Node *node) {
        if(node->parent == 0) return false;
        else return node->parent->right == node;
    }

    Node** get_parent_ptr(Node *node) {
        if(node->parent == 0) return &root;
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

    template <class NodeT>
    static NodeT* search(NodeT* root, const T &value) {
        NodeT *now = root;
        while(now)
        {
            if(value < now->value) now = now->left;
            else if(value > now->value) now = now->right;
            else return now;
        }
        return nullptr;
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

    Node* rotate_right(Node *old_root) {
        //todo: Algorithmus spiegeln!
        return 0;
        Node
                **root_ptr = get_parent_ptr(old_root),
                *parent = old_root->parent,
                *new_root = old_root->left,
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

public:
    Tree():root(0){}

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

    bool contains(const T &t) const{
        return static_cast<bool>(search(root, t));
    }

    bool insert(const T &value) {
        Node *parent = 0,
             *now = root;

        bool is_left_child = false;

        while(now) {
            parent = now;

            if(value < now->value) {
                is_left_child = true;
                now = now->left;
            }
            else if(value > now->value) {
                is_left_child = false;
                now = now->right;
            }
            else {
                return false; // Node Exists already
            }
        }

        Node *new_node = new Node(value);
        new_node->parent = parent;

        if(parent == 0) root = new_node;
        else if(is_left_child) parent->left = new_node;
        else parent->right = new_node;

        assert(is_valid(root));
        return true;
    }

    void rotateLeft() {
        root = rotate_left(root);
    }

    void rotateRight() {
        root = rotate_right(root);
    }

};

template<class T>
Tree<T>::Node::Node(const T &val):
        parent(nullptr), left(nullptr), right(nullptr), value(val) {}


int main() {

    Tree<int> *tree = new Tree<int>();

    tree->insert(5);
    tree->insert(7);
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


    cout << endl << "===ROTATED LEFT===" << endl;
    tree->rotateLeft();

    stream.clear();
    stream.rdbuf(std::cout.rdbuf());
    tree->dump(stream);


    cout << endl << "===ROTATED RIGHT===" << endl;
    tree->rotateRight();

    stream.clear();
    stream.rdbuf(std::cout.rdbuf());
    tree->dump(stream);

    return 0;
}
