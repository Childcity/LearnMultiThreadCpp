#include "ThreadSafeHashMap.hpp"

#include <iostream>
#include <string>
#include <thread>
#include <atomic>
#include <mutex>

using namespace std;

int main()
{
    const int threadCount = 8;
    atomic<bool> startWorking = false;
    atomic<int> keyNum = 0;
    thread t[threadCount];

    ThreadSafeHashMap<int, string, std::hash<int>> safeHashMap;
    mutex mainMutex;

    srand(static_cast<unsigned int>(time(nullptr)));

    for (int i = 0; i < threadCount; ++i) {
        t[i] = thread{[&](int i){

            if(i > 3 /*6*/) { // This is 'Writer Thread'
                while (!startWorking.load(memory_order_acquire)); // wait for all treads started

                while (true) {
                    try {
                        safeHashMap.addOrUpdate(21, to_string(rand())); // NOLINT

                        {// sleep 10 microseconds
                            using namespace std::chrono;
                            auto curr = high_resolution_clock::now();
                            while (duration_cast<microseconds>(high_resolution_clock::now() - curr).count() < 10 /*1800*/);
                        }
                    } catch (exception &ex) {
                        break;
                    }
                }
            } else { // This is 'Reader Thread'
                while (!startWorking.load(memory_order_acquire)); // wait for all treads started

                while (true) {
                    try {
                        lock_guard<mutex> lock(mainMutex);


                        //const auto key = keyNum.fetch_sub(1, std::memory_order_relaxed);
                        //const auto val = safeHashMap.valueFor(key, "Empty");
                        //cout << "value: " << val << endl;
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