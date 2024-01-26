#pragma once
#include <iostream>
#include <queue>
#include <mutex>
#include <condition_variable>

template <class T> class BlockedQueue {
  public:
    void push(T& item); // обычный потокобезопасный push
    void pop(T& item); // блокирующий метод получения элемента из очереди
    bool fast_pop(T& item); // неблокирующий метод получения элемента из очереди
                            // возвращает false, если очередь пуста
  private:
    std::mutex m_locker;
    std::queue<T> m_task_queue;         // очередь задач
    std::condition_variable m_notifier; // уведомитель
};