#include "../headers/Quicksort .h"

void Quicksort::quicksort(int* array, long left, long right) {
    if(left >= right) {
        return;
    }

    long left_bound = left;
    long right_bound = right;
    long middle = array[(left_bound + right_bound) / 2];

    do {
        while(array[left_bound] < middle) {
            left_bound++;
        }
        while(array[right_bound] > middle) {
            right_bound--;
        }

        if(left_bound <= right_bound) {
            std::swap(array[left_bound], array[right_bound]);
            left_bound++;
            right_bound--;
        }
    } while(left_bound <= right_bound);

    if(left_bound < right) {
        if(right_bound - left > 10000) {
            
            auto left_promise = std::make_shared<std::promise<void>>();
            auto left_future = left_promise->get_future().share();

            // Запускаем рекурсивную сортировку в потоке
            auto lambda_task1 = [this, array, left_bound, right, left_promise](int*, long, long) {
               quicksort(array, left_bound, right);
            };
            pool_.push_task(lambda_task1, left_promise, array, left_bound, right);

            // Ожидаем завершения обеих частей
            left_future.get();
        } else {
            // Если элементов меньше max_elements_, сортируем синхронно
            quicksort(array, left, right_bound);
        }
        if(right - left_bound > 10000) {
            auto right_promise = std::make_shared<std::promise<void>>();
            auto right_future = right_promise->get_future().share();

            auto lambda_task2 = [this, array, left, right_bound, right_promise](int*, long, long) {
               quicksort(array, left, right_bound);
            };
            pool_.push_task(lambda_task2, right_promise, array, left, right_bound);

            // Ожидаем завершения обеих частей
            right_future.get();
        } else {
            // Если элементов меньше max_elements_, сортируем синхронно
            quicksort(array, left_bound, right);
        }
    }
}
