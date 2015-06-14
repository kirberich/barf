#pragma once

#include <cstring>

class TestVector;
class TestBaseString;

namespace jsonic {

namespace containers{

template<typename Container, typename T>
struct Iterator {
    Iterator(Container* owner):
        owner_(owner), index_(0) {}

    Iterator():
        owner_(nullptr), index_(0) {}

    void operator++() {
        index_++;

        // We've expired the iterator so make it
        // the same as an empty one
        if(index_ == owner_->size()) {
            owner_ = nullptr;
            index_ = 0;
        }
    }

    T& operator->() {
        return owner_->at(index_);
    }

    T& operator*() {
        return owner_->at(index_);
    }

    bool operator==(const Iterator& rhs) const {
        return owner_ == rhs.owner_ && index_ == rhs.index_;
    }

    bool operator!=(const Iterator& rhs) const {
        return !(*this == rhs);
    }

private:
    Container* owner_ = nullptr;
    uint32_t index_ = 0;
};


#ifdef USE_STL
using std::swap;
#else
template<typename _Tp>
inline void swap(_Tp& __a, _Tp& __b) {
    _Tp __tmp = __a;
    __a = __b;
    __b = __tmp;
}
#endif

template<typename T>
class Vector {
    friend class ::TestVector;

public:
    Vector() {}
    Vector(const Vector& rhs) {
        clear();
        reserve(rhs.size());

        for(uint32_t i = 0; i < rhs.size(); ++i) {
            push_back(rhs[i]);
        }
    }

    Vector& operator=(const Vector& rhs) {
        clear();
        reserve(rhs.size());

        for(uint32_t i = 0; i < rhs.size(); ++i) {
            push_back(rhs[i]);
        }

        return *this;
    }

    ~Vector() {
        clear();
    }

    Iterator<Vector<T>, T> begin() { return Iterator<Vector<T>, T>(this); }
    Iterator<Vector<T>, T> end() { return Iterator<Vector<T>, T>(); }

    void push_back(const T& thing) {
        if(reserved_size_ == size_) {
            //Double the reserved size
            uint32_t new_size = reserved_size_ == 0 ? 1U : reserved_size_ * 2;

            // Reserve the new reserved size (if this is the first time)
            // this is equivalent to mallocing a single item
            reallocate(new_size);
        }

        // Initialize a new thing at the right place in the buffer
        new((void*)&array_[size_]) T(thing);

        //Increment the size
        size_++;
    }

    void pop_back() {
        array_[--size_].~T();
    }

    void resize(uint32_t new_size, const T& v=T()) {
        if(new_size < size()) {
            while(size() != new_size) {
                pop_back();
            }
        } else {
            while(size() != new_size) {
                push_back(v);
            }
        }
    }

    void reserve(uint32_t new_size) {
        if(new_size < reserved_size_) {
            return;
        }

        // Set the new reserved size then reallocate the memory
        reallocate(new_size);
    }

    void clear() {
        while(!empty()) {
            pop_back();
        }

        free((void*) array_);
        array_ = nullptr;
        reserved_size_ = 0;
    }

    const T& operator[](uint32_t index) const {
        return array_[index];
    }

    T& operator[](uint32_t index) {
        return array_[index];
    }

    T& at(uint32_t index) const {
#ifdef __EXCEPTIONS
        if(index >= size_) {
            throw std::out_of_range("Tried to access beyond the vector");
        }
#else
        abort();
#endif
        return array_[index];
    }

    T& back() const { return array_[size_ - 1]; }

    bool empty() const { return size_ == 0; }
    uint32_t size() const { return size_; }
private:
    void reallocate(uint32_t new_size) {
        // Allocate the new array
        T* new_array = (T*) malloc(new_size * sizeof(T));

        if(new_array) {
            // Copy-construct the objects in the new buffer
            // using placement new
            for(uint32_t i = 0; i < size(); ++i) {
                new(&new_array[i]) T(array_[i]);

                // Destroy the old counterpart object
                array_[i].~T();
            }

            // Swap the arrays
            swap(array_, new_array);

            // Free the original array
            free((void*) new_array);

            // Update the reserved size
            reserved_size_ = new_size;
        } else {
#ifdef __EXCEPTIONS
            throw std::bad_alloc();
#else
            abort();
#endif
        }
    }

    T* array_ = nullptr;
    uint32_t reserved_size_ = 0;
    uint32_t size_ = 0;
};


template<typename T>
class BaseString {
    friend class ::TestBaseString;
public:
    static const uint32_t npos = -1;

    BaseString() {
        data_.push_back('\0');
    }

    BaseString(const BaseString& rhs) {
        data_ = rhs.data_;
    }

