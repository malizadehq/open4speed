//----------------------------------------------------------------------------------------
/**
 * \file       pngloader.h
 * \author     Vonasek Lubos
 * \date       2013/04/20
 * \brief      Loading and storing textures from PNG
*/
//----------------------------------------------------------------------------------------

#ifndef PNGLOADER_H
#define PNGLOADER_H

#include "stdafx.h"

/**
 * @brief The pngloader class is png texture loader
 */
class pngloader : public texture {
public:

    unsigned int textureID;     ///< Texture id

    /**
     * @brief destruct removes texture from memory is there is no more instance
     */
    void destruct();

    /**
     * @brief bmp24 loads texture from png file
     * @param filename is name of file
     * @param alpha is amount of blending
     */
    pngloader(const char* filename, float alpha);

    /**
     * @brief apply applies current texture
     */
    void apply();

    void setFrame(int frame);
};

#endif // PNGLOADER_H
