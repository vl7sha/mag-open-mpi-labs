#include <iostream>
#include <cmath>
#include <chrono>
#include <omp.h>
#include <limits> // Для numeric_limits

// Функция для вычисления интеграла методом Симпсона
double simpsonMethod(double a, double b, int n, double (*f)(double, double)) {
    double h = (b - a) / n;
    double sum = 0.0;

#pragma omp parallel for reduction(+:sum)
    for (int i = 0; i < n; ++i) {
        double x0 = a + i * h;
        double x1 = a + (i + 1) * h;
        sum += f(x0, 1e-6) + 4 * f((x0 + x1) / 2, 1e-6) + f(x1, 1e-6);
    }

    return (h / 6) * sum;
}

// Функция, которую будем интегрировать
double f(double x, double precision) {
    double result = 0.0;
    double term;
    int n = 0;

    if (x == 0.0) {
        return 0.0;
    }

    while (true) {
        term = pow(-1, n) / (tgamma(2 * n + 1) * pow(4.0 * n + 1.0, x) * pow(x, 4 * n + 1));

        if (std::abs(term) < precision) {
            break;
        }

        result += term;
        n++;
    }

    return result;
}

int main() {
    double a; // Нижний предел интегрирования
    double b = 1.0; // Верхний предел интегрирования
    int n; // Количество интервалов
    double precision; // Точность вычисления ряда

    // Запрашиваем у пользователя ввод a, n и precision
    std::cout << "Enter the lower limit of integration (a): ";
    std::cin >> a;
    std::cout << "Enter the number of intervals (n): ";
    std::cin >> n;
    std::cout << "Enter the desired precision: ";
    std::cin >> precision;

    // Проверяем входные данные
    if (n % 2 != 0 || n <= 0) {
        std::cout << "Error: n must be a positive even number." << std::endl;
        return 1;
    }

    if (a < 0) {
        std::cout << "Error: a must be non-negative." << std::endl;
        return 1;
    }

    if (precision <= 0) {
        std::cout << "Error: Precision must be a positive number." << std::endl;
        return 1;
    }

    // Замер времени выполнения без OpenMP
    auto start_no_omp = std::chrono::high_resolution_clock::now();
    double result_no_omp = simpsonMethod(a, b, n, f);
    auto end_no_omp = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration_no_omp = end_no_omp - start_no_omp;

    // Вывод результатов без OpenMP
    std::cout << "Result without OpenMP: " << result_no_omp << std::endl;
    std::cout << "Time without OpenMP: " << duration_no_omp.count() << " s" << std::endl;

    // Замер времени выполнения с OpenMP
    auto start_omp = std::chrono::high_resolution_clock::now();
    double result_omp = simpsonMethod(a, b, n, f);
    auto end_omp = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration_omp = end_omp - start_omp;

    // Вывод результатов с OpenMP
    std::cout << "Result with OpenMP: " << result_omp << std::endl;
    std::cout << "Time with OpenMP: " << duration_omp.count() << " s" << std::endl;

    return 0;
}
