#ifndef IDICTIONARY_H
#define IDICTIONARY_H

template <class TKey, class TElement>
class IDictionary {
public:
    virtual ~IDictionary() = default;

    virtual int GetCount() const = 0;
    virtual int GetCapacity() const = 0;

    virtual TElement Get(const TKey& key) const = 0;
    virtual bool ContainsKey(const TKey& key) const = 0;
    virtual void Add(const TKey& key, const TElement& element) = 0;
    virtual void Remove(const TKey& key) = 0;
};

#endif