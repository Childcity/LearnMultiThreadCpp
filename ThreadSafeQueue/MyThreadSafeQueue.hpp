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
//using std::condition_variable;
//using std::lock_guard;
using std::mutex;
using std::move;

template <class T>
struct MyThreadSafeQueue {

private:
    struct Node {
        T Data{};
        unique_ptr<Node> Next{};

        explicit Node(T data)
                : Data(move(data))
        {}
    };

private:
    unique_ptr<Node> head_{};
    Node *tail_ = nullptr;

public:
    MyThreadSafeQueue() = default;

    MyThreadSafeQueue(const MyThreadSafeQueue &other) = delete;
    MyThreadSafeQueue& operator=(const MyThreadSafeQueue &other) = delete;

    shared_ptr<T> tryPop()
    {
        if(! head_){
            return {};
        }

        const shared_ptr<T> res = make_shared<T>(move(head_->Data));

        const unique_ptr<Node> oldHead = move(head_);
        head_ = move(oldHead->Next);

        return res;
    }

    void push(T newVal)
    {
        unique_ptr<Node> p = make_unique<Node>(move(newVal));
        Node *const newTail = p.get();

        if (tail_) {
            tail_->Next = move(p);
        } else {
            head_ = move(p);
        }

        tail_ = newTail;
    }
};


#endif //LEARNMULTITHREADC_MYTHREADSAFEQUEUE_HPP
