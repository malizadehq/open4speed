#ifndef SHADER_H
#define SHADER_H

class shader
{
public:
    char shadername[256];  ///< Shader filename

    virtual ~shader() {}

    virtual void attrib(int size) = 0;
    virtual void attrib(float* vertices, float* coords) = 0;

    virtual void bind() = 0;
    virtual void unbind() = 0;
    virtual void uniformInt(const char* name, int value) = 0;
    virtual void uniformFloat(const char* name, float value) = 0;
    virtual void uniformFloat4(const char* name, float a, float b, float c, float d) = 0;
    virtual void uniformMatrix(const char* name, float* value) = 0;

};

#endif // SHADER_H
