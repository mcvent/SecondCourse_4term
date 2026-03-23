#include <iostream>
#include <iomanip>
#include <vector>
#include <windows.h>
using namespace std;

HANDLE outp = GetStdHandle(STD_OUTPUT_HANDLE);
CONSOLE_SCREEN_BUFFER_INFO csbInfo;

struct tree {
    int inf;
    tree* left;
    tree* right;
    tree* parent;
    int height; //хранение высоты дерева
};

tree* node(int x) {
    tree* n = new tree;
    n->inf = x;
    n->left = n->right = n->parent = NULL;
    n->height = 1;  //высота нового узла - 1
    return n;
}

int height(tree* n) {
    if (n) {
        return n->height;
    }
    else {
        return 0;
    }
}

//функция для вычисления баланс-фактора
int balance_factor(tree* n) {
    return height(n->left) - height(n->right);
}
void update_height(tree* n) {
    int hl = height(n->left);
    int hr = height(n->right);
    if (hl > hr) {
        n->height = hl + 1;
    }
    else {
        n->height = hr + 1;
    }
}

tree* rotateRight(tree* y) {
    tree* x = y->left;
    y->left = x->right;
    if (x->right) x->right->parent = y;
    x->right = y;
    x->parent = y->parent;
    y->parent = x;

    update_height(y);
    update_height(x);

    return x;
}

tree* rotateLeft(tree* x) {
    tree* y = x->right;
    x->right = y->left;
    if (y->left) y->left->parent = x;
    y->left = x;
    y->parent = x->parent;
    x->parent = y;

    update_height(x);
    update_height(y);

    return y;
}

tree* find_min(tree* t) {
    if (t->left != nullptr) {
        return find_min(t->left);
    }
    else {
        return t;
    }
}

//балансируем узел
tree* balance(tree* n) {
    update_height(n);

    if (balance_factor(n) == 2) {
        if (balance_factor(n->left) < 0)
            n->left = rotateLeft(n->left);
        return rotateRight(n);
    }
    if (balance_factor(n) == -2) {
        if (balance_factor(n->right) > 0)
            n->right = rotateRight(n->right);
        return rotateLeft(n);
    }

    return n;
}

tree* remove(tree* t, int x) {//функция для удаления узла
    if (!t) return nullptr;

    if (x < t->inf) {
        t->left = remove(t->left, x);
    }
    else if (x > t->inf) {
        t->right = remove(t->right, x);
    }
    else {
        if (!t->left) {
            tree* temp = t->right;
            delete t;
            return temp;
        }
        else if (!t->right) {
            tree* temp = t->left;
            delete t;
            return temp;
        }
        else {
            tree* maxLeft = t->left;
            while (maxLeft->right) {
                maxLeft = maxLeft->right;
            }
            t->inf = maxLeft->inf;
            t->left = remove(t->left, maxLeft->inf);
        }
    }

    return balance(t);
}

tree* insert(tree* n, int x) {
    if (!n) return node(x);

    if (x < n->inf) {
        n->left = insert(n->left, x);
        n->left->parent = n;
    }
    else if (x > n->inf) {
        n->right = insert(n->right, x);
        n->right->parent = n;
    }
    else {
        return n;  // Дубликаты не допускаются
    }

    return balance(n);
}

//поиск элемента в дереве бинарного поиска
tree* find(tree* tr, int x) {
    if (!tr || tr->inf == x) return tr; //нашли или дошли до конца ветки
    if (x < tr->inf)
        return find(tr->left, x); //ищем по левой ветке
    else
        return find(tr->right, x); //ищем по правой ветке
}

//функция для вычисления высоты дерева
//высота узла - максимальная длина пути от узла до листа
void max_height(tree* x, short& max, short deepness = 1) {
    if (deepness > max) max = deepness;
    if (x->left) max_height(x->left, max, deepness + 1);
    if (x->right) max_height(x->right, max, deepness + 1);
}

//функция проверки размера консоли
bool isSizeOfConsoleCorrect(const short& width, const short& height) {
    GetConsoleScreenBufferInfo(outp, &csbInfo);
    COORD szOfConsole = csbInfo.dwSize;
    if (szOfConsole.X < width || szOfConsole.Y < height) {
        cout << "Please increase the size of the terminal. Size now: "
            << szOfConsole.X << "x" << szOfConsole.Y
            << ". Minimum required: " << width << "x" << height << ".\n";
        return false;
    }
    return true;
}

//вспомогательная функция для печати дерева
void print_helper(tree* x, const COORD pos, const short offset) {
    SetConsoleCursorPosition(outp, pos);
    cout << right << setw(offset + 1) << x->inf;
    if (x->left) print_helper(x->left, { pos.X, short(pos.Y + 1) }, offset >> 1);
    if (x->right) print_helper(x->right, { short(pos.X + offset), short(pos.Y + 1) }, offset >> 1);
}

//основная функция печати дерева
void printTree(tree* tr) {
    if (tr) {
        short max = 1;
        max_height(tr, max); //вычисляем высоту дерева
        short width = 1 << (max + 1); //вычисляем ширину для печати
        short max_w = 128;
        if (width > max_w) width = max_w;

        while (!isSizeOfConsoleCorrect(width, max)) {
            system("pause");
        }

        //резервируем место для вывода
        for (short i = 0; i < max; ++i) cout << '\n';

        GetConsoleScreenBufferInfo(outp, &csbInfo);
        COORD endPos = csbInfo.dwCursorPosition;

        //выводим дерево
        print_helper(tr, { 0, short(endPos.Y - max) }, width >> 1);
        SetConsoleCursorPosition(outp, endPos);
    }
    else {
        cout << "The tree is empty." << endl;
    }
}

int main() {
    setlocale(LC_ALL, "RU");
    vector<int> nums = { 50, 30, 70, 20, 40, 60, 80, 15, 25, 35, 45, 55, 65, 75, 85 };
    tree* treee = NULL;
    for (int num : nums)
        treee = insert(treee, num);

    cout << endl << "вывод" << endl;
    printTree(treee);

    int y;
    cout << "\nвведите узел для удаления: ";
    cin >> y;
    remove(treee, y);
    cout << "\nдерево после удаления " << y << ":\n";
    printTree(treee);

    return 0;
}