    BaseString(const T* cstr) {
        data_.push_back('\0');

        const T* ch = cstr;
        while(*ch != '\0') {
            push_back(*ch);
            ++ch;
        }
    }
    BaseString(const T* cstr, uint32_t len) {
        data_.push_back('\0');

        const T* ch = cstr;
        for(uint32_t i = 0; i < len; i++) {
            push_back(*ch);
            ++ch;
        }
    }

    bool empty() const { return length() == 0; }

    Iterator<BaseString<T>, T> begin() { return Iterator<BaseString<T>, T>(this); }
    Iterator<BaseString<T>, T> end() { return Iterator<BaseString<T>, T>(); }

    uint32_t find(const BaseString& string, uint32_t pos=0) {
        if(string.empty()) return BaseString::npos;

        char firstc = string[0];
        bool matching = false;
        uint32_t j = 0;
        for(uint32_t i = pos; i < size(); ++i) {
            if(matching) {
                if(j == string.size()) {
                    // If we were matching and we just hit the last character
                    // then we return the offset to the first c
                    return i - j;
                } else if(data_[i] != string[j]) {
                    // Not a match
                    matching = false;
                }
                ++j;
            }

            if(!matching && data_[i] == firstc) {
                // Start searching
                matching = true;
                j = 1;
            }
        }
        return BaseString::npos;
    }

    BaseString& erase(uint32_t pos = 0, uint32_t len=BaseString::npos) {
        uint32_t erase_length = (len == BaseString::npos) ? size() - pos: len;

        if(erase_length == 0) {
            return *this;
        }

        uint32_t max = (pos + erase_length) > length() ? length() : pos + erase_length;

        // Go from the start position, until we hit the erase length
        for(uint32_t i = pos; i < max; ++i) {
            // If there characters after the portion we're erasing
            // we shift them down, otherwise we just replace the character
            // with a null char.
            uint32_t replacement_idx = i + erase_length;
            T replacement = '\0';
            if(replacement_idx < size()) {
                replacement = data_.at(replacement_idx);
            }
            data_[i] = replacement;
        }

        // Finally once we've shifted the entire string down by erase_length
        // we truncate the data
        data_.resize(data_.size() - erase_length);
        data_[data_.size() - 1] = '\0';

        return *this;
    }

    BaseString& operator=(const T* str) {
        *this = BaseString(str);
        return *this;
    }

    BaseString& operator=(const BaseString& rhs) {
        data_.resize(1, '\0');
        for(uint32_t i = 0; i < rhs.size(); ++i) {
            push_back(rhs[i]);
        }
        return *this;
    }

    BaseString substr(uint32_t pos, uint32_t len) {
        T sub[len+1];
        memcpy(sub, c_str() + pos, sizeof(T) * (len + 1));
        return BaseString<T>(sub, len);
    }

    BaseString substr(uint32_t pos = 0) {
        uint32_t len = length() - pos;
        return substr(pos, len);
    }

    BaseString& insert(uint32_t pos, const BaseString& string) {
        if(string.empty()) return *this;

        // Make room, plus the null character
        data_.resize(length() + string.length() + 1);

        // Shift everything up to the insertion point to the end
        for(int64_t i = (int) data_.size() - 2; i >= pos; --i) {
            if(i - int(string.length()) >= 0) {
                data_[i] = data_[i - string.length()];
            }
        }

        // Now copy the string to the insertion point
        for(uint32_t i = 0; i < string.length(); ++i) {
            data_[i + pos] = string[i];
        }

        data_[data_.size() - 1] = '\0';

        return *this;
    }

    bool operator==(const T* rhs) const {
        uint32_t i = 0;
        while(*rhs) {
            if(i >= length() || data_[i] != *rhs) {
                return false;
            }

            ++i;
            ++rhs;
        }

        return i == length();
    }

    bool operator==(const BaseString& rhs) const {
        if(rhs.length() != length()) {
            return false;
        }

        for(uint32_t i = 0; i < length(); ++i) {
            if(data_[i] != rhs.data_[i]) {
                return false;
            }
        }
        return true;
    }

    bool operator!=(const T *rhs) const {
        return !(*this == rhs);
    }

    bool operator!=(const BaseString & rhs) const {
        return !(*this == rhs);
    }

    BaseString operator+(const BaseString & rhs) {
        BaseString<T> result = *this;
        return result.insert(length(), rhs);
    }

    T operator[](const uint32_t idx) const {
        return data_[idx];
    }

    const T* c_str() const {
        return &data_[0];
    }

    void push_back(T c) {
        data_[data_.size() - 1] = c;
        data_.push_back('\0');
    }

