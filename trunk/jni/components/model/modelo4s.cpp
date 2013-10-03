//----------------------------------------------------------------------------------------
/**
 * \file       modelo4s.cpp
 * \author     Vonasek Lubos
 * \date       2013/04/20
 * \brief      Class for loading models into OpenGL list, it parse full model file, apply
 *             textures, materials and blending.
*/
//----------------------------------------------------------------------------------------

#include "stdafx.h"

/**
 * @brief modelo4s destructor
 */
modelo4s::~modelo4s() {
    for (unsigned int i = 0; i < models.size(); i++) {
        glDeleteBuffers(1, &models[i].vbo);
        delete[] models[i].colora;
        delete[] models[i].colord;
        delete[] models[i].colors;
        delete models[i].reg;
        delete[] models[i].vertices;
        delete[] models[i].normals;
        delete[] models[i].coords;
        delete[] models[i].triangleCount;
        models[i].texture2D->destruct();
    }
    models.clear();
}


int scandec(char* line) {
    int number = 0;
    for (int i = 0; i < 1024; i++) {
        if (line[i] != 10) {
            number = number * 10 + line[i] - '0';
        } else {
            return number;
        }
    }
    return number;
}

char c[1];

#ifdef ZIP_ARCHIVE
void gets(char* line, zip_file* file) {
    for (int i = 0; i < 1020; i++) {
        zip_fread(file, c, 1);
        line[i] = c[0];
        if (line[i] == '\n') {
            line[i + 1] = '\000';
            return;
        }
    }
}
#endif

void gets(char* line, FILE* file) {
    for (int i = 0; i < 1020; i++) {
        fread(c, 1, 1, file);
        line[i] = c[0];
        if ((line[i] == 10) || (line[i] == 13)) {
            line[i] = '\n';
            line[i + 1] = '\000';
            return;
        }
    }
}


/**
 * @brief Constructor for loading model from file
 * @param filename is path and name of file to load
 */
