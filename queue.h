#ifndef QUEUE_H
#define QUEUE_H

#include <thread> 
#include <queue> 
#include <mutex> 
#include <condition_variable> 
#include <stdexcept>


template <class T>
class Queue{
public:
    Queue() : m_isProgramStopped(false) { }

    void push(T i){
        std::unique_lock<std::mutex> lock(m_mtx);
        m_q.push(i);
        m_cond.notify_one();
    }

    T pop(){
        std::unique_lock<std::mutex> lock(m_mtx);
        std::cout << "<queue> waiting in pop()... \n";
        m_cond.wait(lock, [&](){ return !m_q.empty() || m_isProgramStopped; });

        if (m_isProgramStopped){
            throw std::runtime_error("Program stopped in queue!");
        }

        T x = m_q.front();
        m_q.pop();

        //std::cout << "Thread " << std::this_thread::get_id() << " popped " << x << "." << std::endl;
        return x;
    }

    void stop(){
        m_isProgramStopped = true;
        m_cond.notify_all();
    }
    
    bool cleanup(T& x){

      if (!m_q.empty()) {
        std::unique_lock<std::mutex> lock(m_mtx);
        x = m_q.front();
        m_q.pop();
        return true;      
      }
      else {
        return false;
      }
        
    }

private:
    std::queue<T> m_q;
    std::mutex m_mtx;
    std::condition_variable m_cond;
    bool m_isProgramStopped;
};

#endif