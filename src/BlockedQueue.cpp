#include "../headers/BlockedQueue.h"
#include <functional>
#include <mutex>


// обычный потокобезопасный push
template <class T> void BlockedQueue<T>::push(T& item) {
    std::lock_guard<std::mutex> l(m_locker); // захватываем мьютекс
    m_task_queue.push(item);       // добавляем элемент в очередь
    // делаем оповещение, чтобы поток, вызвавший
    // pop проснулся и забрал элемент из очереди
    m_notifier.notify_one();
}

// блокирующий метод получения элемента из очереди
template <class T> void BlockedQueue<T>::pop(T& item) {
    std::unique_lock<std::mutex> l(m_locker); // захватываем мьютекс
    if(m_task_queue.empty())        // если очередь пуста
        m_notifier.wait(
            l, [this] { return !m_task_queue.empty(); }); // ждем, когда появится элемент в очереди
    item = m_task_queue.front(); // забираем первый элемент из очереди
    m_task_queue.pop();          // удаляем его из очереди
}

// неблокирующий метод получения элемента из очереди
// возвращает false, если очередь пуста
template <class T> bool BlockedQueue<T>::fast_pop(T& item) {
    std::lock_guard<std::mutex> l(m_locker); // захватываем мьютекс
    if(m_task_queue.empty())       // если очередь пуста
        return false;              // выходим    
    item = m_task_queue.front();   // иначе забираем элемент
    m_task_queue.pop();            // удаляем его из очереди
    return true;                   // возвращаем true
}

template class BlockedQueue<std::function<void()>>;