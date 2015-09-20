#ifndef SCOPEALLOCATOR_HPP
#define SCOPEALLOCATOR_HPP

template <class T> class ScopeAllocator {
private:
    ScopeAllocator(const ScopeAllocator&);
    ScopeAllocator& operator=(const ScopeAllocator&);

public:
    ScopeAllocator() : obj(new T) { }
    virtual ~ScopeAllocator() { delete obj; };

    T *operator*() { return obj; }

private:
    T *obj;
};

template <class T> class ScopeArrayAllocator {
private:
    ScopeArrayAllocator(const ScopeArrayAllocator&);
    ScopeArrayAllocator& operator=(const ScopeArrayAllocator&);

public:
    ScopeArrayAllocator(int count) : obj(new T[count]) { }
    virtual ~ScopeArrayAllocator() { delete[] obj; };

    T *operator*() { return obj; }

private:
    T *obj;
};

#endif // SCOPEALLOCATOR_HPP
