#include "ThreadSafeList.hpp"

#include <iostream>
#include <string>
#include <thread>
#include <atomic>

using namespace std;

using ThreadSafeList = childcity::threadsafelist::ThreadSafeList<string>;

int main()
{
    const int threadCount = 8;
    atomic<bool> startWorking = false;
    thread t[threadCount];

    ThreadSafeList safeList;
    mutex mainMutex;

    srand(static_cast<unsigned int>(time(nullptr)));
    const int min = 1, max = 100;

    for (int i = 0; i < threadCount; ++i) {
        t[i] = thread{[&](int i){

            if(i > 6 /*6*/) { // This is 'Writer Thread'
                while (!startWorking.load(memory_order_acquire)); // wait for all treads started

                while (true) {
                    try {
                        safeList.pushFront(std::to_string(min + (rand() % static_cast<int>(max - min + 1)))); // NOLINT

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
                        // Just for test
                        static const auto predicate = [&](const string &current) { return stoi(current) < ((max - min) / 2.); };

                        safeList.removeIf(predicate);
                        auto val = safeList.findFirstIf(predicate);
                        auto val1 = safeList.at(0);

                        lock_guard<mutex> lock(mainMutex);
                        cout << "thread: " << i << " " << (!val ? "Empty" : *val) << "  " << (!val1 ? "Empty" : *val1) << endl;
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