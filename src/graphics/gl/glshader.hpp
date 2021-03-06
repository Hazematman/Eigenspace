#ifndef GLSHADER_HPP
#define GLSHADER_HPP
#include <string>
#include "gl_3_3.hpp"

class ShaderProgram {
  public:
    ShaderProgram(const std::string &vertex, const std::string &fragment);
    ~ShaderProgram();

    GLuint getID();
    GLint getUniform(const std::string &name);
  private:
    GLuint programID;
    GLuint vertexShader, fragmentShader;
    std::string vertexFile, fragmentFile;

    GLuint compileShader(const std::string &file, GLenum type);
    std::string getLog(GLuint object);
    std::string loadFile(const std::string &filename);
};

#endif
