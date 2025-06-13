#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <omp.h>

using namespace std;

// Функция для поиска максимального элемента на заданной диагонали
int findMaxInDiagonal(const vector<vector<int>>& matrix, int diagonalIndex) {
    int maxElement = INT16_MIN; // Инициализируем минимальным возможным значением
    int n = matrix.size();
    for (int i = 0; i < n; ++i) {
        int j = diagonalIndex - i;
        if (j >= 0 && j < n) {
            if (matrix[i][j] > maxElement) {
                maxElement = matrix[i][j];
            }
        }
    }
    return maxElement;
}

int main() {
    int n;
    cout << "Enter matrix volume: ";
    cin >> n;

    // Инициализация матрицы случайными значениями
    vector<vector<int>> matrix(n, vector<int>(n));
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            matrix[i][j] = rand() % 100; // Заполняем случайными числами от 0 до 99
        }
    }

    // Вывод матрицы
    cout << "Matrix:\n";
    for (const auto& row : matrix) {
        for (int value : row) {
            cout << value << " ";
        }
        cout << endl;
    }

    // Замер времени выполнения без распараллеливания
    auto start = chrono::high_resolution_clock::now();
    vector<int> maxElementsSequential(2 * n - 1);
    for (int i = 0; i < 2 * n - 1; ++i) {
        maxElementsSequential[i] = findMaxInDiagonal(matrix, i);
    }
    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::nanoseconds>(end - start);

    cout << "Max elements on diagonals (consecutive):\n";
    for (int i = 0; i < 2 * n - 1; ++i) {
        cout << "Diagonal " << i << ": " << maxElementsSequential[i] << endl;
    }
    cout << "Ex. time (consecutive): " << duration.count() << " nanoseconds\n";

    // Замер времени выполнения с распараллеливанием
    start = chrono::high_resolution_clock::now();

    vector<int> maxElementsParallel(2 * n - 1, INT16_MIN); // Инициализация значениями INT_MIN
    omp_set_num_threads(6);
#pragma omp parallel
    {
        int numThreads = omp_get_num_threads();
        int threadId = omp_get_thread_num();
        printf("threads=%d num=%d\n", numThreads, threadId);
        // Локальный массив для хранения максимальных значений каждой нити
        vector<int> localMax(2 * n - 1, INT16_MIN);

        // Каждая нить обрабатывает свои диагонали
        for (int i = threadId; i < 2 * n - 1; i += numThreads) {
            localMax[i] = findMaxInDiagonal(matrix, i);
        }

        // Объединение результатов из локального массива в общий массив
#pragma omp critical
        {
            for (int i = 0; i < 2 * n - 1; ++i) {
                if (localMax[i] > maxElementsParallel[i]) {
                    maxElementsParallel[i] = localMax[i];
                }
            }
        }
    }

    end = chrono::high_resolution_clock::now();
    duration = chrono::duration_cast<chrono::nanoseconds>(end - start);

    cout << "Max elements on diagonals (parallel):\n";
    for (int i = 0; i < 2 * n - 1; ++i) {
        cout << "Diagonal " << i << ": " << maxElementsParallel[i] << endl;
    }
    cout << "Ex. time (parallel): " << duration.count() << " nanoseconds\n";

    return 0;
}
