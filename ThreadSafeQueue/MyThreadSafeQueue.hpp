//
// Created by childcity on 05.02.20.
//

#ifndef LEARNMULTITHREADC_MYTHREADSAFEQUEUE_HPP
#define LEARNMULTITHREADC_MYTHREADSAFEQUEUE_HPP


template <class T>
struct MyThreadSafeQueue {

private:
    struct Node {
        T Data{};
        std::unique_ptr<Node> Next{};

        explicit Node(T data)
                : Data(std::move(data))
        {}
    };

private:
    std::unique_ptr<Node> head_{};
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

        const shared_ptr<T> res(make_shared<T>(move(head_->Data)));

        const unique_ptr<Node> oldHead = move(head_);
        head_ = move(oldHead->Next);

        return res;
    }

    void push(T newVal)
    {
        unique_ptr<Node> p(make_unique<T>(move(newVal)));
    }
};


#endif //LEARNMULTITHREADC_MYTHREADSAFEQUEUE_HPP
