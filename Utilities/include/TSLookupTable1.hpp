#pragma once

#include <vector>
#include <list>
#include <utility>
#include <mutex>
#include <shared_mutex>
#include <algorithm>
#include <memory>

namespace LCNS::ThreadSafe
{
    template <typename Key, typename Value, typename Hash = std::hash<Key>>
    class LookupTable1
    {
    public:
        LookupTable1(unsigned int bucket_count = 19, const Hash& hasher = Hash());
        LookupTable1(const LookupTable1&)            = delete;
        LookupTable1(LookupTable1&&)                 = delete;
        LookupTable1& operator=(const LookupTable1&) = delete;
        LookupTable1& operator=(LookupTable1&&)      = delete;
        ~LookupTable1()                              = default;

        Value value_for(const Key& key, const Value& default_value = Value()) const;
        void  add_or_update_mapping(const Key& key, const Value& value);
        void  remove_mapping(const Key& key);

    private:
        class Bucket
        {
        private:
            using BucketValue   = std::pair<Key, Value>;
            using BucketData    = std::list<BucketValue>;
            using BucketIt      = BucketData::iterator;
            using BucketConstIt = BucketData::const_iterator;

        public:
            Value value_for(const Key& key, const Value& default_value) const
            {
                std::shared_lock lock(_mutex);

                BucketConstIt it = find_entry_for(key);

                return it == _data.cend() ? default_value : it->second;
            }

            void add_or_update_mapping(const Key& key, const Value& value)
            {
                std::unique_lock lock(_mutex);

                BucketIt it = find_entry_for(key);

                if (it == _data.end())
                {
                    _data.emplace_back(std::make_pair(key, value));
                }
                else
                {
                    it->second = value;
                }
            }

            void remove_mapping(const Key& key)
            {
                std::unique_lock lock(_mutex);

                if (BucketConstIt it = find_entry_for(key); it != _data.end())
                {
                    _data.erase(it);
                }
            }

        private:
            BucketConstIt find_entry_for(const Key& key) const
            {
                return std::find_if(_data.cbegin(), _data.cend(), [&key](const BucketValue& item) { return item.first == key; });
            }

            BucketIt find_entry_for(const Key& key)
            {
                return std::find_if(_data.begin(), _data.end(), [&key](BucketValue& item) { return item.first == key; });
            }

        private:
            BucketData                _data;
            mutable std::shared_mutex _mutex;
        };

    private:
        const Bucket& get_bucket(const Key& key) const;
        Bucket&       get_bucket(const Key& key);

        std::vector<std::unique_ptr<Bucket>> _buckets;
        Hash                                 _hasher;
    };

    template <typename Key, typename Value, typename Hash>
    inline LookupTable1<Key, Value, Hash>::LookupTable1(unsigned int bucket_count, const Hash& hasher)
    : _hasher(hasher)
    {
        _buckets.reserve(bucket_count);
        for (unsigned int i = 0; i < bucket_count; ++i)
        {
            _buckets.emplace_back(new Bucket);
        }
    }
    template <typename Key, typename Value, typename Hash>
    inline Value LookupTable1<Key, Value, Hash>::value_for(const Key& key, const Value& default_value) const
    {
        return get_bucket(key).value_for(key, default_value);
    }

    template <typename Key, typename Value, typename Hash>
    inline void LookupTable1<Key, Value, Hash>::add_or_update_mapping(const Key& key, const Value& value)
    {
        get_bucket(key).add_or_update_mapping(key, value);
    }

    template <typename Key, typename Value, typename Hash>
    inline void LookupTable1<Key, Value, Hash>::remove_mapping(const Key& key)
    {
        get_bucket(key).remove_mapping(key);
    }

    template <typename Key, typename Value, typename Hash>
    inline const LookupTable1<Key, Value, Hash>::Bucket& LookupTable1<Key, Value, Hash>::get_bucket(const Key& key) const
    {
        const std::size_t index = _hasher(key) % _buckets.size();
        return *_buckets.at(index);
    }

    template <typename Key, typename Value, typename Hash>
    inline LookupTable1<Key, Value, Hash>::Bucket& LookupTable1<Key, Value, Hash>::get_bucket(const Key& key)
    {
        const std::size_t index = _hasher(key) % _buckets.size();
        return *_buckets.at(index);
    }
}  // namespace LCNS::TheadSafe
