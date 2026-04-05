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

vector<int> zfunction(string s) {
    int n = (int)s.size();
    vector<int> z(n, 0);
    int l = 0, r = 0;

    for (int i = 1; i < n; i++) {
        // Если текущая позиция внутри уже найденного отрезка
        if (i <= r)
            z[i] = min(r - i + 1, z[i - l]);
        while (i + z[i] < n && s[z[i]] == s[i + z[i]])
            z[i]++;
        // Если новый Z-отрезок длиннее текущего, обновляем границы
        if (i + z[i] - 1 > r) {
            r = i + z[i] - 1;
            l = i;
        }
    }

    return z;
}

int main() {
    setlocale(LC_ALL, "RUS");

    string str;
    cout << "Введите строку: ";
    getline(cin, str);

    vector<int> pref = prefix(str);
    cout << "Prefix: ";
    for (int i = 0; i < pref.size(); i++) {
        cout << pref[i] << " ";
    }
    cout << endl;

    vector<int> z = zfunction(str);
    cout << "Z-function: ";
    for (int i = 0; i < z.size(); i++) {
        cout << z[i] << " ";
    }
    cout << endl;
}