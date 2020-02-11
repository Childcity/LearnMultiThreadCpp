//
// Created by childcity on 11.02.20.
//

#ifndef LEARNMULTITHREADC_THREADSAFEHASHMAP_HPP
#define LEARNMULTITHREADC_THREADSAFEHASHMAP_HPP


#include <functional>

template <class Key, class Value, class Hash = std::hash<Key>>
struct ThreadSafeHashMap {
private:
    struct Bucket {
    private:

    };
};

#endif //LEARNMULTITHREADC_THREADSAFEHASHMAP_HPP
