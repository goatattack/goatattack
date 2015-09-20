#ifndef EDITABLEOBJECT_H
#define EDITABLEOBJECT_H

#include "Object.hpp"

class EditableObject {
public:
    EditableObject(Object *object, int x, int y);
    virtual ~EditableObject();

    Object *object;
    int x;
    int y;
    int spawn_counter;
};

#endif // EDITABLEOBJECT_H
