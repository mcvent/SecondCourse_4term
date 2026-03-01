#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <algorithm>
#include <iomanip>

using namespace std;
using namespace std::chrono;

// объявляем глобальный генератор псевдослучайных чисел, работает быстро
static mt19937 rng(random_device{}());

vector<int> generateRandomArray(int size) {
    vector<int> mas(size);
    uniform_int_distribution<> distrib(0, 1000000); // равномерное распределение
    for (int i = 0; i < size; ++i) {
        mas[i] = distrib(rng);
    }
    return mas;
}

// быстрая сортировка
int partition(vector<int>& mas, int low, int high) {
    uniform_int_distribution<int> distrib(low, high);
    int pivotIndex = distrib(rng); // случайный индекс опорника
    swap(mas[pivotIndex], mas[high]);
    int pivot = mas[high];
    int i = (low - 1);
    for (int j = low; j <= high - 1; j++) {
        if (mas[j] < pivot) {
            i++;
            swap(mas[i], mas[j]);
        }
    }
    swap(mas[i + 1], mas[high]);
    return (i + 1);
}

void quick_recursive(vector<int>& mas, int low, int high) {
    if (low < high) {
        int pi = partition(mas, low, high);
        quick_recursive(mas, low, pi - 1);
        quick_recursive(mas, pi + 1, high);
    }
}

void quick_sort(vector<int>& mas, int size) {
    if (size > 0) quick_recursive(mas, 0, size - 1);
}

// сортировка слиянием
void merge(vector<int>& mas, int left, int mid, int right, vector<int>& tmp) {
    int i = left, j = mid + 1, k = left;
    while (i <= mid && j <= right) {
        if (mas[i] <= mas[j]) tmp[k++] = mas[i++];
        else tmp[k++] = mas[j++];
    }
    // переносим оставшиеся
    while (i <= mid) tmp[k++] = mas[i++];
    while (j <= right) tmp[k++] = mas[j++];
    // возвращение к mas
    for (i = left; i <= right; i++) mas[i] = tmp[i];
}

void merge_recursive(vector<int>& mas, int left, int right, vector<int>& tmp) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        merge_recursive(mas, left, mid, tmp);
        merge_recursive(mas, mid + 1, right, tmp);
        merge(mas, left, mid, right, tmp);
    }
}

void merge_sort(vector<int>& mas, int size) {
    if (size > 0) {
        vector<int> tmp(size);
        merge_recursive(mas, 0, size - 1, tmp);
    }
}

// пирамидальная сортировка
// просеиваем кучу
void heap_help(vector<int>& mas, int n, int i) {
    int largest = i;  // предполагаем, что текущий узел — наибольший
    int left = 2 * i + 1;   // индекс левого потомка в куче
    int right = 2 * i + 2;  // индекс правого потомка    
    if (left < n && mas[left] > mas[largest]) largest = left;
    if (right < n && mas[right] > mas[largest]) largest = right;
    // если наибольший — не текущий, меняем местами и продолжаем «просеивать»
    if (largest != i) { 
        swap(mas[i], mas[largest]);
        heap_help(mas, n, largest);
    }
}

void heap_sort(vector<int>& mas, int size) {
    for (int i = size / 2 - 1; i >= 0; i--) 
        heap_help(mas, size, i);
    // извлечение корней по одному
    for (int i = size - 1; i > 0; i--) {
        swap(mas[0], mas[i]);
        heap_help(mas, i, 0); // просеиваем остальные
    }
}

// измерение времени
double timer(void (*sortFunc)(vector<int>&, int), const vector<int>& data, int size) {
    // копируем только нужную часть массива (от 0 до size)
    vector<int> copy(data.begin(), data.begin() + size);

    auto start = high_resolution_clock::now();
    sortFunc(copy, size);
    auto end = high_resolution_clock::now();

    duration<double, milli> duration = end - start;
    return duration.count();
}

int main() {
    // Размеры для проверки
    vector<int> sizes = { 50000, 100000, 500000, 1000000 };
    int runs = 5;

    // матрица для хранения результатов [run][size][algorithm]
    // algorithm: 0=Quick, 1=Merge, 2=Heap
    vector<vector<vector<double>>> results(runs, vector<vector<double>>(sizes.size(), vector<double>(3, 0.0)));

    // запускаем 5 раз, радномным образом создавая массив
    for (int run = 0; run < runs; ++run) {
        // создаем новый массив для каждого запуска
        vector<int> fullArray = generateRandomArray(1000000);

        for (size_t s = 0; s < sizes.size(); ++s) {
            int size = sizes[s];
            // замеряем все 3 алгоритма на одинаковых данных
            results[run][s][0] = timer(quick_sort, fullArray, size);
            results[run][s][1] = timer(merge_sort, fullArray, size);
            results[run][s][2] = timer(heap_sort, fullArray, size);

            cout << "    Size " << setw(7) << size << ": "
                << "Q=" << fixed << setprecision(2) << setw(8) << results[run][s][0] << "ms | "
                << "M=" << fixed << setprecision(2) << setw(8) << results[run][s][1] << "ms | "
                << "H=" << fixed << setprecision(2) << setw(8) << results[run][s][2] << "ms" << endl;
        }
        cout << endl;
    }
    return 0;
}