#include <iostream>
#include <windows.h>
#include <iomanip>
using namespace std;

HANDLE outp = GetStdHandle(STD_OUTPUT_HANDLE);
CONSOLE_SCREEN_BUFFER_INFO csbInfo;

struct tree {
    int inf;
    tree* left;
    tree* right;
    tree* parent;
};

// создаем узел
tree* node(int x) {
    tree* n = new tree;
    n->inf = x;
    n->left = n->right = n->parent = NULL;
    return n;
}

// функция вставки элемента в дерево
void insert(tree*& tr, int x) {
    tree* n = node(x);
    if (!tr) tr = n; // корень - если дерево пустое
    else {
        tree* y = tr;
        while (y) { // 
            if (n->inf > y->inf) { // правая часть
                if (y->right) y = y->right;
                else {
                    n->parent = y; // теперь правый ребенок
                    y->right = n;
                    break;
                }
            }
            else if (n->inf < y->inf) { // левая часть
                if (y->left) y = y->left;
                else {
                    n->parent = y; // теперь левый ребенок
                    y->left = n;
                    break;
                }
            }
            else {
                delete n;
                break;
            }
        }
    }
}

// ищем элемент в дереве
tree* find(tree* tr, int x) {
    if (!tr || tr->inf == x) return tr; // нашли элемент или дошли до конца ветки
    if (x < tr->inf)
        return find(tr->left, x); // ищем слева
    else
        return find(tr->right, x); //ищем справа
}

// поиск минимального элемента
tree* minimum(tree* tr) {
    if (!tr->left) return tr; // нет левого ребенка
    else return minimum(tr->left); // идем по левой части до конца
}

// поиск максимального элемента
tree* maximum(tree* tr) {
    if (!tr->right) return tr; // нет правого ребенка
    else return maximum(tr->right); // идем по правой части до конца minimum
}

// поиск следующего элемента
tree* Next(tree* tr, int x) {
    tree* n = find(tr, x);
    if (!n) return NULL;

    if (n->right) // если есть правый ребенок
        return minimum(n->right); // minimum по правой части

    tree* y = n->parent; // родитель
    while (y && n == y->right) { // пока не дошли до корня или узел = правый ребенок
        n = y; // идем вверх по дереву
        y = y->parent;
    }
    return y; // возвращаем родител¤
}

// удаление узла из дерева
void Delete(tree*& tr, tree* v) {
    if (!v) return;

    tree* p = v->parent;

    // удаление листа
    if (!v->left && !v->right) {
        if (!p) tr = NULL;
        else {
            if (p->left == v) p->left = NULL;
            else p->right = NULL;
        }
        delete v;
    }
    // удаление узла с одним ребенком
    else if (!v->left || !v->right) {
        tree* child = v->left ? v->left : v->right;
        if (p) {
            if (p->left == v) p->left = child;
            else p->right = child;
            child->parent = p;
        }
        else {
            tr = child;
            child->parent = NULL;
        }
        delete v;
    }
    // удаление узла с двумя детьми
    else {
        tree* succ = minimum(v->right); // следующий за удаляемым узлом
        v->inf = succ->inf; // присваиваем значение
        Delete(tr, succ);
    }
}

// определение максимальной высоты дерева
void max_height(tree* x, short& maximum, short deepness = 1) {
    if (deepness > maximum) maximum = deepness;
    if (x->left) max_height(x->left, maximum, deepness + 1);
    if (x->right) max_height(x->right, maximum, deepness + 1);
}

// проверка размера консоли
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

// вспомогательна¤ функци¤ дл¤ вывода дерева
void print_helper(tree* x, const COORD pos, const short offset) {
    SetConsoleCursorPosition(outp, pos);
    cout << right << setw(offset + 1) << x->inf;
    if (x->left) print_helper(x->left, { pos.X, short(pos.Y + 1) }, offset >> 1);
    if (x->right) print_helper(x->right, { short(pos.X + offset), short(pos.Y + 1) }, offset >> 1);
}

// вывод дерева в консоль
void printTree(tree* tr) {
    if (tr) {
        short maximum = 1;
        max_height(tr, maximum);
        short width = 1 << (maximum + 1);
        short max_w = 128;
        if (width > max_w) width = max_w;

        while (!isSizeOfConsoleCorrect(width, maximum)) {
            system("pause");
        }

        for (short i = 0; i < maximum; ++i) cout << '\n';

        GetConsoleScreenBufferInfo(outp, &csbInfo);
        COORD endPos = csbInfo.dwCursorPosition;

        print_helper(tr, { 0, short(endPos.Y - maximum) }, width >> 1);

        SetConsoleCursorPosition(outp, endPos);
    }
    else {
        cout << "The tree is empty." << endl;
    }
}

// прямой обход (корень-левый-правый)
void preorder(tree* tr) {
    if (tr) {
        cout << tr->inf << " "; // корень
        preorder(tr->left); // левая ветка
        preorder(tr->right); // правая ветка
    }
}

// симметричный обход (левый-корень-правый)
void inorder(tree* tr) {
    if (tr) {
        inorder(tr->left); // левая ветка
        cout << tr->inf << " "; // корень
        inorder(tr->right); // правая ветка
    }
}

//обратный обход (левый-правый-корень)
void postorder(tree* tr) {
    if (tr) {
        postorder(tr->left); // левая ветка
        postorder(tr->right); // правая ветка
        cout << tr->inf << " "; // корень
    }
}

int main() {
    setlocale(LC_ALL, "RU");
    tree* tr = NULL;
    int n, x;

    // 45 23 67 12 34 56 78 5 89 41
    cout << "n: ";
    cin >> n;
    cout << "Введите " << n << " элементов: ";
    for (int i = 0; i < n; ++i) {
        cin >> x;
        insert(tr, x);
    }

    cout << "\ntree:" << endl;
    printTree(tr);

    cout << "\n\nпрямой обход (preorder): ";
    preorder(tr);

    cout << "\nсимметричный обход (inorder): ";
    inorder(tr);

    cout << "\nобратный обход (postorder): ";
    postorder(tr);

    cout << "\n\nвставка элемента" << endl;
    cout << "введите элемент для вставки: ";
    cin >> x;
    insert(tr, x);
    cout << "Элемент " << x << " вставлен в дерево." << endl;
    cout << "Дерево после вставки:" << endl;
    printTree(tr);

    cout << "\n\nудаление элемента" << endl;
    cout << "введите элемент для удаления: ";
    cin >> x;

    tree* toDelete = find(tr, x);
    if (toDelete) {
        Delete(tr, toDelete);
        cout << "Элемент " << x << " удален из дерева." << endl;
        cout << "Дерево после удаления:" << endl;
        printTree(tr);
    }
    else {
        cout << "Элемент " << x << " не найден в дереве!" << endl;
    }

    return 0;
}