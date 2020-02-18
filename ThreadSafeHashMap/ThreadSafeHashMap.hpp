//
// Created by childcity on 11.02.20.
//

#ifndef LEARNMULTITHREADC_THREADSAFEHASHMAP_HPP
#define LEARNMULTITHREADC_THREADSAFEHASHMAP_HPP


#include <functional>
#include <list>
#include <vector>
#include <shared_mutex>
#include <memory>

using std::unique_ptr;
using std::shared_mutex;
using std::shared_lock;
using std::unique_lock;

template <class Key, class Value, class Hash = std::hash<Key>>
struct ThreadSafeHashMap {
private:
    struct Bucket {
    private:
        using BucketValue = std::pair<Key, Value>;
        using BucketData = std::list<BucketValue>;

        BucketData data_;
        mutable shared_mutex bucketMutex_;

    public:
        Value valueFor(const Key &key, const Value &defaultVal)
        {
            shared_lock<shared_mutex> lock(bucketMutex_);
            const auto foundedEntry = findEntryFor(key);
            return (foundedEntry == data_.end()) ? defaultVal : foundedEntry->second;
        }

        void addOrUpdate(const Key &key, const Value &value)
        {
            unique_lock<shared_mutex> lock(bucketMutex_);
            auto foundedEntry = findEntryFor(key);
            if(foundedEntry == data_.end()){
                data_.emplace_back(BucketValue(key, value));
            } else {
                foundedEntry->second = value;
            }
        }

        void erase(const Key &key)
        {
            unique_lock<shared_mutex> lock(bucketMutex_);
            const auto foundedEntry = findEntryFor(key);
            if(foundedEntry != data_.end()) {
                data_.erase(foundedEntry);
            }
        }

    private:
        auto findEntryFor(const Key &key) {
            return std::find_if(data_.begin(), data_.end(), [&](const BucketValue &val){ return val.first == key; });
        }
    };

private:
    std::vector<unique_ptr<Bucket>> buckets_;
    Hash hasher_;

public:
    explicit ThreadSafeHashMap(size_t bucketsNum = 19, const Hash &hasher = Hash())
        : buckets_(bucketsNum)
        , hasher_(hasher)
    {
        for (auto &bucket : buckets_) {
            bucket = std::make_unique<Bucket>();
        }
    }

    ThreadSafeHashMap(const ThreadSafeHashMap &other) = delete;
    ThreadSafeHashMap& operator=(const ThreadSafeHashMap &other) = delete;

    Value valueFor(const Key &key, const Value &defaultVal = Value()) const {
        return getBucket(key).valueFor(key, defaultVal);
    }

    void addOrUpdate(const Key &key, const Value& val) {
        getBucket(key).addOrUpdate(key, val);
    }

    void erase(const Key &key) {
        getBucket(key).erase(key);
    }

private:

    // return adress of value that is storing in unique_ptr
    Bucket &getBucket(const Key &key) const {
        const size_t bucketIndex = hasher_(key) & buckets_.size();
        return *buckets_[bucketIndex];
    }
};

#endif //LEARNMULTITHREADC_THREADSAFEHASHMAP_HPP
