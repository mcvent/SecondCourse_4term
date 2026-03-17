#include <iostream>
#include <vector>
#include <windows.h>
#include <iomanip>
using namespace std;
HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
#define RED true
#define BLACK false

struct tree {
    int inf;
    bool color; //цвет узла
    tree* left, * right, * parent; //указатели левого и правого потомков и на родителя

    tree(int val, bool col = RED, tree* p = nullptr, tree* l = nullptr, tree* r = nullptr)
        : inf(val), color(col), parent(p), left(l), right(r) {
    }
};
tree* root = nullptr; //глобальная переменная для корня

void max_height(tree* node, short& max_depth, short depth = 1) {
    if (depth > max_depth) max_depth = depth;
    if (node->left) max_height(node->left, max_depth, depth + 1);
    if (node->right) max_height(node->right, max_depth, depth + 1);
}

void print_tree(tree* node, COORD pos, short offset) {
    SetConsoleTextAttribute(hConsole, node->color == RED ? 12 : 8);
    SetConsoleCursorPosition(hConsole, pos);
    cout << setw(offset + 1) << node->inf;

    if (node->left) print_tree(node->left, { pos.X, short(pos.Y + 1) }, offset >> 1);
    if (node->right) print_tree(node->right, { short(pos.X + offset), short(pos.Y + 1) }, offset >> 1);
}

void print() {
    if (!root) return;

    short height = 1;
    max_height(root, height);

    short width = 1 << (height + 1);
    const short max_width = 128;
    if (width > max_width) width = max_width;

    // Резервируем место
    for (short i = 0; i < height; ++i) cout << '\n';

    GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
    COORD endPos = consoleInfo.dwCursorPosition;

    print_tree(root, { 0, short(endPos.Y - height) }, width >> 1);

    SetConsoleCursorPosition(hConsole, endPos);
    SetConsoleTextAttribute(hConsole, 7);
}

