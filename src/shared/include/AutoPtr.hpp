#ifndef _AUTOPTR_HPP_
#define _AUTOPTR_HPP_

template<typename T> class AutoPtr {
private:
    AutoPtr(const AutoPtr&);
    const AutoPtr& operator=(const AutoPtr&);

public:
    AutoPtr(T *obj) : obj(obj)  { }
    ~AutoPtr()                  { reset(); }

    T *operator->()             { return obj; }
    T& operator*()              { return *obj; }
    bool valid()                { return obj != 0; }
    T *release()                { T *tmp = obj; obj = 0; return tmp; }
    void reset()                { delete obj; obj = 0; }

private:
    T *obj;
};

template<typename T> class AutoPtr<T[]> {
private:
    AutoPtr(const AutoPtr&);
    const AutoPtr& operator=(const AutoPtr&);

public:
    AutoPtr(T *obj) : obj(obj)  { }
    ~AutoPtr()                  { reset(); }

    T *operator->()             { return obj; }
    T& operator[](int index)    { return obj[index]; }
    bool valid()                { return obj != 0; }
    T *release()                { T *tmp = obj; obj = 0; return tmp; }
    void reset()                { delete[] obj; obj = 0; }

private:
    T *obj;
};

#endif