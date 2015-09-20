#ifndef EDITABLELIGHT_HPP
#define EDITABLELIGHT_HPP

class EditableLight {
public:
    EditableLight(int x, int y, int radius);
    virtual ~EditableLight();

    int x;
    int y;
    int radius;
};

#endif // EDITABLELIGHT_HPP
