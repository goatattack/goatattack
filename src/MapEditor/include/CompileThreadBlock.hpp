#ifndef COMPILETHREADBLOCK_HPP
#define COMPILETHREADBLOCK_HPP

#include "CompileThread.hpp"
#include "Thread.hpp"

class CompileThreadBlock : public CompileThread, private Thread {
public:
    CompileThreadBlock(EditableMap *wmap, unsigned char **lightmap);
    virtual ~CompileThreadBlock();

private:
    void thread();
};

#endif // COMPILETHREADBLOCK_HPP
