#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <chrono>
#include <omp.h>
#include <random>

using namespace std;

// Function to generate a random string of given length
string generateRandomString(int length) {
    static const char charset[] =
        "abcdefghijklmnopqrstuvwxyz"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "0123456789 ";
    static mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());
    static uniform_int_distribution<size_t> uniform(0, sizeof(charset) - 2);
    string str;
    str.reserve(length);
    for (int i = 0; i < length; ++i) {
        str += charset[uniform(rng)];
    }
    return str;
}

int main() {
    int arraySize = 100; // Увеличим размер массива строк
    int stringLength; // Length of each string

    // Get string length from the user
    cout << "Enter the desired length of the strings: ";
    cin >> stringLength;

    vector<string> textArray(arraySize);

    // Generate the string array
    for (int i = 0; i < arraySize; ++i) {
        textArray[i] = generateRandomString(stringLength);
    }

    // Output the generated string array (optional, remove for performance tests)
    // cout << "Generated String Array:" << endl;
    // for (int i = 0; i < arraySize; ++i) {
    //     cout << "String " << i + 1 << ": " << textArray[i] << endl;
    // }
    // cout << endl;

    // **Without parallelization**
    auto start_serial = chrono::high_resolution_clock::now();
    vector<bool> digitPresentSerial(10, false);
    for (const string& text : textArray) {
        for (char c : text) {
            if (isdigit(c)) {
                digitPresentSerial[c - '0'] = true;
            }
        }
    }

    cout << "Missing digits (without parallelization): ";
    bool allDigitsPresentSerial = true;
    for (int i = 0; i < 10; ++i) {
        if (!digitPresentSerial[i]) {
            cout << i << " ";
            allDigitsPresentSerial = false;
        }
    }
    if (allDigitsPresentSerial) {
        cout << "All digits are present";
    }
    cout << endl;

    auto end_serial = chrono::high_resolution_clock::now();
    auto duration_serial = chrono::duration_cast<chrono::microseconds>(end_serial - start_serial);
    cout << "Execution time (without parallelization): " << duration_serial.count() << " microseconds" << endl;

    // **With parallelization**
    auto start_parallel = chrono::high_resolution_clock::now();
    vector<bool> digitPresentParallel(10, false);

#pragma omp parallel
    {
        vector<bool> localDigitPresent(10, false);

#pragma omp for nowait
        for (int i = 0; i < textArray.size(); ++i) {
            for (char c : textArray[i]) {
                if (isdigit(c)) {
                    localDigitPresent[c - '0'] = true;
                }
            }
        }

#pragma omp critical
        {
            for (int j = 0; j < 10; ++j) {
                digitPresentParallel[j] = digitPresentParallel[j] || localDigitPresent[j];
            }
        }
    }

    cout << "Missing digits (with parallelization): ";
    bool allDigitsPresentParallel = true;
    for (int i = 0; i < 10; ++i) {
        if (!digitPresentParallel[i]) {
            cout << i << " ";
            allDigitsPresentParallel = false;
        }
    }
    if (allDigitsPresentParallel) {
        cout << "All digits are present";
    }
    cout << endl;

    auto end_parallel = chrono::high_resolution_clock::now();
    auto duration_parallel = chrono::duration_cast<chrono::microseconds>(end_parallel - start_parallel);
    cout << "Execution time (with parallelization): " << duration_parallel.count() << " microseconds" << endl;

    return 0;
}
