#include <iostream>
#include <cmath>
#include <string>
#include <map>
#include <omp.h>

using namespace std;

// Функция, которую будем интегрировать
double f(double x, const string& func) {
    if (func == "x*x") return x * x;
    else if (func == "sin(x)") return sin(x);
    else if (func == "cos(x)") return cos(x);
    else if (func == "exp(x)") return exp(x);
    else if (func == "sqrt(x)") return sqrt(x);
    else if (func == "1/(1+x*x)") return 1.0 / (1.0 + x * x);
    else {
        cerr << "Unknown function. Using default: x*x\n";
        return x * x;
    }
}

// Последовательное вычисление
double integrateSequential(double a, double b, int n, const string& func) {
    double h = (b - a) / n;
    double sum = 0.0;

    for (int i = 1; i <= n; ++i) {
        double x = a + i * h;
        sum += f(x, func);
    }

    return sum * h;
}

// Параллельное вычисление с OpenMP
double integrateParallel(double a, double b, int n, const string& func) {
    double h = (b - a) / n;
    double sum = 0.0;

#pragma omp parallel for reduction(+:sum)
    for (int i = 1; i <= n; ++i) {
        double x = a + i * h;
        sum += f(x, func);
    }

    return sum * h;
}

int main() {
    // Список поддерживаемых функций
    map<int, string> functions = {
        {1, "x*x"},
        {2, "sin(x)"},
        {3, "cos(x)"},
        {4, "exp(x)"},
        {5, "sqrt(x)"},
        {6, "1/(1+x*x)"}
    };

    // Ввод данных от пользователя
    int choice;
    cout << "Выберите функцию для интегрирования:\n";
    for (const auto& [key, val] : functions) {
        cout << key << ". " << val << endl;
    }
    cout << "Введите номер функции: ";
    cin >> choice;

    string func = functions[choice];
    if (func.empty()) {
        cerr << "Неверный выбор функции!" << endl;
        return -1;
    }

    double a, b;
    int n;

    cout << "Введите начало интервала a: ";
    cin >> a;

    cout << "Введите конец интервала b: ";
    cin >> b;

    cout << "Введите количество разбиений n: ";
    cin >> n;

    if (n <= 0 || b <= a) {
        cerr << "Ошибка: n должно быть положительным, и b > a." << endl;
        return -1;
    }

    double start_time, end_time;

    // Последовательная версия
    cout << "\n--- Последовательное вычисление ---\n";
    start_time = omp_get_wtime();
    double result_seq = integrateSequential(a, b, n, func);
    end_time = omp_get_wtime();

    cout << "Результат (последовательно): " << result_seq << endl;
    cout << "Время выполнения: " << end_time - start_time << " секунд\n";

    // Параллельная версия
    cout << "\n--- Параллельное вычисление ---\n";
    start_time = omp_get_wtime();
    double result_par = integrateParallel(a, b, n, func);
    end_time = omp_get_wtime();

    cout << "Результат (параллельно): " << result_par << endl;
    cout << "Время выполнения: " << end_time - start_time << " секунд\n";

    // Разница между результатами
    cout << "Разница между результатами: " << abs(result_par - result_seq) << endl;

    return 0;
}