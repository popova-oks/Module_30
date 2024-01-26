#include <chrono>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>

#include "../headers/ThreadPool.h"

ThreadPool::ThreadPool()
    : m_thread_count(std::thread::hardware_concurrency() != 0 ? std::thread::hardware_concurrency()
                                                              : 4), // количество ядер процессора
      m_thread_queues(m_thread_count) {}


/*
void ThreadPool::start() {
    m_work = true;               // устанавливаем флаг в true
    for(int i = 0; i < 4; i++) { // создаем три потока
        m_threads.push_back(
            std::thread(&ThreadPool::threadFunc,
                        this)); // передаем указатель на метод и this в качестве аргумента
        // помещаем каждый поток в вектор
    }
}
*/

void ThreadPool::start() {
    for(int i = 0; i < m_thread_count; i++) {
        m_threads.emplace_back(&ThreadPool::threadFunc, this, i);
    }
}

/*
void ThreadPool::stop() {
    m_work = false;              // устанавливаем флаг в false
    m_event_holder.notify_all(); // передаем событие чтобы проснулись заблокированные потоки
    for(auto& t : m_threads) {   // проходим по вектору потоков
        t.join(); // и дожидаемся завершения каждого потока
    }
}
*/

void ThreadPool::stop() {
    for(int i = 0; i < m_thread_count; i++) {
        // кладем задачу-пустышку в каждую очередь
        // для завершения потока
        task_type empty_task;
        m_thread_queues[i].push(empty_task);
    }
    for(auto& t : m_threads) {
        t.join();
    }
}

/*
// Метод проброса задач:
void ThreadPool::push_task(FuncType f, int id, int arg) {
    std::lock_guard<std::mutex> l(m_locker); // блокируем доступ к очереди задач
    task_type new_task(
        [=] { f(id, arg); }); // функция для задачи типа task_type = функция void ()
                              // с передачей указателя FuncType f на функцию void (int, int)
    m_task_queue.push(new_task); // добавляем задачу в очередь задач
    m_event_holder.notify_one(); // оповещаем случайный поток о новой задаче
}
*/

void ThreadPool::push_task(FuncType f, int id, int arg) {
    // вычисляем индекс очереди, куда положим задачу
    int queue_to_push = m_index++ % m_thread_count;
    // формируем функтор
    task_type task ([=] { f(id, arg); });
    // кладем в очередь
    m_thread_queues[queue_to_push].push(task);
}

/*
void ThreadPool::threadFunc() {
    while(true) {
        task_type task_to_do; // создаем новую переменную для задачи
        {
            std::unique_lock<std::mutex> l(m_locker); // блокируем доступ к очереди задач
            if(m_task_queue.empty() && !m_work) // если очередь пуста и флаг false — выходим
                return;
            if(m_task_queue.empty()) { // очередь пуста — ждем события
                m_event_holder.wait(l, [&]() {
                    return !m_task_queue.empty() || !m_work;
                }); // если очередь не пуста или флаг false
                    // то выходим из цикла и проснулись
            }
            if(!m_task_queue.empty()) { // если очередь не пуста — берем задачу из очереди
                task_to_do = m_task_queue.front(); // берем первый элемент из очереди задачи
                                                   // и передаем ее в переменную задачи
                m_task_queue.pop(); // удаляем задачу из очереди задач
            }
        }
        // выполняем задачу, вызвав функтор
        task_to_do();
    }
}
*/

void ThreadPool::threadFunc(int qindex) {
    while(true) {
        // обработка очередной задачи
        task_type task_to_do; // Создается переменная для хранения следующей задачи.
        bool res; // Инициализируется переменная res для отслеживания успешности операции получения
                  // задачи из очереди.
        int i = 0;
        // Используется цикл for для попытки быстрого извлечения задачи из любой из очередей.
        // Цикл проходится по всем очередям, начиная с той, которая соответствует текущему индексу
        // потока (qindex). Если задача быстро извлекается (res == true), цикл прерывается.
        for(; i < m_thread_count; i++) {
            // Это арифметическое выражение для выбора следующей очереди задач.
            // qindex - индекс текущего потока, i - переменная цикла, m_thread_count - общее
            // количество потоков .fast_pop(task_to_do): Это вызов функции fast_pop у объекта
            // очереди.
            if(res = m_thread_queues[(qindex + i) % m_thread_count].fast_pop(task_to_do))
                break; // если задача быстро извлечена
        }

        // Если задача не была извлечена быстро, выполняется блокирующая операция получения задачи
        // из собственной очереди потока
        if(!res) {
            // вызываем блокирующее получение очереди
            m_thread_queues[qindex].pop(task_to_do);
        } else if(!task_to_do) {
            // чтобы не допустить зависания потока
            // кладем обратно задачу-пустышку
            m_thread_queues[(qindex + i) % m_thread_count].push(task_to_do);
        }
        // Если задача не была получена (в очереди больше нет задач), 
        // поток выходит из цикла и завершает свою работу.
        if(!task_to_do) {
            return;
        }
        // иначе выполняем задачу
        task_to_do();
    }
}
