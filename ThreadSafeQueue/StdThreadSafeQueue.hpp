//
// Created by childcity on 05.02.20.
//

#ifndef LEARNMULTITHREADC_STDTHREADSAFEQUEUE_HPP
#define LEARNMULTITHREADC_STDTHREADSAFEQUEUE_HPP


#include <mutex>
#include <memory>
#include <condition_variable>
#include <queue>

using std::move;
using std::shared_ptr;
using std::make_shared;
using std::condition_variable;
using std::lock_guard;
using std::unique_lock;
using std::queue;
using std::mutex;

template<class T>
struct StdThreadSafeQueue {

private:
    mutable mutex m_{};
    queue<shared_ptr<T>> data_{};
    condition_variable dataCond_{};

public:
    StdThreadSafeQueue() = default;

    StdThreadSafeQueue(const StdThreadSafeQueue &other) = delete;
    StdThreadSafeQueue& operator=(const StdThreadSafeQueue &other) = delete;

    ~StdThreadSafeQueue() = default;

    void push(T val)
    {
        shared_ptr<T> data = make_shared<T>(move(val));
        lock_guard<mutex> lk(m_);
        data_.emplace(move(data));
        dataCond_.notify_one();
    }

    void waitAndPop(T &value)
    {
        unique_lock<mutex> lk(m_);
        dataCond_.wait(lk, [this]{ return !data_.empty(); });
        value = move(*data_.front());
        data_.pop();
    }

    shared_ptr<T> waitAndPop()
    {
        unique_lock<mutex> lk(m_);
        dataCond_.wait(lk, [this] { return !data_.empty(); });
        shared_ptr<T> res(data_.front());
        data_.pop();
        return res;
    }

    bool tryPop(T &val)
    {
        lock_guard<mutex> lk(m_);
        if (data_.empty()) {
            return {};
        }
        val = move(*data_.front());
        data_.pop();
        return true;
    }

    shared_ptr<T> tryPop()
    {
        lock_guard<mutex> lk(m_);
        if (data_.empty()) {
            return {};
        }
        shared_ptr<T> res(data_.front());
        data_.pop();
        return res;
    }

    bool empty() const
    {
        lock_guard<mutex> lk(m_);
        return data_.empty();
    }

    size_t size() const
    {
        lock_guard<mutex> lock(m_);
        return data_.size();
    }
};


#endif //LEARNMULTITHREADC_STDTHREADSAFEQUEUE_HPP
