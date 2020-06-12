#include "StdThreadSafeStack.hpp"

#include <iostream>
#include <thread>

using namespace std;
using ThreadSafeStack = childcity::threadsafestack::StdThreadSafeStack<int>;
using EmptyStackException = childcity::threadsafestack::EmptyStackException;

int main() {
    const int threadCount = 8;
    thread t[threadCount];

    ThreadSafeStack safeStack;
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
                    } catch (EmptyStackException &ex) {
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