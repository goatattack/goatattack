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

#ifndef DEDICATED_SERVER
#ifndef _WIN32

#include "ShaderGL.hpp"
#include "MultiReader.hpp"
#include "AutoPtr.hpp"

static const int ErrorBuffer = 1024;

ShaderGL::ShaderGL(Subsystem& subsystem, const std::string& filename, ZipReader *zip)
    throw (KeyValueException, ShaderException)
    : Shader(filename, zip), subsystem(subsystem), program_id(0),
      vert_id(0), frag_id(0)
{
    try {
        /* read in codes */
        MultiReader mr_vert(filename + ".vert", zip);
        MultiReader mr_frag(filename + ".frag", zip);
        AutoPtr<char[]> code_vert(new char[mr_vert.get_size() + 1]);
        AutoPtr<char[]> code_frag(new char[mr_frag.get_size() + 1]);
        mr_vert.read(&code_vert[0], mr_vert.get_size());
        mr_frag.read(&code_frag[0], mr_frag.get_size());
        code_vert[mr_vert.get_size()] = 0;
        code_frag[mr_frag.get_size()] = 0;

        /* create program, compile code and link program */
        program_id = glCreateProgram();
        vert_id = glCreateShader(GL_VERTEX_SHADER);
        compile(vert_id, &code_vert[0]);
        frag_id = glCreateShader((GL_FRAGMENT_SHADER));
        compile(frag_id, &code_frag[0]);
        glAttachShader(program_id, vert_id);
        glAttachShader(program_id, frag_id);
        glLinkProgram(program_id);
    } catch (const Exception& e) {
        cleanup();
        throw ShaderException(e.what());
    }
}

ShaderGL::~ShaderGL() {
    cleanup();
}

Shader::ID ShaderGL::get_location(const char *name) throw (ShaderException) {
    ID id = glGetUniformLocation(program_id, name);
    if (id < 0) {
        subsystem << "*** WARNING ***: " << ": uniform location '" << name << "' not found." << std::endl;
    }

    return id;
}

Shader::ID ShaderGL::get_attrib_location(const char *name) throw (ShaderException) {
    ID id = glGetAttribLocation(program_id, name);
    if (id < 0) {
        subsystem << "*** WARNING ***: " << ": attrib location '" << name << "' not found." << std::endl;
    }

    return id;
}

void ShaderGL::set_value(ID location, int value) {
    glUniform1i(location, value);
}

void ShaderGL::set_value(ID location, float value) {
    glUniform1f(location, value);
}

void ShaderGL::set_value(ID location, float v1, float v2) {
    glUniform2f(location, v1, v2);
}
void ShaderGL::set_value(ID location, float v1, float v2, float v3) {
    glUniform3f(location, v1, v2, v3);
}

void ShaderGL::set_value(ID location, float v1, float v2, float v3, float v4) {
    glUniform4f(location, v1, v2, v3, v4);
}

void ShaderGL::set_value(ID location, size_t sz, int *value) {
    glUniform1iv(location, static_cast<GLsizei>(sz), value);
}

void ShaderGL::set_value(ID location, size_t sz, float *value) {
    glUniform1fv(location, static_cast<GLsizei>(sz), value);
}

void ShaderGL::set_value(ID location, const Matrix4x4& mat) {
    glUniformMatrix4fv(location, 1, GL_FALSE, mat.v);
}

void ShaderGL::activate() {
    glUseProgram(program_id);
}

void ShaderGL::deactivate() {
    glUseProgram(0);
}

void ShaderGL::compile(GLuint id, const char *program) throw (ShaderException) {
    char error[ErrorBuffer];

    memset(error, 0, sizeof(error));
    glShaderSource(id, 1, &program, 0);
    glCompileShader(id);
    glGetShaderInfoLog(id, sizeof(error) - 1, 0, error);
    if (strlen(error)) {
        throw ShaderException(error);
    }
}

void ShaderGL::cleanup() {
    if (frag_id) {
        glDeleteShader(frag_id);
        frag_id = 0;
    }
    if (vert_id) {
        glDeleteShader(vert_id);
        vert_id = 0;
    }
    if (program_id) {
        glDeleteProgram(program_id);
        program_id = 0;
    }
}

#endif
#endif
