//
// Created by childcity on 05.02.20.
//

#ifndef LEARNMULTITHREADC_STDTHREADSAFEQUEUE_HPP
#define LEARNMULTITHREADC_STDTHREADSAFEQUEUE_HPP


#include <mutex>
#include <memory>
#include <condition_variable>
#include <queue>


namespace childcity {
    namespace threadsafeqeue {

template<class T>
struct StdThreadSafeQueue {

private:
    mutable std::mutex m_{};
    std::queue<std::shared_ptr<T>> data_{};
    std::condition_variable dataCond_{};

public:
    StdThreadSafeQueue() = default;

    StdThreadSafeQueue(const StdThreadSafeQueue &other) = delete;
    StdThreadSafeQueue& operator=(const StdThreadSafeQueue &other) = delete;

    ~StdThreadSafeQueue() = default;

    void push(T val)
    {
        std::shared_ptr<T> data = std::make_shared<T>(std::move(val));
        std::lock_guard<std::mutex> lk(m_);
        data_.emplace(std::move(data));
        dataCond_.notify_one();
    }

    void waitAndPop(T &value)
    {
        std::unique_lock<std::mutex> lk(m_);
        dataCond_.wait(lk, [this]{ return !data_.empty(); });
        value = std::move(*data_.front());
        data_.pop();
    }

    std::shared_ptr<T> waitAndPop()
    {
        std::unique_lock<std::mutex> lk(m_);
        dataCond_.wait(lk, [this] { return !data_.empty(); });
        std::shared_ptr<T> res(data_.front());
        data_.pop();
        return res;
    }

    bool tryPop(T &val)
    {
        std::lock_guard<std::mutex> lk(m_);
        if (data_.empty()) {
            return {};
        }
        val = std::move(*data_.front());
        data_.pop();
        return true;
    }

    std::shared_ptr<T> tryPop()
    {
        std::lock_guard<std::mutex> lk(m_);
        if (data_.empty()) {
            return {};
        }
        std::shared_ptr<T> res(data_.front());
        data_.pop();
        return res;
    }

    bool empty() const
    {
        std::lock_guard<std::mutex> lk(m_);
        return data_.empty();
    }

    size_t size() const
    {
        std::lock_guard<std::mutex> lock(m_);
        return data_.size();
    }
};


    } // namespace threadsafeqeue
} // namespace childcity


#endif //LEARNMULTITHREADC_STDTHREADSAFEQUEUE_HPP
