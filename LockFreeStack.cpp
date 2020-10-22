//
// Created by childcity on 12.06.20.
//

#include <memory>
#include <atomic>

template<typename T>
class LockFreeStack {
private:
    struct Node {
        std::shared_ptr<T> Data;
        Node *Next;

        explicit Node(const T &datas)
            : Data(std::make_shared<T>(datas))
        {}
    };

private:
    std::atomic<unsigned> threadsInPop_;
    std::atomic<Node *> toBeDeleted_;
    std::atomic<Node *> head_;


public:
    void push(const T &Data)
    {
        auto *new_node = new Node(Data);
        new_node->Next = head_.load();
        while(! head_.compare_exchange_weak(new_node->Next, new_node));
    }

    std::shared_ptr<T> pop()
    {
        ++threadsInPop_; // Увеличиваем счетчик потоков в pop()

        Node *oldHead = nullptr; oldHead = head_.load();
        while ((oldHead != nullptr) && (! head_.compare_exchange_weak(oldHead, oldHead->Next)));

        std::shared_ptr<T> res;
        if (oldHead) {
           // res.swap(oldHead->Data); // Извлекаем данные узла БЕЗ КОПИРОВАНИЯ
        }

        //try_reclaim(oldHead); // Попытаемся удалить из памяти узлы, ожидающие удаления (в том числе и oldHead)
        return res;
    }

private:
    static void deleteNodes(Node *nodes)
    {
        while (nodes) {
            Node *Next = nodes->Next;
            delete nodes;
            nodes = Next;
        }
    }

    void try_reclaim(Node *oldHead)
    {
        if (threadsInPop_ == 1) { //1. Если я единственный поток в pop()

            Node *nodes_to_delete = toBeDeleted_.exchange(nullptr); //2. Заявляем права на список toBeDeleted_

            if((--threadsInPop_) == 0) { //3. Я все еще единственный поток в pop() ?
                deleteNodes(nodes_to_delete); //4. Удаляем узлы, которые можно удалить безопасно

            } else if (nodes_to_delete != nullptr) {  //5
                chainPendingNodes(nodes_to_delete); //6
            }

            delete oldHead; //7

        } else {
            chainPendingNode(oldHead); //8
            --threadsInPop_;
        }
    }

    void chainPendingNodes(Node *nodes)
    {
        Node *last = nodes;
        while (Node * Next = last->Next) {  //9 дойдем до конца списка
            last = Next;
        }
        chainPendingNodes(nodes, last);
    }

    void chainPendingNodes(Node *first, Node *last)
    {
        last->Next = toBeDeleted_; //10
        while (! toBeDeleted_.compare_exchange_weak(last->Next, first)); //11 чтобы last->Next было корректно
    }

    void chainPendingNode(Node *pNode)
    {
        chainPendingNodes(pNode, pNode); //12
    }

};

#include <iostream>
#include <thread>
#include <mutex>
using namespace std;
using LockFreeStackk = LockFreeStack<int>;
int main()
{

    const int threadCount = 8;
    thread t[threadCount];

    LockFreeStackk safeStack;
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
                        auto val = safeStack.pop();
                        lock_guard<mutex> lock(mainMutex);
                        //cout << "pop: " << val << endl;
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