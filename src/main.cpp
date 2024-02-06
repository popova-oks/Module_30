#include <iostream>
#include <thread>
#include <vector>
#include <future>
#include <memory>
#include <chrono>
#include "../headers/ThreadPool.h"
#include "../headers/Quicksort .h"

using namespace std;

int main() {
    constexpr int arr_size = 1000000;
    int* array = new int[arr_size];

    for (int i = 0; i < arr_size; ++i) {
        array[i] = rand() % 1000000;
    }

    ThreadPool threadPool;
    Quicksort qsort(threadPool, array, 0, arr_size - 1, 100000);
    auto start = chrono::high_resolution_clock::now();    
    // Запускаем параллельную сортировку
    qsort();
    auto end = chrono::high_resolution_clock::now();


    for(long i = 0; i < arr_size - 1; i++) { // Проверяем сортировку
        if(array[i] > array[i + 1]) {
            cout << "Unsorted" << endl;
            break;
        }
    }

    chrono::duration<double> elapsed = end - start;
    cout << "Time: " << elapsed.count() << " seconds" << endl;

    delete[] array;
    return 0;
}
