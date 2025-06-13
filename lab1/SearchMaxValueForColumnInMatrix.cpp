#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <omp.h>
#include <stdio.h>

using namespace std;
using namespace std::chrono;

// Функция для поиска максимальных элементов в столбцах без распараллеливания
vector<int> findMaxInColumnsSequential(const vector<vector<int>>& matrix) {
    int m = matrix.size();
    int n = matrix[0].size();
    vector<int> maxElements(n);

    for (int j = 0; j < n; ++j) {
        maxElements[j] = matrix[0][j];
        for (int i = 1; i < m; ++i) {
            if (matrix[i][j] > maxElements[j]) {
                maxElements[j] = matrix[i][j];
            }
        }
    }

    return maxElements;
}

// Функция для поиска максимальных элементов в столбцах с распараллеливанием OpenMP
vector<int> findMaxInColumnsParallel(const vector<vector<int>>& matrix) {
    int m = matrix.size();
    int n = matrix[0].size();
    vector<int> maxElements(n);

#pragma omp parallel for
    for (int j = 0; j < n; ++j) {
        int max_val = matrix[0][j];
        for (int i = 1; i < m; ++i) {
            if (matrix[i][j] > max_val) {
                max_val = matrix[i][j];
            }
        }
        maxElements[j] = max_val;
        /*int threads = omp_get_num_threads();
        printf("threads=%d\n", threads);*/
    }

    return maxElements;
}

int main() {
    // Размеры матрицы
    int m = 10;
    int n = 10;

    // Инициализация матрицы случайными числами
    vector<vector<int>> matrix(m, vector<int>(n));
    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < n; ++j) {
            matrix[i][j] = rand() % 1000; // случайные числа от 0 до 999
        }
    }

    cout << "Matrix:" << endl;
    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < n; ++j) {
            cout << matrix[i][j] << " ";
        }
        cout << endl;
    }
    cout << endl;

    // Замер времени выполнения без распараллеливания
    auto startSequential = high_resolution_clock::now();
    vector<int> maxElementsSequential = findMaxInColumnsSequential(matrix);
    auto endSequential = high_resolution_clock::now();
    auto durationSequential = duration_cast<nanoseconds>(endSequential - startSequential); // Изменил на nanoseconds

    // Замер времени выполнения с распараллеливанием
    auto startParallel = high_resolution_clock::now();
    vector<int> maxElementsParallel = findMaxInColumnsParallel(matrix);
    auto endParallel = high_resolution_clock::now();
    auto durationParallel = duration_cast<nanoseconds>(endParallel - startParallel); // Изменил на nanoseconds

    // Вывод результатов
    cout << "Sequential execution time: " << durationSequential.count() << " ns" << endl; // Изменил единицу измерения
    cout << "Parallel execution time: " << durationParallel.count() << " ns" << endl; // Изменил единицу измерения

    // Вывод максимальных элементов в столбцах (последовательная версия)
    cout << "Max elements in columns (sequential):" << endl;
    for (int i = 0; i < n; ++i) {
        cout << maxElementsSequential[i] << " ";
    }
    cout << endl;

    // Вывод максимальных элементов в столбцах (параллельная версия)
    cout << "Max elements in columns (parallel):" << endl;
    for (int i = 0; i < n; ++i) {
        cout << maxElementsParallel[i] << " ";
    }
    cout << endl;

    // Проверка корректности результатов
    bool correct = true;
    for (int i = 0; i < n; ++i) {
        if (maxElementsSequential[i] != maxElementsParallel[i]) {
            correct = false;
            break;
        }
    }

    if (correct) {
        cout << "Results are consistent." << endl;
    }
    else {
        cout << "Results are inconsistent." << endl;
    }

    return 0;
}
