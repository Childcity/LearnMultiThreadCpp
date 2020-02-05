#include "StdThreadSafeQueue.hpp"
#include "MyThreadSafeQueue.hpp"

#include <iostream>
#include <thread>
#include <atomic>


int main()
{
    const int threadCount = 8;
    atomic<bool> startWorking = false;
    thread t[threadCount];

    StdThreadSafeQueue<string> safeQueue;
    mutex mainMutex;

    srand(static_cast<unsigned int>(time(nullptr)));


    for (int i = 0; i < threadCount; ++i) {
        t[i] = thread{[&](int i){

            if(i > 3) {
                while (startWorking.load(memory_order_acquire)) {
                    try {
                        safeQueue.push(to_string(rand())); // NOLINT

                        {// sleep 10 microseconds
                            using namespace std::chrono;
                            auto curr = high_resolution_clock::now();
                            while (duration_cast<microseconds>(high_resolution_clock::now() - curr).count() < 10);
                        }
                    } catch (exception &ex){
                        break;
                    }
                }
            } else {
                while (true) {
                    try {
                        lock_guard<mutex> lock(mainMutex);

                        auto val = safeQueue.tryPop();
                        auto size = safeQueue.size();
                        cout << "pop: " << (val == nullptr ? "Empty" : *val) << " size: " << size << endl;
                    } catch (exception &ex){
                        break;
                    }
                }
            }

        }, i};
    }

    // enable all threads
    startWorking.store(memory_order_release);

    for (auto &i : t) {
        i.join();
    }

    return 0;
}