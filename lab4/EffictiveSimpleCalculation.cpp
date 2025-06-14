#include <iostream>
#include <cmath>
#include <omp.h>
#include <chrono>

// Функция для вычисления f(x) с заданной точностью
double f(double x, double epsilon) {
    double sum = 0.0;
    double term = 1.0; // Первый член ряда
    int n = 1;

    while (fabs(term) > epsilon) {
        term = pow(-1, n) / (tgamma(2 * n + 1) * (2 * n)) * pow(x, 2 * n);
        sum += term;
        n++;
    }

    return sum;
}

// Последовательный метод трапеций
double trapezoidal_rule_seq(double a, double b, int N, double epsilon) {
    double h = (b - a) / N;
    double sum = 0.0;

    // Вычисляем значения f(a) и f(b)
    double fa = f(a, epsilon);
    double fb = f(b, epsilon);

    // Суммируем значения f(x_i) для i = 1, ..., N-1
    for (int i = 1; i < N; ++i) {
        double xi = a + i * h;
        sum += f(xi, epsilon);
    }

    // Формула метода трапеций
    return h * (fa + 2 * sum + fb) / 2.0;
}

// Параллельный метод трапеций с OpenMP
double trapezoidal_rule_par(double a, double b, int N, double epsilon) {
    double h = (b - a) / N;
    double sum = 0.0;

    #pragma omp parallel for reduction(+ : sum)
    for (int i = 1; i < N; ++i) {
        double xi = a + i * h;
        sum += f(xi, epsilon);
    }

    // Вычисляем значения f(a) и f(b)
    double fa = f(a, epsilon);
    double fb = f(b, epsilon);

    // Формула метода трапеций
    return h * (fa + 2 * sum + fb) / 2.0;
}

int main() {
    // Параметры задачи
    double a = 0.0; // Левая граница
    double b = 10.0; // Правая граница
    int N = 1; // Количество отрезков разбиения
    double epsilon = 1e-6; // Точность для ряда

    // Измерение времени для последовательного метода
    auto start_seq = std::chrono::high_resolution_clock::now();
    double result_seq = trapezoidal_rule_seq(a, b, N, epsilon);
    auto end_seq = std::chrono::high_resolution_clock::now();

    // Измерение времени для параллельного метода
    auto start_par = std::chrono::high_resolution_clock::now();
    double result_par = trapezoidal_rule_par(a, b, N, epsilon);
    auto end_par = std::chrono::high_resolution_clock::now();

    // Расчет времени выполнения
    auto duration_seq = std::chrono::duration_cast<std::chrono::milliseconds>(end_seq - start_seq).count();
    auto duration_par = std::chrono::duration_cast<std::chrono::milliseconds>(end_par - start_par).count();

    // Вывод результатов
    std::cout << "Последовательный метод:" << std::endl;
    std::cout << "  Результат: " << result_seq << std::endl;
    std::cout << "  Время выполнения: " << duration_seq << " мс" << std::endl;

    std::cout << "Параллельный метод:" << std::endl;
    std::cout << "  Результат: " << result_par << std::endl;
    std::cout << "  Время выполнения: " << duration_par << " мс" << std::endl;

    return 0;
}