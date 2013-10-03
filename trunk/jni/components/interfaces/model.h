//----------------------------------------------------------------------------------------
/**
 * \file       model.h
 * \author     Vonasek Lubos
 * \date       2013/04/20
 * \brief      Loading and storing models
*/
//----------------------------------------------------------------------------------------

#ifndef MODEL_H
#define MODEL_H

#include "stdafx.h"

/**
 * @brief The edge struct
 */
struct edge {
    float ax;    ///< Point coordinate
    float ay;    ///< Point coordinate
    float az;    ///< Point coordinate
    float bx;    ///< Point coordinate
    float by;    ///< Point coordinate
    float bz;    ///< Point coordinate
};

/**
 * @brief The region struct
 */
struct region {
    float minX; ///< Region extreme
    float minY; ///< Region extreme
    float minZ; ///< Region extreme
    float maxX; ///< Region extreme
    float maxY; ///< Region extreme
    float maxZ; ///< Region extreme
    float size; ///< Region size
};

/**
 * @brief The model3d struct
 */
struct model3d {
    bool touchable;
    int filter;
    shader* material;
    float *colora;      ///< Ambient color
    float *colord;      ///< Diffuse color
    float *colors;      ///< Specular color
    bool dynamic;       ///< True if object is dynamic
    int dynamicID;      ///< ID of the last dynamic update
    region *reg;        ///< AABB of the object
    int* triangleCount; ///< Amount of triangles
    texture* texture2D; ///< Object texture
    float* vertices;    ///< Object vertices
    float* normals;     ///< Object normals
    float* coords;      ///< Object texture coordinates
    GLuint vbo;         ///< VBO for current object
    float x,y,z;        ///< Object translation
};

/**
 * @brief The model interface
 */
class model {
public:

    /**
     * @brief model destructor
     */
    virtual ~model() {}

    int cutX, cutY;                            ///< Size of SS Culling
    std::vector<edge> *edges;                  ///< All standalone edges of model
    std::vector<model3d> models;               ///< All parts of model
    float minx, miny, minz, maxx, maxy, maxz;  ///< Extremes of current model
    float width, aplitude, height, size;       ///< Dimensions of current model
};


#endif // MODEL_H
