#include <iostream>
using namespace std;    

void countingSort(int mas[], int n) {
    if (n <= 0) return;
    // Нахождение минимума и максимума
    int minVal = mas[0], maxVal = mas[0];
    for (int i = 1; i < n; i++) {
        if (mas[i] < minVal) minVal = mas[i];
        if (mas[i] > maxVal) maxVal = mas[i];
    }
    int range = maxVal - minVal + 1;
    // Массив счётчиков и заполняем нулями
    int* count = new int[range]();
    int* output = new int[n];
    // Подсчёт частот
    for (int i = 0; i < n; i++) {
        count[mas[i] - minVal]++;
    }
    for (int i = 1; i < range; i++) {
        count[i] += count[i - 1];
    }
    // Заполнение выходного массива справа налево
    for (int i = n - 1; i >= 0; i--) {  
        int pos = count[mas[i] - minVal] - 1;
        output[pos] = mas[i];
        count[mas[i] - minVal]--;
    }
    for (int i = 0; i < n; i++) {
        mas[i] = output[i];
    }

    delete[] count;
    delete[] output;
}
void printMas(int mas[], int n) {
    for (int i = 0; i < n; i++) {
        cout << mas[i] << " ";
    }
    cout << '\n';
}


int getD(int num, int p) {
    for (int i = 0; i < p; i++) num /= 10;
    return num % 10;
}
// Сортировка подсчётом по указанному разряду
void countingSortByDigit(int* mas, int n, int p) {
    const int BASE = 10; // десятичная система
    int* output = new int[n];
    int count[BASE] = { 0 };
    // Подсчёт количества элементов с каждым значением цифры в разряде p
    for (int i = 0; i < n; i++) {
        int digit = getD(mas[i], p);
        count[digit]++;
    }
    // count в массив префиксных сумм
    for (int i = 1; i < BASE; i++) {
        count[i] += count[i - 1];
    }
    // Заполнение выходного массива справа налево
    for (int i = n - 1; i >= 0; i--) {
        int digit = getD(mas[i], p);
        output[count[digit] - 1] = mas[i];
        count[digit]--;
    }
    for (int i = 0; i < n; i++) {
        mas[i] = output[i];
    }

    delete[] output;
}
// Сама поразрядная сортировка
void radixSort(int* mas, int n) {
    // Находим максимальное число чтобы узнать количество разрядов
    int maxVal = mas[0];
    for (int i = 1; i < n; i++) {
        if (mas[i] > maxVal) maxVal = mas[i];
    }
    // Определяем количество разрядов у максимального числа
    int maxDigits = 0;
    while (maxVal > 0) {
        maxDigits++;
        maxVal /= 10;
    }
    // Если все числа нули, то сортировка не нужна
    if (maxDigits == 0) maxDigits = 0;

    // Для каждого разряда от младшего к старшему
    for (int p = 0; p < maxDigits; p++) {
        countingSortByDigit(mas, n, p);
    }
}

int main()
{
    int mas[] = {4, 3, 3, -2, -2, 6, 3, -2, 8, 0};
    int lenMas = sizeof(mas) / sizeof(mas[0]);
    printMas(mas, lenMas);

    cout << "Counting sort\n";
    countingSort(mas, lenMas);
    printMas(mas, lenMas);


    cout << "Discharge sort\n";
    int mas2[] = { 2, 12, 4, 46, 7, 51, 2, 22, 706, 908, 1, 132 };
    int lenMas2 = sizeof(mas2) / sizeof(mas2[0]);
    radixSort(mas2, lenMas2);
    printMas(mas2, lenMas2);

}

