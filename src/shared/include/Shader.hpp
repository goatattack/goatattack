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

#ifdef __unix__
#include <SDL2/SDL_opengl.h>
#elif _WIN32
#include <SDL_opengl.h>
#endif

class ShaderException : public Exception {
public:
    ShaderException(const char *msg) : Exception(msg) { }
    ShaderException(const std::string& msg) : Exception(msg) { }
};

class Shader {
private:
    Shader(const Shader&);
    Shader& operator=(const Shader&);

public:
    Shader(const std::string& filename, ZipReader *zip) throw (ShaderException);
    ~Shader();

private:
    GLuint program_id;
    GLuint vert_id;
    GLuint frag_id;
};

#endif