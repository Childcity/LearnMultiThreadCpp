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


namespace childcity {
namespace threadsafehash {


template <class Key, class Value, class Hash = std::hash<Key>>
struct ThreadSafeHashMap {
private:
    struct Bucket {
    private:
        using BucketValue = std::pair<Key, Value>;
        using BucketData = std::list<BucketValue>;

        BucketData data_;
        mutable std::shared_mutex bucketMutex_;

    public:
        Value valueFor(const Key &key, const Value &defaultVal)
        {
            std::shared_lock<std::shared_mutex> lock(bucketMutex_);
            const auto foundedEntry = findEntryFor(key);
            return (foundedEntry == data_.end()) ? defaultVal : foundedEntry->second;
        }

        void addOrUpdate(const Key &key, const Value &value)
        {
            std::unique_lock<std::shared_mutex> lock(bucketMutex_);
            auto foundedEntry = findEntryFor(key);
            if(foundedEntry == data_.end()){
                data_.emplace_back(BucketValue(key, value));
            } else {
                foundedEntry->second = value;
            }
        }

        void erase(const Key &key)
        {
            std::unique_lock<std::shared_mutex> lock(bucketMutex_);
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
    std::vector<std::unique_ptr<Bucket>> buckets_;
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

    void remove(const Key &key) {
        getBucket(key).erase(key);
    }

private:

    // return adress of value that is storing in std::unique_ptr
    Bucket &getBucket(const Key &key) const {
        static const size_t lastIndex = buckets_.size() - 1;
        const size_t bucketIndex = hasher_(key) & lastIndex;
        return *buckets_[bucketIndex];
    }
};


    } // namespace threadsafehash
} // namespace childcity

#endif //LEARNMULTITHREADC_THREADSAFEHASHMAP_HPP