    uint32_t size() const { return data_.size() - 1; }
    uint32_t length() const { return data_.size() - 1; }

private:
    Vector<T> data_;
};

template<typename T>
bool operator==(const char* lhs, const BaseString<T>& rhs) {
    return rhs == lhs;
}

template<typename T>
bool operator!=(const char* lhs, const BaseString<T>& rhs) {
    return !(lhs == rhs);
}


typedef BaseString<char> String;

template <typename K, typename V>
struct HashNode {
public:
    // key-value pair
    K first;
    V second;

    // next bucket with the same key
    HashNode *next = nullptr;
};

const int TABLE_SIZE = 10;

// Default hash function class
template <typename K>
struct KeyHash {
    unsigned long operator()(const K& key) const
    {
        return reinterpret_cast<unsigned long>(key) % TABLE_SIZE;
    }
};

template<>
struct KeyHash<String> {
    unsigned long operator()(const String& key) const {
        int hash = 0;

        for(unsigned int i = 0; i<key.length();  i++) {
            hash = 37 * hash + key[i];
        }

        hash %= TABLE_SIZE;

        if(hash < 0) {
             hash += TABLE_SIZE;
        }

        return hash;
    }
};

template <typename K, typename V, typename F>
class HashMap;

template<typename K, typename V, typename F>
struct HashMapIterator {
    HashMapIterator(){} // Represents 'end'

    HashMapIterator(HashMap<K, V, F>* owner):
        owner_(owner) {

    }

    // Stuff to implement!
    void operator++() {}
    HashNode<K, V>& operator->() const {

    }
    HashNode<K, V>& operator*() const {

    }
    bool operator==(const HashMapIterator& rhs) const {}
    bool operator!=(const HashMapIterator) const {}

    HashMap<K, V, F>* owner_ = nullptr;
};


// Hash map class template
template <typename K, typename V, typename F = KeyHash<K> >
class HashMap {
public:
    HashMap() {
        // construct zero initialized hash table of size
        table_.resize(TABLE_SIZE, nullptr);
    }

    ~HashMap() {
        // destroy all buckets one by one
        for (int i = 0; i < table_.size(); ++i) {
            HashNode<K, V> *entry = table_.at(i);
            while (entry) {
                HashNode<K, V> *prev = entry;
                entry = entry->next;
                delete prev;
            }
            table_[i] = nullptr;
        }
        // destroy the hash table
        table_.clear();
    }

    HashMapIterator<K, V, F> begin() { return HashMapIterator<K, V, F>(this); }
    HashMapIterator<K, V, F> end() { return HashMapIterator<K, V, F>(); }

    uint32_t count(const K& key) const {
        unsigned long hash = hash_func_(key);
        HashNode<K, V>* entry = table_[hash];

        while (entry) {
            if (entry->first == key) {
                return 1;
            }
            entry = entry->next;
        }
        return 0;
    }

    V& operator[](const K& key) const {
        return at(key);
    }

    V& at(const K &key) const {
        unsigned long hash = hash_func_(key);
        HashNode<K, V>* entry = table_[hash];

        while (entry) {
            if (entry->first == key) {
                return entry->second;
            }
            entry = entry->next;
        }

#ifdef __EXCEPTIONS
        throw std::out_of_range("No such key exists");
#else
        abort();
#endif
    }

    void insert(const K &key, const V &value) {
        unsigned long hash = hash_func_(key);
        HashNode<K, V>* prev = nullptr;
        HashNode<K, V>* entry = table_[hash];

        while (entry && entry->first != key) {
            prev = entry;
            entry = entry->second;
        }

        if (!entry) {
            entry = new HashNode<K, V>(key, value);
            if (!prev) {
                // insert as first bucket
                table_[hash] = entry;
            } else {
                prev->next = entry;
            }
        } else {
            // just update the value
            entry->second = value;
        }
    }

    void erase(const K &key) {
        unsigned long hash = hash_func_(key);
        HashNode<K, V> *prev = nullptr;
        HashNode<K, V> *entry = table_[hash];

        while (entry && entry->first != key) {
            prev = entry;
            entry = entry->next;
        }

        if (!entry) {
            // key not found
            return;
        }
        else {
            if (!prev) {
                // remove first bucket of the list
                table_[hash] = entry->next;
            } else {
                prev->next = entry->next;
            }
            delete entry;
        }
    }

    void clear() {
        for(uint32_t i = 0; i < table_.size(); ++i) {
            HashNode<K, V> *node = table_[i];
            while(node) {
                HashNode<K, V>* to_delete = node;
                node = node->next;
                delete to_delete;
            }
        }
        table_.clear();
    }

private:
    // hash table
    Vector<HashNode<K, V>*> table_;
    F hash_func_;
};

}


}
