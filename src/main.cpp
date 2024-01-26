#include <iostream>
#include <thread>
#include <chrono>

#include "../headers/ThreadPool.h"
#include "../headers/RequestHandler.h"
#include "../headers/BlockedQueue.h"

std::mutex coutLocker; // мьютекс для консоли
using namespace std;


// функция, выполняющая задачу
void taskFunc(int id, int delay){
   // имитируем время выполнения задачи
   std::this_thread::sleep_for(std::chrono::seconds(delay));
   // выводим информацию о завершении
   std::unique_lock<std::mutex> l(coutLocker);
   std::cout << "task " << id << " made by thread_id " << std::this_thread::get_id() << std::endl;
}

// функция, выполняющая задачу
void taskFunc2(int id, int delay){
   // имитируем время выполнения задачи
   std::this_thread::sleep_for(std::chrono::microseconds(delay));
   // выводим информацию о завершении
   std::unique_lock<std::mutex> l(coutLocker);
   std::cout << "task " << id << " made by thread_id " << std::this_thread::get_id() << std::endl;
}

int main()
{
    srand(0);
    RequestHandler rh;
    for(int i=0; i<20; i++) {
       rh.pushRequest(taskFunc2, i, 1 + rand()%4);
    }

    return 0;
}