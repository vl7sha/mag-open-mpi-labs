#include <iostream>
#include <string>
#include <vector>
#include <unordered_set>
#include <chrono>
#include <omp.h>
#include <locale>

using namespace std;

// --- Английские гласные ---
unordered_set<char> getEnglishVowels() {
    return {'A','E','I','O','U','a','e','i','o','u'};
}

// --- Русские гласные ---
unordered_set<wchar_t> getRussianVowels() {
    return {L'А',L'Е',L'Ё',L'И',L'О',L'У',L'Ы',L'Э',L'Ю',L'Я',
            L'а',L'е',L'ё',L'и',L'о',L'у',L'ы',L'э',L'ю',L'я'};
}

// --- Последовательный поиск (английский) ---
unordered_set<char> findMissingVowelsSequential(
        const vector<string>& text, const unordered_set<char>& vowels)
{
    unordered_set<char> found;

    for (const auto& line : text) {
        for (char c : line) {
            if (vowels.count(c)) {
                found.insert(c);
            }
        }
    }

    unordered_set<char> missing;
    for (char v : vowels) {
        if (!found.count(v)) missing.insert(v);
    }
    return missing;
}

// --- Параллельный поиск (английский) ---
unordered_set<char> findMissingVowelsParallel(
        const vector<string>& text, const unordered_set<char>& vowels)
{
    vector<unordered_set<char>> local_found(omp_get_max_threads());

    #pragma omp parallel for
    for (int i = 0; i < (int)text.size(); ++i) {
        int tid = omp_get_thread_num();
        for (char c : text[i]) {
            if (vowels.count(c)) {
                local_found[tid].insert(c);
            }
        }
    }

    unordered_set<char> found;
    for (const auto& s : local_found) found.insert(s.begin(), s.end());

    unordered_set<char> missing;
    for (char v : vowels) {
        if (!found.count(v)) missing.insert(v);
    }
    return missing;
}

// --- Последовательный поиск (русский) ---
unordered_set<wchar_t> findMissingVowelsSequentialW(
        const vector<wstring>& text, const unordered_set<wchar_t>& vowels)
{
    unordered_set<wchar_t> found;

    for (const auto& line : text) {
        for (wchar_t c : line) {
            if (vowels.count(c)) {
                found.insert(c);
            }
        }
    }

    unordered_set<wchar_t> missing;
    for (wchar_t v : vowels) {
        if (!found.count(v)) missing.insert(v);
    }
    return missing;
}

// --- Параллельный поиск (русский) ---
unordered_set<wchar_t> findMissingVowelsParallelW(
        const vector<wstring>& text, const unordered_set<wchar_t>& vowels)
{
    vector<unordered_set<wchar_t>> local_found(omp_get_max_threads());

    #pragma omp parallel for
    for (int i = 0; i < (int)text.size(); ++i) {
        int tid = omp_get_thread_num();
        for (wchar_t c : text[i]) {
            if (vowels.count(c)) {
                local_found[tid].insert(c);
            }
        }
    }

    unordered_set<wchar_t> found;
    for (const auto& s : local_found) found.insert(s.begin(), s.end());

    unordered_set<wchar_t> missing;
    for (wchar_t v : vowels) {
        if (!found.count(v)) missing.insert(v);
    }
    return missing;
}

// --- Вывод для английских гласных ---
void printResult(const unordered_set<char>& missing, const string& lang) {
    if (missing.empty()) {
        cout << "[" << lang << "] Все гласные присутствуют в тексте.\n";
    } else {
        cout << "[" << lang << "] Отсутствующие гласные: ";
        for (char c : missing) cout << c << " ";
        cout << "\n";
    }
}

// --- Вывод для русских гласных ---
void printResultW(const unordered_set<wchar_t>& missing, const wstring& lang) {
    if (missing.empty()) {
        wcout << L"[" << lang << L"] Все гласные присутствуют в тексте.\n";
    } else {
        wcout << L"[" << lang << L"] Отсутствующие гласные: ";
        for (wchar_t c : missing) wcout << c << L" ";
        wcout << L"\n";
    }
}

// --- Тест для английского текста ---
void testEnglishVowels(const vector<string>& text) {
    auto vowels = getEnglishVowels();

    auto start_seq = chrono::high_resolution_clock::now();
    auto missing_seq = findMissingVowelsSequential(text, vowels);
    auto end_seq = chrono::high_resolution_clock::now();

    auto start_par = chrono::high_resolution_clock::now();
    auto missing_par = findMissingVowelsParallel(text, vowels);
    auto end_par = chrono::high_resolution_clock::now();

    printResult(missing_seq, "English");

    chrono::duration<double> dur_seq = end_seq - start_seq;
    chrono::duration<double> dur_par = end_par - start_par;

    cout << "⏱ Sequential: " << dur_seq.count() << " s\n";
    cout << "⏱ Parallel:   " << dur_par.count() << " s\n";

    if (missing_seq == missing_par) {
        cout << "✅ Results match.\n";
    } else {
        cout << "❌ Results differ!\n";
    }
}

// --- Тест для русского текста ---
void testRussianVowels(const vector<wstring>& text) {
    auto vowels = getRussianVowels();

    auto start_seq = chrono::high_resolution_clock::now();
    auto missing_seq = findMissingVowelsSequentialW(text, vowels);
    auto end_seq = chrono::high_resolution_clock::now();

    auto start_par = chrono::high_resolution_clock::now();
    auto missing_par = findMissingVowelsParallelW(text, vowels);
    auto end_par = chrono::high_resolution_clock::now();

    printResultW(missing_seq, L"Русский");

    chrono::duration<double> dur_seq = end_seq - start_seq;
    chrono::duration<double> dur_par = end_par - start_par;

    wcout << L"⏱ Последовательно: " << dur_seq.count() << L" секунд\n";
    wcout << L"⏱ Параллельно:     " << dur_par.count() << L" секунд\n";

    if (missing_seq == missing_par) {
        wcout << L"✅ Результаты совпадают.\n";
    } else {
        wcout << L"❌ Результаты отличаются!\n";
    }
}

int main() {
    // Устанавливаем локаль для корректного вывода русских символов
    setlocale(LC_ALL, "");

    // Пример английского текста
    vector<string> englishText = {
        "The quick brown fox jumps over the lazy dog",
        "This sentence contains almost every vowel"
    };

    // Пример русского текста (wstring + L"")
    vector<wstring> russianText = {
        L"Съешь ещё этих мягких французских булок, да выпей чаю",
        L"В этой строке содержатся почти все русские гласные"
    };

    testEnglishVowels(englishText);
    wcout << L"\n";
    testRussianVowels(russianText);

    return 0;
}
