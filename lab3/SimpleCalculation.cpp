#include <iostream>
#include <vector>
#include <cmath>
#include <omp.h>

using namespace std;

// Function to calculate area sequentially
double calculateAreaSequential(const vector<pair<double, double>>& vertices) {
    double area = 0.0;
    int n = vertices.size();

    for (int i = 0; i < n; ++i) {
        int j = (i + 1) % n; // Next vertex
        area += (vertices[i].first * vertices[j].second) - (vertices[j].first * vertices[i].second);
    }

    return fabs(area) / 2.0;
}

// Function to calculate area in parallel
double calculateAreaParallel(const vector<pair<double, double>>& vertices) {
    double area = 0.0;
    int n = vertices.size();

#pragma omp parallel
    {
        double localArea = 0.0;
        int thread = omp_get_num_threads();
        printf("%d \n", thread);
#pragma omp for
        for (int i = 0; i < n; ++i) {
            int j = (i + 1) % n; // Next vertex
            localArea += (vertices[i].first * vertices[j].second) - (vertices[j].first * vertices[i].second);
        }

#pragma omp atomic
        area += localArea;
    }

    return fabs(area) / 2.0;
}

// Function to get preset vertices
vector<pair<double, double>> getPresetVertices(int choice) {
    switch (choice) {
    case 1: // Triangle
        return { {0, 0}, {4, 0}, {2, 3} };
    case 2: // Square
        return { {0, 0}, {4, 0}, {4, 4}, {0, 4} };
    case 3: // Pentagon
        return { {1, 1}, {2, 4}, {5, 5}, {6, 2}, {3, 0} };
    default:
        return {};
    }
}

int main() {
    int methodChoice;
    cout << "Choose the method to calculate the area:\n";
    cout << "1. Input coordinates manually\n";
    cout << "2. Use preset coordinates\n";
    cout << "Enter your choice (1-2): ";
    cin >> methodChoice;

    vector<pair<double, double>> vertices;

    if (methodChoice == 1) {
        int n;
        cout << "Enter the number of vertices of the polygon: ";
        cin >> n;

        vertices.resize(n);
        for (int i = 0; i < n; ++i) {
            cout << "Enter coordinates of vertex " << i + 1 << " (x y): ";
            cin >> vertices[i].first >> vertices[i].second;
        }
    }
    else if (methodChoice == 2) {
        int presetChoice;
        cout << "Choose a preset polygon:\n";
        cout << "1. Triangle\n";
        cout << "2. Square\n";
        cout << "3. Pentagon\n";
        cout << "Enter your choice (1-3): ";
        cin >> presetChoice;

        vertices = getPresetVertices(presetChoice);

        if (vertices.empty()) {
            cout << "Invalid choice! Exiting program." << endl;
            return -1;
        }
    }
    else {
        cout << "Invalid choice! Exiting program." << endl;
        return -1;
    }

    // Measure execution time for sequential version
    double startTime = omp_get_wtime();
    double areaSequential = calculateAreaSequential(vertices);
    double endTime = omp_get_wtime();

    cout << "Polygon area (sequential): " << areaSequential << endl;
    cout << "Execution time (sequential): " << endTime - startTime << " seconds" << endl;

    // Measure execution time for parallel version
    startTime = omp_get_wtime();
    double areaParallel = calculateAreaParallel(vertices);
    endTime = omp_get_wtime();

    cout << "Polygon area (parallel): " << areaParallel << endl;
    cout << "Execution time (parallel): " << endTime - startTime << " seconds" << endl;

    return 0;
}
