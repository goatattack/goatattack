#ifndef COMPILETHREAD_HPP
#define COMPILETHREAD_HPP

#include "EditableMap.hpp"
#include "Mutex.hpp"
#include "Thread.hpp"

#include <string>

class CompileThread {
public:
    CompileThread(EditableMap *wmap, unsigned char **lightmap);
    virtual ~CompileThread() = 0;

    bool is_finished();
    int get_percentage();

protected:
    struct Point {
        Point() : x(0), y(0) { }
        Point(float x, float y) : x(x), y(y) { }
        float x;
        float y;
    };

    EditableMap *wmap;
    unsigned char **lightmap;
    bool finished;
    int finished_percent;
    Mutex mtx;

    static bool intersection(const Point& p1, const Point& p2,
        const Point& p3, const Point& p4, Point& intersection);
};

#endif // COMPILETHREAD_HPP
