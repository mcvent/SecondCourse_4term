#include <iostream>
#include <vector>  
#include <string>

using namespace std;

vector<int> prefix(string s) {
    int n = (int)s.size();
    vector<int> p(n, 0);
    for (int i = 1; i < n; i++) {
        // Длина нового префикса не может превышать p[i-1] + 11
        int c = p[i - 1];
        // Уменьшаем c, пока не найдём позицию, где символ s[cur] совпадёт с s[i]
        while (s[i] != s[c] && c > 0)
            c = p[c - 1];
        // Здесь или s[i] == s[cur], или cur == 0
        if (s[i] == s[c])
            p[i] = c + 1; // Увеличиваем длину префикса на 1
    }
    return p;
}

vector<int> KMP_algo(string s, string t) {
    vector<int> A;

    if (s.empty()) return A;

    int k = 0;  // Количество совпавших символов
    vector<int> pi = prefix(s);  // Префикс от образца S
    int n = s.size();
    int m = t.size();

    for (int i = 0; i < m; i++) {
        // Пока есть совпадение и текущий символ не совпадает
        while (k > 0 && t[i] != s[k]) {
            k = pi[k - 1];
        }
        // Если символы совпали
        if (t[i] == s[k]) {
            k++;
        }
        // Если нашли полное вхождение
        if (k == n) {
            A.push_back(i - n + 1); // Позиция начала вхождения
            k = pi[k - 1];
        }
    }

    return A;
}

int main() {
    setlocale(LC_ALL, "RUS");

    string st, text;
    cout << "Введите строку для поиска: ";
    getline(cin, st);
    cout << "Начальная строка: ";
    getline(cin, text);
    vector<int> result = KMP_algo(st, text);

    cout << "Результат работы: ";
    for (int i = 0; i < result.size(); i++) {
        cout << result[i] << " ";
    }
    cout << endl;
}