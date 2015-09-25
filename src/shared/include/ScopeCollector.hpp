#ifndef SCOPECOLLECTOR_HPP
#define SCOPECOLLECTOR_HPP

template <class T> class ScopeCollector {
private:
    ScopeCollector(const ScopeCollector&);
    ScopeCollector& operator=(const ScopeCollector&);

public:
    ScopeCollector(T *obj) : obj(obj) { }
    virtual ~ScopeCollector() { delete obj; };

    T *operator*() { return obj; }

private:
    T *obj;
};

template <class T> class ScopeArrayCollector {
private:
    ScopeArrayCollector(const ScopeArrayCollector&);
    ScopeArrayCollector& operator=(const ScopeArrayCollector&);

public:
    ScopeArrayCollector(T *obj) : obj(obj) { }
    virtual ~ScopeArrayCollector() { delete[] obj; };

    T *operator*() { return obj; }

private:
    T *obj;
};

#endif
