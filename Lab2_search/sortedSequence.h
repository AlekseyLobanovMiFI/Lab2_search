#ifndef SORTED_SEQUENCE_H
#define SORTED_SEQUENCE_H

#include <stdexcept>
#include "mutableArraySequence.h"
#include "iSortedSequence.h"

template <class T>
class SortedSequence : public ISortedSequence<T> {
private:
    MutableArraySequence<T> items;   // элементы, всегда по возрастанию
    int splitLeft = 1;               // пропорция деления в бинарном поиске — левая часть : правая часть
    int splitRight = 1;              // mid = lo + (hi - lo) * left / (left + right); 1:1 — пополам


    int Bound(const T& x, bool upper) const {
        int lo = 0;
        int hi = items.GetLength();
        while (lo < hi){
            int mid = lo + (int)((long long)(hi - lo) * splitLeft / (splitLeft + splitRight));
            const T& m = items[mid];
            bool goRight;
            if (upper) {
                goRight = !(x < m);   // m <= x
            } else {
                goRight = m < x;
            }
            if (goRight) {
                lo = mid + 1;
            } else {
                hi = mid;
            }
        }
        return lo;
    }


public:
    SortedSequence() = default;

    // готовая последовательность из массива; проверяем, что он правда отсортирован
    SortedSequence(const T* data, int count) : items(data, count) {
        for (int i = 1; i < count; i++) {
            if (items[i] < items[i - 1]) {
                throw std::invalid_argument("NotSorted");
            }
        }
    }

    // пропорция деления: (1,1) пополам, (618,382) золотое сечение, любые свои > 0
    void SetSplit(int left, int right) {
        if (left <= 0 || right <= 0) {
            throw std::invalid_argument("InvalidSplit");
        }
        splitLeft = left;
        splitRight = right;
    }

    int LowerBound(const T& x) const override { return Bound(x, false); }
    int UpperBound(const T& x) const override { return Bound(x, true); }

    int IndexOf(const T& x) const override {
        int i = LowerBound(x);
        if ((i < GetLength()) && (!(x < items[i]) && !(items[i] < x))){
            return i;
        }
        return -1;
    }

    void Add(const T& x) override {
        int pos = UpperBound(x);
        items.Append(x);
        for (int i = GetLength()-1; i>pos; i--){
            items[i] = std::move(items[i-1]);
        }
        items[pos] = x;
    }

    // возвращает SortedSequence<T>* вместо ISortedSequence<T>* — так можно (ковариантный тип возврата)
    SortedSequence<T>* GetSubsequence(int from, int to) const override {
        if (from < 0 || from > to || to >= GetLength()) {
            throw std::out_of_range("IndexOutOfRange");
        }
        SortedSequence<T>* result = new SortedSequence<T>();
        result->splitLeft = splitLeft;
        result->splitRight = splitRight;
        for (int i = from; i <= to; i++){
            result->items.Append(items[i]);
        }
        return result;
    }

    SortedSequence<T>* GetRange(const T& a, const T& b) const override {
        int lo = LowerBound(a);
        int hi = UpperBound(b) - 1;
        if (lo > hi){
            SortedSequence<T>* empty = new SortedSequence<T>();
            empty->splitLeft = splitLeft;
            empty->splitRight = splitRight;
            return empty;
        }
        return GetSubsequence(lo, hi);
    }



    int GetLength() const override {
        return items.GetLength();
    }

    bool GetIsEmpty() const override {
        return items.GetLength() == 0;
    }

    const T& Get(int index) const override {
        return items[index];               // за границами — IndexOutOfRange из DynamicArray
    }

    const T& GetFirst() const override {            // минимум
        if (GetIsEmpty()) {
            throw std::out_of_range("EmptySequence");
        }
        return items[0];
    }

    const T& GetLast() const override {             // максимум
        if (GetIsEmpty()) {
            throw std::out_of_range("EmptySequence");
        }
        return items[items.GetLength() - 1];
    }
};

#endif