void left_rotate(tree* x) { //функция для балансировки дерева
    tree* y = x->right;
    x->right = y->left;
    if (y->left != nullptr) {
        y->left->parent = x;
    }
    y->parent = x->parent;
    if (x->parent == nullptr) {
        root = y;
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

void right_rotate(tree* x) { //функция для балансировки дерева
    tree* y = x->left;
    x->left = y->right;
    if (y->right != nullptr) {
        y->right->parent = x;
    }
    y->parent = x->parent;
    if (x->parent == nullptr) {
        root = y;
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

void insert_help(tree* k) { //"перекрашиваем"(чиним) дерево после вставки
    while (k->parent != nullptr && k->parent->color == RED) {
        if (k->parent == k->parent->parent->right) {
            tree* u = k->parent->parent->left;
            if (u != nullptr && u->color == RED) {
                u->color = BLACK;
                k->parent->color = BLACK;
                k->parent->parent->color = RED;
                k = k->parent->parent;
            }
            else {
                if (k == k->parent->left) {
                    k = k->parent;
                    right_rotate(k);
                }
                k->parent->color = BLACK;
                k->parent->parent->color = RED;
                left_rotate(k->parent->parent);
            }
        }
        else {
            tree* u = k->parent->parent->right;
            if (u != nullptr && u->color == RED) {
                u->color = BLACK;
                k->parent->color = BLACK;
                k->parent->parent->color = RED;
                k = k->parent->parent;
            }
            else {
                if (k == k->parent->right) {
                    k = k->parent;
                    left_rotate(k);
                }
                k->parent->color = BLACK;
                k->parent->parent->color = RED;
                right_rotate(k->parent->parent);
            }
        }
        if (k == root) break; //выход из цикла если пришли в корень
    }
    root->color = BLACK; //корень всегда черный
}

void insert(int el) { //функция вставки элемента
    tree* node = new tree(el); //при создании, узел - красный
    tree* y = nullptr;
    tree* x = root;

    while (x) { //поиск места вставки
        y = x;
        if (node->inf < x->inf) {
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
    else if (node->inf < y->inf) {
        y->left = node;
    }
    else {
        y->right = node;
    }

    if (node->parent == nullptr) {
        node->color = BLACK;
        return;
    }
    if (node->parent->parent == nullptr) return;

    insert_help(node); //перекрашиваем дерево
}

// функция для поиска узла по значению
tree* search(tree* node, int val) {
    if (node == nullptr || node->inf == val)
        return node;

    if (val < node->inf)
        return search(node->left, val);
    else
        return search(node->right, val);
}

// Функция для поиска минимального элемента в поддереве
tree* find_min(tree* node) {
    while (node->left != nullptr)
        node = node->left;
    return node;
}

// Функция для поиска максимального элемента в поддереве
tree* find_max(tree* node) {
    while (node->right != nullptr)
        node = node->right;
    return node;
}

// Функция для поиска узла-преемника (следующий по величине)
tree* find_successor(tree* node) {
    if (node->right != nullptr)
        return find_min(node->right);

    tree* parent = node->parent;
    while (parent != nullptr && node == parent->right) {
        node = parent;
        parent = parent->parent;
    }
    return parent;
}

// Функция для замены одного узла другим
void transplant(tree* u, tree* v) {
    if (u->parent == nullptr) {
        root = v;
    }
    else if (u == u->parent->left) {
        u->parent->left = v;
    }
    else {
        u->parent->right = v;
    }

    if (v != nullptr) {
        v->parent = u->parent;
    }
}

// функция для исправления красно-черных свойств после удаления
void delete_fixup(tree* x, tree* parent) {
    while (x != root && (x == nullptr || x->color == BLACK)) {
        if (x == parent->left) {
            tree* w = parent->right; // брат узла x

            if (w->color == RED) {
                w->color = BLACK;
                parent->color = RED;
                left_rotate(parent);
                w = parent->right;
            }

            if ((w->left == nullptr || w->left->color == BLACK) &&
                (w->right == nullptr || w->right->color == BLACK)) {
                w->color = RED;
                x = parent;
                parent = parent->parent;
            }
            else {
                if (w->right == nullptr || w->right->color == BLACK) {
                    if (w->left != nullptr)
                        w->left->color = BLACK;
                    w->color = RED;
                    right_rotate(w);
                    w = parent->right;
                }

                w->color = parent->color;
                parent->color = BLACK;
                if (w->right != nullptr)
                    w->right->color = BLACK;
                left_rotate(parent);
                x = root;
                break;
            }
        }
        else {
            tree* w = parent->left; // брат узла x

            if (w->color == RED) {
                w->color = BLACK;
                parent->color = RED;
                right_rotate(parent);
                w = parent->left;
            }

            if ((w->right == nullptr || w->right->color == BLACK) &&
                (w->left == nullptr || w->left->color == BLACK)) {
                w->color = RED;
                x = parent;
                parent = parent->parent;
            }
            else {
                if (w->left == nullptr || w->left->color == BLACK) {
                    if (w->right != nullptr)
                        w->right->color = BLACK;
                    w->color = RED;
                    left_rotate(w);
                    w = parent->left;
                }

                w->color = parent->color;
                parent->color = BLACK;
                if (w->left != nullptr)
                    w->left->color = BLACK;
                right_rotate(parent);
                x = root;
                break;
            }
        }
    }

    if (x != nullptr)
        x->color = BLACK;
}

// функция для удаления узла
void delete_node(int val) {
    tree* z = search(root, val);
    if (z == nullptr) {
        cout << "Элемент " << val << " не найден" << endl;
        return;
    }

    tree* y = z;
    tree* x;
    bool y_original_color = y->color;

    // нет левого потомка
    if (z->left == nullptr) {
        x = z->right;
        transplant(z, z->right);
    }
    // нет правого потомка
    else if (z->right == nullptr) {
        x = z->left;
        transplant(z, z->left);
    }
    // есть оба потомка
    else {
        y = find_min(z->right); // находим минимальный в правом поддереве
        y_original_color = y->color;
        x = y->right;

        if (y->parent == z) {
            if (x != nullptr)
                x->parent = y;
        }
        else {
            transplant(y, y->right);
            y->right = z->right;
            y->right->parent = y;
        }

        transplant(z, y);
        y->left = z->left;
        y->left->parent = y;
        y->color = z->color;
    }

    delete z;

    //если удалили черный узел, нужно исправить дерево
    if (y_original_color == BLACK) {
        if (x != nullptr) {
            delete_fixup(x, x->parent);
        }
        else {
            //создаем временный лист для исправления
            tree* nil = new tree(-1, BLACK);
            nil->parent = x ? x->parent : nullptr;
            delete_fixup(nil, nil->parent);
            delete nil;
        }
    }

    if (root != nullptr)
        root->color = BLACK;
}

int main() {
    setlocale(LC_ALL, "RU");

    //вставка элементов в дерево
    vector<int> nums = { 8, 3, 10, 1, 6, 14, 4, 7, 13 };
    for (int num : nums) {
        insert(num);
    }

    cout << "Визуализация дерева после вставки: " << endl;
    print();
    cout << "\n\n\n";

    // Демонстрация удаления
    cout << "Удаляем элемент 6:" << endl;
    delete_node(6);
    print();
    cout << "\n\n\n";

    cout << "Удаляем элемент 8 (корень):" << endl;
    delete_node(8);
    print();
    cout << "\n\n\n";
    return 0;
}