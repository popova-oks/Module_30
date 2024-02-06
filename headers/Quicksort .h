#include <iostream>
#include <vector>
#include <algorithm>
#include <future>
#include <memory>
#include <chrono>
#include "ThreadPool.h"

using namespace std;

class Quicksort {
private:
    ThreadPool& pool_;
    int* array_;
    long left_;
    long right_;
    int max_elements_;
public:
    Quicksort(ThreadPool& pool, int* array, long left, long right, int max_elements)
        : pool_(pool), array_(array), left_(left), right_(right), max_elements_(max_elements) {}

    void operator()() {
        quicksort(array_, left_, right_);
    }
    void quicksort (int* array, long left, long right);
};
