//----------------------------------------------------------------------------------------
/**
 * \file       texture.h
 * \author     Vonasek Lubos
 * \date       2013/04/20
 * \brief      Loading and storing textures
*/
//----------------------------------------------------------------------------------------

#ifndef TEXTURE_H
#define TEXTURE_H

#include "stdafx.h"

/**
 * @brief The texture interface
 */
class texture {
public:
    float alpha;            ///< Texture alpha value for blending
    bool transparent;       ///< Texture transparency
    int instanceCount;      ///< Amount of instances
    char texturename[256];  ///< Texture filename

    /**
     * @brief destruct removes texture from memory is there is no more instance
     */
    virtual void destruct() = 0;

    /**
     * @brief apply applies current texture
     */
    virtual void apply() = 0;

    virtual void setFrame(int frame) = 0;
};

#endif // TEXTURE_H
