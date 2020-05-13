//
// Created by childcity on 05.05.20.
//

#ifndef LEARNMULTITHREADC_THREADSAFESTACK_HPP
#define LEARNMULTITHREADC_THREADSAFESTACK_HPP

#include <mutex>
#include <stack>
#include <exception>


namespace childcity {
    namespace threadsafestack {


struct EmptyStackException: public std::runtime_error {
    EmptyStackException()
        : std::runtime_error("Stack is empty."){}
};

template <class T>
struct StdThreadSafeStack {
private:
    std::stack<T> data_;
    mutable std::mutex mutex_;

public:
    StdThreadSafeStack() = default;
    ~StdThreadSafeStack() = default;

    StdThreadSafeStack(const StdThreadSafeStack &other)
    {
        lock_guard<std::mutex> lock(other.mutex_);
        data_ = other.data_; // копирование происходит в теле конструктора
    }

    StdThreadSafeStack(const StdThreadSafeStack &&) = delete;
    StdThreadSafeStack& operator=(const StdThreadSafeStack &) = delete;
    StdThreadSafeStack& operator=(const StdThreadSafeStack &&) = delete;

    void push(T newVal)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        data_.push(newVal);
    }

    std::shared_ptr<T> pop()
    {
        std::lock_guard<std::mutex> lock(mutex_);

        //Перед тем как выталкивать значение, проверяем, не пуст ли стек
        if(data_.empty())
            throw EmptyStackException();

        const std::shared_ptr<T> res(make_shared<T>(data_.top()));
        data_.pop(); // Перед тем как модифицировать стек в функции pop, выделяем память для возвращаемого значения

        return res;
    }

    void pop(T& value)
    {
        std::lock_guard<std::mutex> lock(mutex_);

        //Перед тем как выталкивать значение, проверяем, не пуст ли стек
        if(data_.empty())
            throw EmptyStackException();

        value = data_.top();
        data_.pop();
    }

    bool isEmpty() const
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return data_.empty();
    }

    size_t size() const
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return data_.size();
    }
};


    } // namespace threadsafestack
} // namespace childcity


#endif //LEARNMULTITHREADC_THREADSAFESTACK_HPP