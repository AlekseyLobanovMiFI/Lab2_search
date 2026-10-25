#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <stdexcept>
#include "iDictionary.h"
#include "mutableArraySequence.h"

template <class TKey, class TElement>
struct Pair {
    TKey key;
    TElement value;
};

template <class TKey, class TElement>
class HashTable : public IDictionary<TKey, TElement> {
private:
    using Bucket = MutableArraySequence<Pair<TKey, TElement>>;   // одна корзина = список пар

    MutableArraySequence<Bucket*>* buckets;   // массив корзин
    int count;                                // сколько пар лежит всего
    int (*hashFunc)(const TKey&);             // указатель на хеш-функцию

    // номер корзины для ключа при заданной ёмкости;
    // поправка нужна, если хеш-функция вернула отрицательное число (-7 % 10 == -7 в C++)
    int IndexFor(const TKey& key, int capacity) const {
        int h = hashFunc(key) % capacity;
        return h < 0 ? h + capacity : h;
    }

    int IndexFor(const TKey& key) const {
        return IndexFor(key, buckets->GetLength());
    }

    // позиция ключа в корзине или -1, если ключа нет
    int FindInBucket(const Bucket* bucket, const TKey& key) const {
        for (int i = 0; i < bucket->GetLength(); i++) {
            if ((*bucket)[i].key == key) {   // ссылка на пару, без копирования
                return i;
            }
        }
        return -1;
    }

    static const int P = 4;
    static const int Q = 2;
    static const int MIN_CAPACITY = 4;

    void Rehash(int newCapacity) {
        auto* newBuckets = new MutableArraySequence<Bucket*>();
        for (int i = 0; i < newCapacity; i++) {
            newBuckets->Append(new Bucket());
        }

        // перекладываем каждую пару: номер корзины считаем заново от НОВОГО размера
        for (int i = 0; i < buckets->GetLength(); i++) {
            Bucket* oldBucket = (*buckets)[i];
            for (int j = 0; j < oldBucket->GetLength(); j++) {
                Pair<TKey, TElement>& p = (*oldBucket)[j];
                int index = IndexFor(p.key, newCapacity);
                (*newBuckets)[index]->Append(std::move(p));
            }
            delete oldBucket;
        }

        delete buckets;
        buckets = newBuckets;
    }

public:
    HashTable(int (*hashFunc)(const TKey&), int capacity) : count(0), hashFunc(hashFunc)
    {
        if (capacity <= 0) {
            throw std::invalid_argument("InvalidCapacity");
        }
        buckets = new MutableArraySequence<Bucket*>();
        for (int i = 0; i < capacity; i++) {
            buckets->Append(new Bucket());
        }
    }

    ~HashTable() {
        for (int i = 0; i < buckets->GetLength(); i++) {
            delete (*buckets)[i];
        }
        delete buckets;
    }

    // копировать таблицу запрещаем — иначе две таблицы удалят одни и те же корзины
    HashTable(const HashTable&) = delete;
    HashTable& operator=(const HashTable&) = delete;

    int GetCount() const override    { return count; }
    int GetCapacity() const override { return buckets->GetLength(); }

    TElement Get(const TKey& key) const override {
        Bucket* bucket = (*buckets)[IndexFor(key)];
        int pos = FindInBucket(bucket, key);
        if (pos == -1) {
            throw std::out_of_range("KeyNotFound");
        }
        return (*bucket)[pos].value;
    }

    bool ContainsKey(const TKey& key) const override {
        return FindInBucket((*buckets)[IndexFor(key)], key) != -1;
    }

    void Add(const TKey& key, const TElement& elem) override {
        if (ContainsKey(key)) {
            throw std::invalid_argument("KeyAlreadyExists");
        }
        Bucket* bucket = (*buckets)[IndexFor(key)];
        bucket->Append(Pair<TKey, TElement>{key, elem});
        count++;
        if (count == buckets->GetLength()) {
            Rehash(buckets->GetLength() * Q);
        }
    }

    void Remove(const TKey& key) override {
        Bucket* bucket = (*buckets)[IndexFor(key)];
        int pos = FindInBucket(bucket, key);
        if (pos == -1) {
            throw std::out_of_range("KeyNotFound");
        }

        int last = bucket->GetLength() - 1;
        if (pos != last){
            (*bucket)[pos] = std::move((*bucket)[last]);   // последнюю пару — на место удаляемой
        }
        bucket->RemoveLast();               // и отрезаем хвост
        count--;

        int cap = buckets->GetLength();
        if (count <= cap / P && cap / Q >= MIN_CAPACITY) {
            Rehash(cap / Q);
        }
    }
};

#endif
