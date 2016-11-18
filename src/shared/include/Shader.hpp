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

#ifndef _SHADER_HPP_
#define _SHADER_HPP_

#include "Exception.hpp"
#include "ZipReader.hpp"
#include "Properties.hpp"
#include "Subsystem.hpp"
#include "Matrix4x4.hpp"

class ShaderException : public Exception {
public:
    ShaderException(const char *msg) : Exception(msg) { }
    ShaderException(const std::string& msg) : Exception(msg) { }
};

class Shader : public Properties {
private:
    Shader(const Shader&);
    Shader& operator=(const Shader&);

public:
    typedef int ID;

    Shader(const std::string& filename, ZipReader *zip) throw (KeyValueException, ShaderException);
    virtual ~Shader();

    virtual ID get_location(const char *name) throw (ShaderException) = 0;
    virtual ID get_attrib_location(const char *name) throw (ShaderException) = 0;
    virtual void set_value(ID location, int value) = 0;
    virtual void set_value(ID location, float value) = 0;
    virtual void set_value(ID location, float v1, float v2) = 0;
    virtual void set_value(ID location, float v1, float v2, float v3) = 0;
    virtual void set_value(ID location, float v1, float v2, float v3, float v4) = 0;
    virtual void set_value(ID location, size_t sz, int *value) = 0;
    virtual void set_value(ID location, size_t sz, float *value) = 0;
    virtual void set_value(ID location, const Matrix4x4& mat) = 0;
    virtual void activate() = 0;
    virtual void deactivate() = 0;
};

#endif