modelo4s::modelo4s(const char* filename) {

    models = *(new std::vector<model3d>());

    /// open file
#ifdef ZIP_ARCHIVE
    zip_file* file = zip_fopen(APKArchive, prefix(filename), 0);
#else
    FILE* file = fopen(prefix(filename), "r");
#endif
    char line[1024];

    /// get model dimensions
    gets(line, file);
    cutX = scandec(line);
    gets(line, file);
    cutY = scandec(line);
    gets(line, file);
    sscanf(line, "%f %f %f %f %f %f", &minx, &miny, &minz, &maxx, &maxy, &maxz);
    width = maxx - minx;
    aplitude = maxy - miny;
    height = maxz - minz;
    size = width;
    if (size < aplitude)
        size = aplitude;
    if (size < height)
        size = height;

    /// get amount of textures in model
    gets(line, file);
    int textureCount = scandec(line);

    /// parse all textures
    for (int i = 0; i < textureCount; i++) {

        /// set default value
        model3d *m = new model3d();
        m->reg = new region();
        m->triangleCount = new int[cutX * cutY + 1];
        GLfloat *colora = new GLfloat[4];
        GLfloat *colord = new GLfloat[4];
        GLfloat *colors = new GLfloat[4];
        colora[3] = 1;
        colord[3] = 1;
        colors[3] = 1;

        /// get material attributes
        float alpha = 1;
        char texturePath[255];
        char material[255];
        material[0] = '\0';
        gets(line, file);
        sscanf(line, "%f %f %f %f %f %f %s %f %f %f %f %f %f %f %f %f %f %s",
               &m->reg->minX, &m->reg->minY, &m->reg->minZ, &m->reg->maxX, &m->reg->maxY, &m->reg->maxZ,
               &texturePath[0], &colora[0], &colora[1], &colora[2], &colord[0], &colord[1], &colord[2],
               &colors[0], &colors[1], &colors[2], &alpha, &material[0]);

        /// get model size
        m->reg->size = m->reg->maxX - m->reg->minX;
        if (m->reg->size < m->reg->maxY - m->reg->minY)
            m->reg->size = m->reg->maxY - m->reg->minY;
        if (m->reg->size < m->reg->maxZ - m->reg->minZ)
            m->reg->size = m->reg->maxZ - m->reg->minZ;
        m->x = m->reg->minX;
        m->y = m->reg->minY;
        m->z = m->reg->minZ;

        /// if texture is not only single color then load it
        if (texturePath[0] != '*') {
            m->texture2D = getTexture(texturePath, alpha);
        /// create color texture
        } else {
            m->texture2D = new rgb(colord[0], colord[1], colord[2], alpha);
        }

        int cursor = 0;
        m->dynamic = false;
        m->filter = 0;
        m->touchable = true;
        if (m->texture2D->transparent) {
            m->material = getShader("standart_alpha");
        } else {
            m->material = getShader("standart");
        }
        while(true) {
            if (material[cursor] == '!') {
                m->touchable = false;
                cursor++;
            } else if (material[cursor] == '$') {
                m->dynamic = true;
                cursor++;
            } else if (material[cursor] == '#') {
                cursor++;
                m->filter = material[cursor] - '0';
                cursor++;
            } else if (material[cursor] == '%') {
                cursor++;
                char* shadername = new char[strlen(material) - cursor + 1];
                for (unsigned int j = cursor; j < strlen(material); j++) {
                    shadername[j - cursor] = material[j];
                    if (material[j] == '/') {
                        shadername[j - cursor] = '\000';
                        break;
                    }
                }
                shadername[strlen(material) - cursor] = '\000';
                m->material = getShader(shadername);
                break;
            } else {
                break;
            }
        }

        /// prepare model arrays
        m->triangleCount[0] = 0;
        for (int j = 1; j <= cutX * cutY; j++) {
            gets(line, file);
            m->triangleCount[j] = scandec(line);
        }
        m->vertices = new GLfloat[m->triangleCount[cutX * cutY] * 3 * 3];
        m->normals = new GLfloat[m->triangleCount[cutX * cutY] * 3 * 3];
        m->coords = new GLfloat[m->triangleCount[cutX * cutY] * 3 * 2];
        m->colora = colora;
        m->colord = colord;
        m->colors = colors;
        for (int j = 0; j < m->triangleCount[cutX * cutY]; j++) {
            /// read triangle parameters
            gets(line, file);
            sscanf(line, "%f %f %f %f %f %f %f %f%f %f %f %f %f %f %f %f%f %f %f %f %f %f %f %f",
                   &m->coords[j * 3 * 2 + 0], &m->coords[j * 3 * 2 + 1],
                   &m->normals[j * 3 * 3 + 0], &m->normals[j * 3 * 3 + 1], &m->normals[j * 3 * 3 + 2],
                   &m->vertices[j * 3 * 3 + 0], &m->vertices[j * 3 * 3 + 1], &m->vertices[j * 3 * 3 + 2],
                   &m->coords[j * 3 * 2 + 2], &m->coords[j * 3 * 2 + 3],
                   &m->normals[j * 3 * 3 + 3], &m->normals[j * 3 * 3 + 4], &m->normals[j * 3 * 3 + 5],
                   &m->vertices[j * 3 * 3 + 3], &m->vertices[j * 3 * 3 + 4], &m->vertices[j * 3 * 3 + 5],
                   &m->coords[j * 3 * 2 + 4], &m->coords[j * 3 * 2 + 5],
                   &m->normals[j * 3 * 3 + 6], &m->normals[j * 3 * 3 + 7], &m->normals[j * 3 * 3 + 8],
                   &m->vertices[j * 3 * 3 + 6], &m->vertices[j * 3 * 3 + 7], &m->vertices[j * 3 * 3 + 8]);
        }

        /// store model in VBO
        int size = sizeof(GLfloat)*m->triangleCount[cutX * cutY] * 3;
        glGenBuffers(1, &m->vbo);
        glBindBuffer(GL_ARRAY_BUFFER, m->vbo);
        glBufferData(GL_ARRAY_BUFFER, size * 8, NULL, GL_STATIC_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, size * 3, m->vertices);
        glBufferSubData(GL_ARRAY_BUFFER, size * 3, size * 3, m->normals);
        glBufferSubData(GL_ARRAY_BUFFER, size * 6, size * 2, m->coords);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        models.push_back(*m);
    }

    /// load edges
    gets(line, file);
    int trackCount = scandec(line);
    edges = new std::vector<edge>[trackCount];
    for (int i = 0; i < trackCount; i++) {
        gets(line, file);
        int edgeCount = scandec(line);
        for (int j = 0; j < edgeCount; j++) {
            edge value = *(new edge());
            gets(line, file);
            sscanf(line, "%f %f %f %f %f %f", &value.ax, &value.ay, &value.az, &value.bx, &value.by, &value.bz);
            edges[i].push_back(value);
        }
        for (int j = 0; j < edgeCount; j++) {
            edge value = *(new edge());
            value.ax = edges[i][j].bx;
            value.ay = edges[i][j].by;
            value.az = edges[i][j].bz;
            value.bx = edges[i][j].ax;
            value.by = edges[i][j].ay;
            value.bz = edges[i][j].az;
            edges[i].push_back(value);
        }
    }

#ifdef ZIP_ARCHIVE
    zip_fclose(file);
#else
    fclose(file);
#endif
}
