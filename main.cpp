#include <iostream>
#include <cassert>

#include <boost/algorithm/string.hpp>

#define RED 0
#define BLACK 1

using namespace std;

template <class T>
class Tree {
private:

    struct Node {
        T value;
        bool color; // 0 = RED ; 1 = BLACK
        Node *left, *right, *parent;

        explicit Node(const T&val);;
    };

    Node *root;

    static string format_label(const Node *node) {
        if(node) {

            string reset_bg = "\033[0m";
            string red = "\033[31m";
            string black = "\033[37m"; //WHITE

            string out = to_string(node->value);

            if(node->color == RED) out.insert(0, red);
            else if(node->color == BLACK) out.insert(0, black);

            out.append(reset_bg);

            return out;
        }
        else return "";
    }
    static unsigned int get_height(const Node *node) {
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

        //Remove Colors
        string formatted = remove_formats(format_label(node));

        return left_width + formatted.length() + right_width;
    }
    static void dump_spaces(string &out, unsigned long count) {
        for(unsigned i = 0; i < count; ++i) out += ' ';
    }
    static string remove_formats (string in) {
        boost::erase_all(in, "\033[0m");
        boost::erase_all(in, "\033[31m");
        boost::erase_all(in, "\033[37m");

        return in;
    }
    static void dump_line(string &out, const Node *node, unsigned line) {
        if(!node) return;

        if(line == 1) {
            dump_spaces(out, get_width(node->left));
            out += format_label(node);
            dump_spaces(out, get_width(node->right));
        }
        else {
            dump_line(out, node->left, line-1);

            string formatted = remove_formats(format_label(node));

            dump_spaces(out, formatted.length());
            dump_line(out, node->right, line-1);
        }
    }
    static void dump_node(string &out, const Node *node) {
        for(unsigned line = 1, height = get_height(node); line <= height; ++line)
        {
            dump_line(out, node, line);
            out += '\n';
        }
    }

    static void dispose(Node *node) {
        if(node) {
            dispose(node->left);
            dispose(node->right);
            delete node;
        }
    }

    void rotate_left(Node *&root, Node *&pt) {
        Node *pt_right = pt->right;

        pt->right = pt_right->left;

        if(pt->right != NULL) pt_right->parent = pt;

        pt_right->parent = pt->parent;

        if(pt->parent == NULL) root = pt_right;
        else if(pt == pt->parent->left) pt->parent->left = pt_right;
        else pt->parent->right = pt_right;

        pt_right->left = pt;
        pt->parent = pt_right;
    }

    void rotate_right(Node *&root, Node *&pt) {
        Node *pt_left = pt->left;

        pt->left = pt_left->right;

        if (pt->left != NULL) pt->left->parent = pt;

        pt_left->parent = pt->parent;

        if (pt->parent == NULL) root = pt_left;
        else if (pt == pt->parent->left) pt->parent->left = pt_left;
        else pt->parent->right = pt_left;

        pt_left->right = pt;
        pt->parent = pt_left;
    }

    Node* search(Node* node, const T &value) {
        if(node == NULL) return NULL;
        else if(node->value > value) return search(node->left, value);
        else if(node->value < value) return search(node->right, value);
        else return node;
    }

    void bst_insert(Node *parent, Node *root_node, const T &value) {
        if(root_node == nullptr) {
            auto *new_node = new Node(value);
            new_node->color = RED;

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

//            assert(is_valid(root_node));

            return;
        }

        if(root_node->value > value) bst_insert(root_node, root_node->left, value);
        else bst_insert(root_node, root_node->right, value);
    }

    void fix_violation(Node *&root, Node *&pt)
    {
        Node *parent_pt = NULL;
        Node *grand_parent_pt = NULL;

        while ((pt != root) && (pt->color != BLACK) && (pt->parent->color == RED))
        {
            parent_pt = pt->parent;
            grand_parent_pt = pt->parent->parent;

            /* Case : A
             *
             * Parent of pt is left child of Grand-parent of pt
             */
            if (parent_pt == grand_parent_pt->left)
            {

                Node *uncle_pt = grand_parent_pt->right;

                /* Case : 1
                 *
                 * The uncle of pt is also red
                 * Only Recoloring required
                 */
                if (uncle_pt != NULL && uncle_pt->color == RED)
                {
                    grand_parent_pt->color = RED;
                    parent_pt->color = BLACK;
                    uncle_pt->color = BLACK;
                    pt = grand_parent_pt;
                }

                else
                {
                    /* Case : 2
                     *
                     * pt is right child of its parent
                     * Left-rotation required
                     */
                    if (pt == parent_pt->right)
                    {
                        rotate_left(root, parent_pt);
                        pt = parent_pt;
                        parent_pt = pt->parent;
                    }

                    /* Case : 3
                     *
                     * pt is left child of its parent
                     * Right-rotation required
                     */
                    rotate_right(root, grand_parent_pt);
                    swap(parent_pt->color, grand_parent_pt->color);
                    pt = parent_pt;
                }
            }

                /* Case : B
                 *
                 * Parent of pt is right child of Grand-parent of pt
                 */
            else
            {
                Node *uncle_pt = grand_parent_pt->left;

                /*  Case : 1
                 *
                 * The uncle of pt is also red
                 * Only Recoloring required
                 */
                if ((uncle_pt != NULL) && (uncle_pt->color == RED))
                {
                    grand_parent_pt->color = RED;
                    parent_pt->color = BLACK;
                    uncle_pt->color = BLACK;
                    pt = grand_parent_pt;
                }
                else
                {
                    /* Case : 2
                     *
                     * pt is left child of its parent
                     * Right-rotation required
                     */
                    if (pt == parent_pt->left)
                    {
                        rotate_right(root, parent_pt);
                        pt = parent_pt;
                        parent_pt = pt->parent;
                    }

                    /* Case : 3
                     *
                     * pt is right child of its parent
                     * Left-rotation required
                     */
                    rotate_left(root, grand_parent_pt);
                    swap(parent_pt->color, grand_parent_pt->color);
                    pt = parent_pt;
                }
            }
        }

        root->color = BLACK;
    }
public:
    Tree():root(NULL){}

    ~Tree() {
        dispose(root);
    }

    Tree&operator=(const Tree &other) {
        Tree temp(other);
        swap(temp);
        return *this;
    }

    string print() {
        string out;
        dump_node(out, root);

        return out;
    }


    void insert(const T &value) {
        bst_insert(NULL, root, value);
        Node* node = search(root, value);
        fix_violation(root, node);

    }


};

template<class T>
Tree<T>::Node::Node(const T &val):
        parent(NULL), left(NULL), right(NULL), value(val) {}


int main() {
    auto *tree = new Tree<int>();

    tree->insert(1);
    tree->insert(2);
    tree->insert(3);
    tree->insert(4);
    tree->insert(5);
    tree->insert(6);
    tree->insert(7);
    tree->insert(8);

    cout << tree->print();

    return 1;
}
