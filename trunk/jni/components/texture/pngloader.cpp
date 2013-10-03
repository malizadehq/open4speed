//----------------------------------------------------------------------------------------
/**
 * \file       pngloader.cpp
 * \author     Vonasek Lubos
 * \date       2013/04/20
 * \brief      Loading and storing textures from PNG
*/
//----------------------------------------------------------------------------------------

#include "stdafx.h"

/**
 * @brief destruct removes texture from memory is there is no more instance
 */
void pngloader::destruct() {
  instanceCount--;
  if (instanceCount == 0) {
    glDeleteTextures(1, &textureID);
    for (unsigned int i = 0; i < textures.size(); i++) {
        if (strcmp(texturename, textures[i]->texturename) == 0) {
            textures.erase(textures.begin() + i);
        }
    }
  }
}

#ifdef ZIP_ARCHIVE
zip_file* file;

void png_read(png_structp png_ptr, png_bytep data, png_size_t length) {
  zip_fread(file, data, length);
}
#else
FILE* fp;
void png_read(png_structp png_ptr, png_bytep data, png_size_t length) {
  fread(data, length, 1, fp);
}
#endif


/**
 * @brief pngloader loads texture from png file
 * @param filename is name of file
 * @param alpha is amount of blending
 */
pngloader::pngloader(const char* filename, float alpha) {

  unsigned int sig_read = 0;
#ifdef ZIP_ARCHIVE
  file = zip_fopen(APKArchive, prefix(filename), 0);
#else
  fp = fopen(prefix(filename), "rb");
#endif

  png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  png_infop info_ptr = png_create_info_struct(png_ptr);
  setjmp(png_jmpbuf(png_ptr));

  png_set_read_fn(png_ptr, NULL, png_read);

  png_set_sig_bytes(png_ptr, sig_read);
  png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_STRIP_16, NULL);

  int bit_depth, color_type, interlace_type;
  png_uint_32 width, height;
  png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, &interlace_type, NULL, NULL);

  switch (color_type) {
      case PNG_COLOR_TYPE_RGBA:
          alpha = true;
          break;
      case PNG_COLOR_TYPE_RGB:
          alpha = false;
          break;
  }

  unsigned int row_bytes = png_get_rowbytes(png_ptr, info_ptr);
  GLubyte *outData = new GLubyte[row_bytes * height];
  png_bytepp row_pointers = png_get_rows(png_ptr, info_ptr);

  for (unsigned int i = 0; i < height; i++) {
      memcpy(outData+(row_bytes * (height-1-i)), row_pointers[i], row_bytes);
  }

  /* Clean up after the read,
   * and free any memory allocated */
  png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
#ifdef ZIP_ARCHIVE
  zip_fclose(file);
#else
  fclose(fp);
#endif

  /// transform data
  transparent = alpha;

  glGenTextures(1, &textureID);
  glEnable(GL_TEXTURE_2D);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, textureID);
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR );

  if (alpha) {
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, outData);
  } else {
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, outData);
  }

  delete[] outData;
}

/**
 * @brief apply applies current texture
 */
void pngloader::apply() {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureID);
}

void pngloader::setFrame(int frame) {

}
