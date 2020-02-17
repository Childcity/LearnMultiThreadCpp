//
// Created by childcity on 11.02.20.
//

#ifndef LEARNMULTITHREADC_THREADSAFEHASHMAP_HPP
#define LEARNMULTITHREADC_THREADSAFEHASHMAP_HPP


#include <functional>
#include <memory>
#include <shared_mutex>
#include <list>

using std::shared_ptr;
using std::make_shared;
using std::unique_ptr;
using std::make_unique;
using std::lock_guard;
using std::shared_mutex;
using std::shared_lock;
using std::unique_lock;
using std::pair;
using std::move;
using std::list;

template <class Key, class Value, class Hash = std::hash<Key>>
struct ThreadSafeHashMap {
private:
    struct Bucket {
    private:
        using BucketValue = pair<Key, Value>;
        using BucketData = list<BucketValue>;
        using BucketIterator = BucketData::iterator;

        BucketData data_;
        mutable shared_mutex bucketMutex_;

    public:
        Value ValueFor(const Key &key, const Value &defaultVal) const
        {
            shared_lock<shared_mutex> lock(bucketMutex_);
            const BucketIterator foundedEntry = findEntryFor(key);
            return (foundedEntry == data_.end()) ? defaultVal : foundedEntry->second;
        }

        void addOrUpdateMapping(const Key &key, const Value &value)
        {
            unique_lock<shared_mutex> lock(bucketMutex_);
            const BucketIterator foundedEntry = findEntryFor(key);
            if(foundedEntry == data_.end()){
                data_.emplace_back(BucketValue(key, value));
            } else {
                foundedEntry->second = value;
            }
        }

        void removeMapping(const Key &key)
        {
            unique_lock<shared_mutex> lock(bucketMutex_);
            const BucketIterator foundedEntry = findEntryFor(key);
            if(foundedEntry != data_.end()) {
                data_.erase(key);
            }
        }

    private:
        BucketIterator findEntryFor(const Key &key) const {
            return std::find_if(data_.begin(), data_.end(), [&](const BucketValue &val){ return val.first == key; });
        }
    };

private:

};

#endif //LEARNMULTITHREADC_THREADSAFEHASHMAP_HPP
