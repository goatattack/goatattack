/*
 *  This file is part of Goat Attack.
 *
 *  Goat Attack is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Goat Attack is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Goat Attack.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _SHADERGL_HPP_
#define _SHADERGL_HPP_

#include "Shader.hpp"

#ifdef __unix__
#include <SDL2/SDL_opengl.h>
#elif _WIN32
#include <SDL_opengl.h>
#endif

class ShaderGL : public Shader {
private:
    ShaderGL(const ShaderGL&);
    ShaderGL& operator=(const ShaderGL&);

public:
    typedef GLint ID;

    ShaderGL(Subsystem& subsystem, const std::string& filename, ZipReader *zip);
    virtual ~ShaderGL();

    virtual ID get_location(const char *name);
    virtual ID get_attrib_location(const char *name);
    virtual void set_value(ID location, int value);
    virtual void set_value(ID location, float value);
    virtual void set_value(ID location, float v1, float v2);
    virtual void set_value(ID location, float v1, float v2, float v3);
    virtual void set_value(ID location, float v1, float v2, float v3, float v4);
    virtual void set_value(ID location, size_t sz, int *value);
    virtual void set_value(ID location, size_t sz, float *value);
    virtual void set_value(ID location, const Matrix4x4& mat);
    virtual void activate();
    virtual void deactivate();

private:
    Subsystem& subsystem;
    GLuint program_id;
    GLuint vert_id;
    GLuint frag_id;

    void compile(GLuint id, const char *program);
    void cleanup();
};

#endif
