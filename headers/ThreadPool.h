#pragma once
#include <condition_variable>
#include <future>
#include <queue>
#include <vector>
#include "BlockedQueue.h"
      
typedef std::function<void()> task_type;  // псевдоним task_type функции void  name()          
typedef void (*FuncType)(int, int);  // псевдоним FuncType на указатель для функции void name(int, int)

// пул потоков для задач
class ThreadPool {
  public:
    ThreadPool();
    void start();                                // запуск
    void stop();                                 // остановка
    void push_task(FuncType f, int id, int arg); // проброс задач

    // функция входа для потока
    //void threadFunc();                           
    void threadFunc(int qindex);                 // функция входа для потока

  private:
/*
    std::vector<std::thread> m_threads;         // потоки
    std::queue<task_type> m_task_queue;         // очередь задач
    std::mutex m_locker;                        // поддержка синхронизации очереди    
    std::condition_variable m_event_holder;     // для синхронизации работы потоков
    volatile bool m_work;                       // флаг для остановки работы потоков
*/     
    int m_thread_count;                         // количество потоков       
    std::vector<std::thread> m_threads;         // потоки       
    std::vector<BlockedQueue<task_type>> m_thread_queues;     // Это массив (или контейнер) очередей, в которых хранятся задачи для каждого потока в пуле.  
    int m_index;                                // для равномерного распределения задач

};