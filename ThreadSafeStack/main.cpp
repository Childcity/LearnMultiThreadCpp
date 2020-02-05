#include <iostream>
#include <mutex>
#include <stack>
#include <thread>

using namespace std;

struct EmptyStack: exception {
    const char *what() const noexcept override {
        return "Stack is empty.";
    }
};

template <class T>
struct ThreadSafeStack {

private:
    stack<T> data{};
    mutable mutex m{};

public:
    ThreadSafeStack() = default;
    ~ThreadSafeStack() = default;

    ThreadSafeStack(const ThreadSafeStack &other)
    {
        lock_guard<mutex> lock(other.m);
        data = other.data; // копирование происходит в теле конструктора
    }

    ThreadSafeStack(const ThreadSafeStack &&) = delete;
    ThreadSafeStack& operator=(const ThreadSafeStack &) = delete;
    ThreadSafeStack& operator=(const ThreadSafeStack &&) = delete;

    void push(T newVal)
    {
        lock_guard<mutex> lock(m);
        data.push(newVal);
    }

    shared_ptr<T> pop()
    {
        lock_guard<mutex> lock(m);

        //Перед тем как выталкивать значение, проверяем, не пуст ли стек
        if(data.empty())
            throw EmptyStack();

        const shared_ptr<T> res(make_shared<T>(data.top()));
        data.pop(); // Перед тем как модифицировать стек в функции pop, выделяем память для возвращаемого значения

        return res;
    }

    void pop(T& value)
    {
        lock_guard<mutex> lock(m);

        //Перед тем как выталкивать значение, проверяем, не пуст ли стек
        if(data.empty())
            throw EmptyStack();

        value = data.top();
        data.pop();
    }

    bool isEmpty() const
    {
        lock_guard<mutex> lock(m);
        return data.empty();
    }

    size_t size() const
    {
        lock_guard<mutex> lock(m);
        return data.size();
    }
};


int main() {
    const int threadCount = 8;
    thread t[threadCount];

    ThreadSafeStack<int> safeStack;
    mutex mainMutex;

    srand(static_cast<unsigned int>(time(nullptr)));


    for (int i = 0; i < threadCount; ++i) {
        t[i] = thread{[&safeStack, &mainMutex](int i){

            if(i > 3){
                while (true) {
                    try {
                        safeStack.push(rand()); // NOLINT

                        {// sleep 20 microseconds
                            using namespace std::chrono;
                            auto curr = high_resolution_clock::now();
                            while (duration_cast<microseconds>(high_resolution_clock::now() - curr).count() < 20);
                        }
                    } catch (exception &ex){
                        break;
                    }
                }
            } else {
                while (true) {
                    try {
                        lock_guard<mutex> lock(mainMutex);

                        auto val = safeStack.pop();
                        auto size = safeStack.size();
                        cout << "pop: " << *val.get() << " size: " << size << endl;
                    } catch (EmptyStack &ex) {
                        cout << "Empty" << endl;
                    } catch (EmptyStack &ex) {
                        cout << "Empty" << endl;
                    } catch (exception &ex){
                        break;
                    }
                }
            }

        }, i};
    }

    for (auto &i : t) {
        i.join();
    }

    return 0;
}