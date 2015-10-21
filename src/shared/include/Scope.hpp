#ifndef SCOPE_HPP
#define SCOPE_HPP

template<class T> class Scope {
public:
    Scope(T& obj) : obj(obj) {
        obj.enter_scope();
    }

    ~Scope() {
        obj.leave_scope();
    }

private:
    T& obj;
};

#endif