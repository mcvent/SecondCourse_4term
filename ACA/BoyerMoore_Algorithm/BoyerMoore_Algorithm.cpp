#include <algorithm>
#include <iostream>
#include <string>

#define NO_OF_CHARS 256

using namespace std;

// Построение таблицы стоп-символов
void badChar(const string& str, int size, int badchar[NO_OF_CHARS])
{
    // Инициализируем все символы как не встречающиеся (-1)
    for (int i = 0; i < NO_OF_CHARS; i++)
        badchar[i] = -1;

    // Запоминаем последнюю позицию каждого символа в образце
    for (int i = 0; i < size; i++)
        badchar[(int)str[i]] = i;
}

// Поиск всех вхождений образца pat в текст txt
void search(const string& txt, const string& pat)
{
    int m = pat.size();
    int n = txt.size();

    int badchar[NO_OF_CHARS];
    badChar(pat, m, badchar);

    int s = 0; // текущий сдвиг образца относительно текста

    cout << "Текст:    " << txt << "\n";
    cout << "Образец:  " << pat << "\n\n";
    cout << "Начинаем поиск:\n";

    while (s <= (n - m))
    {
        int j = m - 1;

        // Сравниваем справа налево
        while (j >= 0 && pat[j] == txt[s + j])
            j--;

        if (j < 0)
        {
            // Нашли вхождение
            cout << "Образец найден на позиции " << s << "\n";

            // Сдвигаем, чтобы проверить перекрывающиеся вхождения
            s += (s + m < n) ? m - badchar[txt[s + m]] : 1;
        }
        else
        {
            // Несовпадение на позиции j
            int badCharShift = j - badchar[txt[s + j]];
            int shift = max(1, badCharShift);
            cout << "\nНесовпадение на позиции j=" << j
                << " (символ текста '" << txt[s + j]
                << "'), сдвиг = " << shift << "\n";
            s += shift;
            cout << txt << "\n";
            cout << string(s, ' ') << pat << "\n";
        }
    }
}

int main()
{
    setlocale(LC_ALL, "RUS");

    // Ваши данные
    string txt = "BABAABCBACBADB";
    string pat = "BACBA";

    search(txt, pat);

    return 0;
}