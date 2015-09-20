#include "EditableObject.hpp"

EditableObject::EditableObject(Object *object, int x, int y)
    : object(object), x(x), y(y) { }

EditableObject::~EditableObject() { }
