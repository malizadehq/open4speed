//----------------------------------------------------------------------------------------
/**
 * \file       pxxloader.h
 * \author     Vonasek Lubos
 * \date       2013/04/20
 * \brief      Loading and storing sequences of PNG
*/
//----------------------------------------------------------------------------------------

#ifndef PXXLOADER_H
#define PXXLOADER_H

#include "stdafx.h"

/**
 * @brief The pxxloader class is png sequence loader
 */
class pxxloader : public texture {
public:
    std::vector<pngloader*> anim;
    unsigned int currentFrame;
    unsigned int currentMultiFrame;
    unsigned int multiFrame;
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
    pxxloader(const char* filename, float alpha);

    /**
     * @brief apply applies current texture
     */
    void apply();

    void setFrame(int frame);
};

#endif // PXXLOADER_H
