//----------------------------------------------------------------------------------------
/**
 * \file       rgb.h
 * \author     Vonasek Lubos
 * \date       2013/04/20
 * \brief      Loading and storing textures
*/
//----------------------------------------------------------------------------------------

#ifndef RGB_H
#define RGB_H

#include "stdafx.h"

/**
 * @brief The rgb class
 */
class rgb : public texture {
public:

    unsigned int textureID;     ///< Texture id

    /**
     * @brief destruct removes texture from memory is there is no more instance
     */
    void destruct();

    /**
     * @brief rgb creates texture from color
     * @param r is red color value
     * @param g is green color value
     * @param b is blue color value
     * @param alpha is amount of blending
     */
    rgb(float r, float g, float b, float alpha);

    /**
     * @brief apply applies current texture
     */
    void apply();

    void setFrame(int frame);
};

#endif // RGB_H
