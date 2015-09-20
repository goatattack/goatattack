#ifndef COMPILETHREADPIXEL_HPP
#define COMPILETHREADPIXEL_HPP

#include "CompileThread.hpp"
#include "Thread.hpp"

class CompileThreadPixel : public CompileThread, private Thread {
public:
    CompileThreadPixel(EditableMap *wmap, unsigned char **lightmap);
    virtual ~CompileThreadPixel();

private:
    void thread();
};

#endif // COMPILETHREADPIXEL_HPP
