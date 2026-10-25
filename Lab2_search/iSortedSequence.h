#ifndef ISORTED_SEQUENCE_H
#define ISORTED_SEQUENCE_H

template <class T>
class ISortedSequence {
public:
    virtual ~ISortedSequence() = default;

    virtual int GetLength() const = 0;
    virtual bool GetIsEmpty() const = 0;

    virtual const T& Get(int index) const = 0;
    virtual const T& GetFirst() const = 0;
    virtual const T& GetLast() const = 0;

    virtual int IndexOf(const T& x) const = 0;       // индекс или -1
    virtual void Add(const T& x) = 0;                // вставка с сохранением порядка

    virtual int LowerBound(const T& x) const = 0;    // первый индекс, где элемент >= x
    virtual int UpperBound(const T& x) const = 0;    // первый индекс, где элемент >  x

    // Новый объект в куче: удаляет тот, кто вызвал (delete).
    // По значению вернуть нельзя — ISortedSequence абстрактный.
    virtual ISortedSequence<T>* GetSubsequence(int from, int to) const = 0;
    virtual ISortedSequence<T>* GetRange(const T& a, const T& b) const = 0;   // a <= x <= b
};

#endif
