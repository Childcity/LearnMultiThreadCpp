//
// Created by childcity on 05.02.20.
//

#ifndef LEARNMULTITHREADC_MYTHREADSAFEQUEUE_HPP
#define LEARNMULTITHREADC_MYTHREADSAFEQUEUE_HPP


#include <memory>

using std::shared_ptr;
using std::make_shared;
using std::unique_ptr;
using std::make_unique;
using std::condition_variable;
using std::lock_guard;
using std::mutex;
using std::move;

template <class T>
struct MyThreadSafeQueue {

private:

    // Node of queue list
    struct Node {
        shared_ptr<T> Data{};
        unique_ptr<Node> Next{};
    };

private:
    unique_ptr<Node> head_{};
    mutable mutex headMutex_{};

    Node *tail_{};
    mutable mutex tailMutex_{};

    condition_variable dataCond_{};

public:
    MyThreadSafeQueue()
            : head_(make_unique<Node>())
            , tail_(head_.get())
    {}

    MyThreadSafeQueue(const MyThreadSafeQueue &other) = delete;
    MyThreadSafeQueue& operator=(const MyThreadSafeQueue &other) = delete;

    ~MyThreadSafeQueue() = default;

    void push(T newVal)
    {
        // Next two operations require memory allocation.
        // So we do this without blocking any mutex
        shared_ptr<T> newData = make_shared<T>(move(newVal));
        unique_ptr<Node> p = make_unique<Node>();

        // Next block is running under mutex 'tailMutex_'
        {
            lock_guard<mutex> lk(tailMutex_);

            tail_->Data = newData;

            Node *const newTail = p.get();
            tail_->Next = move(p);
            tail_ = newTail;
        }

        dataCond_.notify_one();
    }

    void waitAndPop(T &value)
    {
        const unique_ptr<Node> oldData = waitAndPopHead(value);
    }

    shared_ptr<T> waitAndPop()
    {
        const unique_ptr<Node> oldData = waitAndPopHead();
        return oldData->Data;
    }

    bool tryPop(T &value)
    {
        const unique_ptr<Node> oldHead = tryPopHead(value);
        return oldHead ? true : false;
    }

    shared_ptr<T> tryPop()
    {
        unique_ptr<Node> oldHead = tryPopHead();
        return oldHead ? oldHead->Data : shared_ptr<T>();
    }

    bool empty() const
    {
        lock_guard<mutex> lk(headMutex_);
        return head_.get() == getTail();
    }

private:
    Node* getTail() const
    {
        lock_guard<mutex> tailLock(tailMutex_);
        return tail_;
    }

    unique_ptr<Node> popHead()
    {
        unique_ptr<Node> oldHead = move(head_);
        head_ = move(oldHead->Next);
        return oldHead;
    }

    unique_lock<mutex> waitForData()
    {
        unique_lock<mutex> headLock(headMutex_);
        dataCond_.wait(headLock, [&] { return head_.get() != getTail(); });
        return move(headLock);
    }

    unique_ptr<Node> waitAndPopHead()
    {
        unique_lock<mutex> headLock = waitForData();
        return popHead();
    }

    unique_ptr<Node> waitAndPopHead(T &value)
    {
        unique_lock<mutex> headLock = waitForData();
        value = move(*head_->Data);
        return popHead();
    }

    unique_ptr<Node> tryPopHead()
    {
        lock_guard<mutex> headLock(headMutex_);

        if(head_.get() == getTail()){
            return {};
        }

        return popHead();
    }

    unique_ptr<Node> tryPopHead(T &value)
    {
        lock_guard<mutex> headLock(headMutex_);

        if(head_.get() == getTail()){
            return {};
        }

        value = move(*head_->Data);
        return popHead();
    }
};


#endif //LEARNMULTITHREADC_MYTHREADSAFEQUEUE_HPP
