#include "ThreadSafeHashMap.hpp"

#include <iostream>
#include <string>
#include <thread>
#include <atomic>
#include <mutex>

using namespace std;
using ThreadSafeHashMap = childcity::threadsafehash::ThreadSafeHashMap<int, string, std::hash<int>>;

int main()
{
    const int threadCount = 8;
    atomic<bool> startWorking = false;
    atomic<int> keyNum = 0;
    thread t[threadCount];

    ThreadSafeHashMap safeHashMap;
    mutex mainMutex;

    srand(static_cast<unsigned int>(time(nullptr)));

    for (int i = 0; i < threadCount; ++i) {
        t[i] = thread{[&](int i){

            if(i > 3 /*6*/) { // This is 'Writer Thread'
                while (!startWorking.load(memory_order_acquire)); // wait for all treads started

                while (true) {
                    try {
                        int newKey = keyNum.fetch_add(1);
                        //cout << "key: " << newKey << endl;
                        safeHashMap.addOrUpdate(newKey, to_string(rand())); // NOLINT

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
                        const auto key = keyNum.fetch_sub(1) - 1;
                        const auto val = safeHashMap.valueFor(key, "Empty");

                        lock_guard<mutex> lock(mainMutex);
                        cout << "key: " << key << " value: " << val << endl;
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