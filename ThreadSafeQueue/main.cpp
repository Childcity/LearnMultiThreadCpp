#include "StdThreadSafeQueue.hpp"
#include "MyThreadSafeQueue.hpp"

#include <iostream>
#include <string>
#include <thread>
#include <atomic>

using namespace std;

int main()
{
    const int threadCount = 8;
    atomic<bool> startWorking = false;
    thread t[threadCount];

    //StdThreadSafeQueue<string> stdSafeQueue;
    MyThreadSafeQueue<string> mySafeQueue;
    mutex mainMutex;

    srand(static_cast<unsigned int>(time(nullptr)));


//    {
//        MyThreadSafeQueue<int> mySafeQueue;
//        mySafeQueue.push(12);
//        cout << *mySafeQueue.tryPop() << endl;
//        return 0;
//    }


    for (int i = 0; i < threadCount; ++i) {
        t[i] = thread{[&](int i){

            if(i > 3) {
                while (!startWorking.load(memory_order_acquire)); // wait for all treads started

                while (true) {
                    try {
                        //stdSafeQueue.push(to_string(rand())); // NOLINT
                        mySafeQueue.push(to_string(rand())); // NOLINT

                        {// sleep 10 microseconds
                            using namespace std::chrono;
                            auto curr = high_resolution_clock::now();
                            while (duration_cast<microseconds>(high_resolution_clock::now() - curr).count() < 10);
                        }
                    } catch (exception &ex) {
                        break;
                    }
                }
            } else {
                while (!startWorking.load(memory_order_acquire)); // wait for all treads started

                while (true) {
                    try {
                        lock_guard<mutex> lock(mainMutex);

//                        auto val = stdSafeQueue.tryPop();
//                        auto size = stdSafeQueue.size();
                        auto val = mySafeQueue.tryPop();
                        auto size = 0;
                        cout << "pop: " << (val == nullptr ? "Empty" : *val) << " size: " << size << endl;
                    } catch (exception &ex) {
                        break;
                    }
                }
            }

        }, i};
    }

    // enable all threads
    startWorking.store(true, memory_order_release);

    for (auto &i : t) {
        i.join();
    }

    return 0;
}