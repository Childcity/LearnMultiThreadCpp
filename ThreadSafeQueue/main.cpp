#include <iostream>
#include <condition_variable>
#include <queue>

using namespace std;

template<class T>
class ThreadSafeQueue {

public:
    ThreadSafeQueue() = default;

    ~ThreadSafeQueue() = default;

    void push(T val)
    {
        lock_guard<mutex> lk(m_);
        dataQueue_.push(move(val));
        dataQueueCond_.notify_one();
    }

    void waitAndPop(T &value)
    {
        unique_lock<mutex> lk(m_);
        dataQueueCond_.wait(lk, [this]{ return !dataQueue_.empty(); });
        value = move(dataQueue_.front());
        dataQueue_.pop();
    }

    shared_ptr<T> waitAndPop()
    {
        unique_lock<mutex> lk(m_);
        dataQueueCond_.wait(lk, [this] { return !dataQueue_.empty(); });
        shared_ptr<T> res(make_shared<T>(move(dataQueue_.front())));
        dataQueue_.pop();
        return res;
    }

    bool tryPop(T &val)
    {
        lock_guard<mutex> lk(m_);
        if (dataQueue_.empty()) {
            return {};
        }
        val = move(dataQueue_.front());
        dataQueue_.pop();
        return true;
    }

    shared_ptr<T> tryPop()
    {
        lock_guard<mutex> lk(m_);
        if (dataQueue_.empty()) {
            return {};
        }
        shared_ptr<T> res(make_shared<T>(move(dataQueue_.front())));
        dataQueue_.pop();
        return res;
    }

    bool empty() const
    {
        lock_guard<mutex> lk(m_);
        return dataQueue_.empty();
    }

private:
    mutable mutex m_;
    queue<T> dataQueue_;
    condition_variable dataQueueCond_;
};


int main()
{
    ThreadSafeQueue<string> queue;
    queue.push("Marine cost");

    string s;
    queue.tryPop(s);
    cout << s << endl;

    //cout << *queue.tryPop() << endl;

    return 0;
}