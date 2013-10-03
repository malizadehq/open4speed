#ifndef GLSL_H
#define GLSL_H

class glsl : public shader
{
unsigned int id;            ///< Shader id
GLint attribute_v_vertex;   ///< VBO vertices
GLint attribute_v_coord;    ///< VBO coords
GLint attribute_v_normal;   ///< VBO normals

public:
    glsl(std::vector<char*> vert, std::vector<char*> frag);

    void attrib(int size);
    void attrib(float* vertices, float* coords);

    void bind();

    void unbind();
    void uniformInt(const char* name, int value);
    void uniformFloat(const char* name, float value);
    void uniformFloat4(const char* name, float a, float b, float c, float d);
    void uniformMatrix(const char* name, float* value);
};

#endif // GLSL_H
