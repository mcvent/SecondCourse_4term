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
tree* sibling(tree* X);
void replace(tree*& tr, tree* X);
void delete_case1(tree*& tr, tree* X);
void delete_case2(tree*& tr, tree* X);
void delete_case3(tree*& tr, tree* X);
void delete_case4(tree*& tr, tree* X);
void delete_case5(tree*& tr, tree* X);
void delete_case6(tree*& tr, tree* X);
void delete_node(tree*& tr, int value);
tree* find_max(tree* node);
tree* find_min(tree* node);

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

tree* sibling(tree* X) {
    if (!X || !X->parent) return nullptr;
    if (X == X->parent->left)
        return X->parent->right;
    else
        return X->parent->left;
}

void replace(tree*& tr, tree* X) {
    // Алгоритм 18: замена X на его единственного ребенка
    tree* ch = nullptr;
    if (X->left && !X->right)
        ch = X->left;
    else if (X->right && !X->left)
        ch = X->right;
    else return; // нет одного ребенка — не вызываем

    ch->parent = X->parent;
    if (X->parent) {
        if (X == X->parent->left)
            X->parent->left = ch;
        else
            X->parent->right = ch;
    }
    else {
        tr = ch; // новый корень
    }
}

void delete_case1(tree*& tr, tree* X);
void delete_case2(tree*& tr, tree* X);
void delete_case3(tree*& tr, tree* X);
void delete_case4(tree*& tr, tree* X);
void delete_case5(tree*& tr, tree* X);
void delete_case6(tree*& tr, tree* X);

void delete_case1(tree*& tr, tree* X) {
    // Алгоритм 12: X — корень
    if (!X->parent) {
        if (X->left) tr = X->left;
        else tr = X->right;
        if (tr) tr->parent = nullptr;
        // цвет не важен, корень станет чёрным потом
    }
    else {
        delete_case2(tr, X);
    }
}

void delete_case2(tree*& tr, tree* X) {
    // Алгоритм 13: брат красный
    tree* S = sibling(X);
    if (S && S->color == RED) {
        X->parent->color = RED;
        S->color = BLACK;
        if (X == X->parent->left)
            left_rotate(X->parent);
        else
            right_rotate(X->parent);
    }
    delete_case3(tr, X);
}

void delete_case3(tree*& tr, tree* X) {
    // Алгоритм 14: родитель, брат, племянники — чёрные
    tree* S = sibling(X);
    bool left_nephew_black = (!S || !S->left || S->left->color == BLACK);
    bool right_nephew_black = (!S || !S->right || S->right->color == BLACK);

    if ((!X->parent || X->parent->color == BLACK) &&
        (!S || S->color == BLACK) &&
        left_nephew_black && right_nephew_black) {
        if (S) S->color = RED;
        delete_case1(tr, X->parent ? X->parent : X);
    }
    else {
        delete_case4(tr, X);
    }
}

void delete_case4(tree*& tr, tree* X) {
    // Алгоритм 15: родитель красный, брат и племянники чёрные
    tree* S = sibling(X);
    bool left_nephew_black = (!S || !S->left || S->left->color == BLACK);
    bool right_nephew_black = (!S || !S->right || S->right->color == BLACK);

    if (X->parent && X->parent->color == RED &&
        S && S->color == BLACK &&
        left_nephew_black && right_nephew_black) {
        if (S) S->color = RED;
        X->parent->color = BLACK;
    }
    else {
        delete_case5(tr, X);
    }
}

void delete_case5(tree*& tr, tree* X) {
    // Алгоритм 16: брат чёрный, "прилежащий" племянник красный
    tree* S = sibling(X);
    if (S && S->color == BLACK) {
        if (X == X->parent->left &&
            S->left && S->left->color == RED &&
            (!S->right || S->right->color == BLACK)) {
            S->color = RED;
            S->left->color = BLACK;
            right_rotate(S);
        }
        else if (X == X->parent->right &&
            S->right && S->right->color == RED &&
            (!S->left || S->left->color == BLACK)) {
            S->color = RED;
            S->right->color = BLACK;
            left_rotate(S);
        }
    }
    delete_case6(tr, X);
}

void delete_case6(tree*& tr, tree* X) {
    // Алгоритм 17: брат чёрный, "противоположный" племянник красный
    tree* S = sibling(X);
    if (S) {
        S->color = X->parent->color;
        X->parent->color = BLACK;
        if (X == X->parent->left) {
            if (S->right) S->right->color = BLACK;
            left_rotate(X->parent);
        }
        else {
            if (S->left) S->left->color = BLACK;
            right_rotate(X->parent);
        }
    }
}

tree* find_max(tree* node) {
    while (node->right) node = node->right;
    return node;
}

tree* find_min(tree* node) {
    while (node->left) node = node->left;
    return node;
}

void delete_node(tree*& tr, int value) {
    // Алгоритм 19: удаление узла
    tree* X = search(tr, value);
    if (!X) return;

    // Случай 1: два ребёнка
    if (X->left && X->right) {
        tree* buf;
        if (X->inf <= tr->inf)
            buf = find_max(X->left);  // максимум в левом поддереве
        else
            buf = find_min(X->right); // минимум в правом поддереве

        // Меняем значения
        int temp = X->inf;
        X->inf = buf->inf;
        buf->inf = temp;
        X = buf; // теперь удаляем buf
    }
    // Случай 2: один ребёнок
    if ((X->left && !X->right) || (!X->left && X->right)) {
        tree* ch = X->left ? X->left : X->right;
        replace(tr, X);
        if (X->color == BLACK) {
            if (ch->color == RED)
                ch->color = BLACK;
            else
                delete_case1(tr, ch);
        }
        delete X;
        return;
    }

    // Случай 3: нет детей
    if (!X->left && !X->right) {
        if (X->color == BLACK) {
            delete_case1(tr, X);
        }
        // Удаляем ссылку у родителя
        if (X->parent) {
            if (X == X->parent->left)
                X->parent->left = nullptr;
            else
                X->parent->right = nullptr;
        }
        else {
            tr = nullptr; // дерево стало пустым
        }
        delete X;
    }
}

int main() {
    setlocale(LC_ALL, "RU");
    //вставка элементов в дерево
    vector<int> nums = { 18, 45, 30, 15,20,25,28,27,26,32,33,35 };
    for (int num : nums) {
        insert(num);
    }

    cout << "Визуализация дерева после вставки: " << endl;
    print();
    cout << "\n\n\n";

    cout << "Удаляем элемент 18:" << endl;
    delete_node(root, 18);
    print();
    cout << "\n\n\n";

    cout << "Удаляем элемент 30:" << endl;
    delete_node(root, 30);
    print();
    cout << "\n\n\n";
    return 0;
}
