//----------------------------------------------------------------------------------------
/**
 * \file       glphong.cpp
 * \author     Vonasek Lubos
 * \date       2013/04/20
 * \brief      GL renderer draws geometry and other things on screen
*/
//----------------------------------------------------------------------------------------

#include "stdafx.h"

struct RTT {
    GLuint rendertexture;        ///< Render to texture texture
    GLuint rboID;                ///< Render buffer object id
    GLuint fboID;                ///< Frame buffer object id
};

GLushort dynindices[4095];
int culling = 300;
int frame = 0;
float camX;
float camZ;
bool oddFrame;
RTT rtt[2];
int res = 0;                     ///< Resolution for antialiasing
texture *glslfont = 0;           ///< Font texture
shader* gui_shader = 0;
shader* scene_shader = 0;

/// GUI projection matrix
glm::mat4x4 gui_projection_matrix(
    (float)(1 / tan(45 * (3.1415926535 / 180))), 0, 0, 0,
    0, (float)(1 / tan(45 * (3.1415926535 / 180))), 0, 0,
    0, 0, -1, -1,
    0, 0, -1, 0
);

glm::mat4x4 eye = glm::mat4x4(
            1,0,0,0,
            0,1,0,0,
            0,0,1,0,
            0,0,0,1
      );
glm::mat4x4 matrix;
glm::mat4x4 scene_projection_matrix;    ///< Scene projection matrix
glm::mat4x4 view_matrix;                ///< View matrix
glm::mat4x4 matrix_result;              ///< Temp matrix for calculations
std::stack<glm::mat4x4> matrixBuffer;   ///< Matrix stack

/**
 * @brief glphong constructor
 */
glphong::glphong() {

    for (int i = 0; i < 10; i++) {
        enable[i] = true;
    }

    for (int i = 0; i < 4095; i++) {
        dynindices[i] = i;
    }

    /// set open-gl
    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
    glDepthFunc(GL_LESS);
    glDisable(GL_CULL_FACE);

    //set atrib locations
    gui_shader = getShader("gui");
    scene_shader = getShader("scene");

    if (screen_width * screen_height > 0) {

        //find ideal texture resolution
        res = 2;
        while (res < screen_width * antialiasing) {
            res *= 2;
        }

        //create render texture
        for (int i = 0; i < 2; i++) {
            rtt[i] = *(new RTT());
            glGenTextures(1, &rtt[i].rendertexture);
            glBindTexture(GL_TEXTURE_2D, rtt[i].rendertexture);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, res, res, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
            glGenerateMipmap(GL_TEXTURE_2D);

            //create frame buffer
            glGenFramebuffers(1, &rtt[i].fboID);
            glBindFramebuffer(GL_FRAMEBUFFER, rtt[i].fboID);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            //create render buffer
            glGenRenderbuffers(1, &rtt[i].rboID);
            glBindRenderbuffer(GL_RENDERBUFFER, rtt[i].rboID);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, res, res);
            glBindRenderbuffer(GL_RENDERBUFFER, 0);

            //clear it
            glBindRenderbuffer(GL_RENDERBUFFER, rtt[i].rboID);
            glBindFramebuffer(GL_FRAMEBUFFER, rtt[i].fboID);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, rtt[i].rendertexture, 0);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rtt[i].rboID);
            glViewport (0, 0, res, res);
            clear(true);
        }

        //set viewport
        glViewport (0, 0, (GLsizei) screen_width, (GLsizei) screen_height);
        clear(true);
        glActiveTexture( GL_TEXTURE0 );
    }
}

/**
 * @brief lookAt implements GLUlookAt
 * @param eyex is eye vector coordinate
 * @param eyey is eye vector coordinate
 * @param eyez is eye vector coordinate
 * @param centerx is camera center coordinate
 * @param centery is camera center coordinate
 * @param centerz is camera center coordinate
 * @param upx is up vector coordinate
 * @param upy is up vector coordinate
 * @param upz is up vector coordinate
 */
void glphong::lookAt(GLfloat eyex, GLfloat eyey, GLfloat eyez,
                     GLfloat centerx, GLfloat centery, GLfloat centerz,
                     GLfloat upx, GLfloat upy, GLfloat upz) {

    camX = eyex;
    camZ = eyez;
    view_matrix = glm::lookAt(glm::vec3(eyex, eyey, eyez),
                              glm::vec3(centerx, centery, centerz),
                              glm::vec3(upx, upy, upz));
    matrix_result = eye;
}

/**
 * @brief perspective implements GLUPerspective
 * @param fovy is fov angle
 * @param aspect is screen aspect ration
 * @param zNear is near cutting plate
 * @param zFar is far cutting plane
 */
void glphong::perspective(GLfloat fovy, GLfloat aspect, GLfloat zNear, GLfloat zFar) {
    while (!matrixBuffer.empty()) {
        matrixBuffer.pop();
    }
    scene_projection_matrix = glm::perspective((float)(fovy * M_PI / 180.0f), aspect, zNear,zFar);
    glEnable(GL_DEPTH_TEST);
}

/**
 * @brief clear clears fragmet/depth buffer
 * @param colors true to clear both, false to clear only depth buffer
 */
void glphong::clear(bool colors) {
    if (colors) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    } else {
        glClear(GL_DEPTH_BUFFER_BIT);
    }
}

/**
 * @brief multMatrix multiplies with matrix
 * @param matrix is 4x4 matrix in OpenGL format
 */
void glphong::multMatrix(float* matrix) {
  matrix_result *= glm::mat4x4(
      matrix[0], matrix[1], matrix[2], matrix[3],
      matrix[4], matrix[5], matrix[6], matrix[7],
      matrix[8], matrix[9], matrix[10], matrix[11],
      matrix[12], matrix[13], matrix[14], matrix[15]
  );
}

/**
 * @brief popMatrix pops matrix from stack
 */
void glphong::popMatrix() {
  // popping matrix from stack
  matrix_result = (glm::mat4x4)matrixBuffer.top();
  matrixBuffer.pop();
}

/**
 * @brief pushMatrix pushs current matrix to stack
 */
void glphong::pushMatrix() {
  // push matrix m to stack
  matrixBuffer.push(matrix_result);
}

/**
 * @brief rotateX rotate around X axis
 * @param value is angle
 */
void glphong::rotateX(float value) {
  float radian = value * M_PI / 180;
  // rotation matrix for 2D transformations (around Z axis)
  glm::mat4x4 rotation(
      1,0,0,0,
      0,cosf(radian),-sinf(radian),0,
      0,sinf(radian),cosf(radian),0,
      0,0,0,1
  );
  // mult current maxtrix
  matrix_result *= rotation;
}

/**
 * @brief rotateX rotate around Y axis
 * @param value is angle
 */
void glphong::rotateY(float value) {
  float radian = value * M_PI / 180;
  // rotation matrix for 2D transformations (around Z axis)
  glm::mat4x4 rotation(
      cosf(radian),0,-sinf(radian),0,
      0,1,0,0,
      sinf(radian),0,cosf(radian),0,
      0,0,0,1
  );
  // mult current maxtrix
  matrix_result *= rotation;
}

/**
 * @brief rotateX rotate around Z axis
 * @param value is angle
 */
void glphong::rotateZ(float value) {
  float radian = value * M_PI / 180;
  // rotation matrix for 2D transformations (around Z axis)
  glm::mat4x4 rotation(
      cosf(radian),sinf(radian),0,0,
      -sinf(radian),cosf(radian),0,0,
      0,0,1,0,
      0,0,0,1
  );
  // mult current maxtrix
  matrix_result *= rotation;
}

/**
 * @brief scale scales current matrix
 * @param value is amount of scale(1 to keep current)
 */
void glphong::scale(float value) {
  // scale matrix
  glm::mat4x4 scale(
      value,0,0,0,
      0,value,0,0,
      0,0,value,0,
      0,0,0,1
  );
  // mult current maxtrix
  matrix_result *= scale;
}

/**
 * @brief translate translates object
 * @param x is translate coordinate
 * @param y is translate coordinate
 * @param z is translate coordinate
 */
void glphong::translate(float x, float y, float z) {
  //transformation matrix
  glm::mat4x4 translation(
      1,0,0,0,
      0,1,0,0,
      0,0,1,0,
      x,y,z,1
  );
  // mult current matrix
  matrix_result *= translation;
}

/**
 * @brief renderButton renders button in GUI mode
 * @param x is position x
 * @param y is position y
 * @param w is width
 * @param h is height
 * @param layer is distance from camera
 * @param button is button texture
 * @param text is button text
 */
void glphong::renderButton(float x, float y, float w, float h, float layer, texture* button, const char* text) {
    renderImage(x, y, w, h, layer, button);
    renderText(x, y, layer, text);
}

void glphong::renderDynamic(GLfloat *vertices, GLfloat *coords, shader* sh, texture* txt, int triangleCount) {
    glEnable(GL_BLEND);
    glDepthMask(false);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE);
    txt->apply();
    sh->bind();
    matrix = scene_projection_matrix * view_matrix;
    sh->uniformMatrix("u_Matrix",glm::value_ptr(matrix));
    sh->attrib(vertices, coords);
    glDrawElements(GL_TRIANGLES, triangleCount * 3, GL_UNSIGNED_SHORT, dynindices);
    sh->unbind();
    glDisable(GL_BLEND);
    glDepthMask(true);
}

/**
 * @brief renderImage renders image in GUI mode
 * @param x is position x
 * @param y is position y
 * @param w is width
 * @param h is height
 * @param layer is distance from camera
 * @param image is image texture
 */
void glphong::renderImage(float x, float y, float w, float h, float layer, texture* image) {
    /// indicies
    GLubyte indices[] = {3,0,1,3,1,2};

    /// vertices
    GLfloat vertices[] = {
        -w / 100, -h / 100, 0,
        +w / 100, -h / 100, 0,
        +w / 100, +h / 100, 0,
        -w / 100, +h / 100, 0
    };

    GLfloat coords[] = {0,0,1,0,1,1,0,1};

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    image->apply();

    /// render    
    glm::mat4x4 gui_modelview_matrix(
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        w / 100 - 1 + x / 50, -h / 100 + 1 - y / 50, -layer, 1
    );
    // mult current maxtrix
    matrix_result = gui_projection_matrix * gui_modelview_matrix;

    gui_shader->bind();
    gui_shader->uniformMatrix("u_Matrix", glm::value_ptr(matrix_result));
    gui_shader->attrib(vertices, coords);
    glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(GLubyte), GL_UNSIGNED_BYTE, indices);
    glDisable(GL_BLEND);
    gui_shader->unbind();
}

/**
 * @brief renderModel renders model into scene
 * @param m is instance of model to render
 * @param physic is physical model instance
 * @param gamma is requested render gamma
 */
void glphong::renderModel(model* m) {
    glDisable(GL_BLEND);
    glActiveTexture(GL_TEXTURE0);
    /// set opengl for rendering models
    for (unsigned int i = 0; i < m->models.size(); i++) {
        if (enable[m->models[i].filter]) {
            renderSubModel(m, m->models[i]);
        }
    }
}

/**
 * @brief renderSubModel renders model into scene
 * @param m is instance of model to render
 * @param physic is physical model instance
 * @param gamma is requested render gamma
 */
void glphong::renderSubModel(model* mod, model3d m) {

    glm::mat4x4 modelView;
    if (m.dynamic) {
        physic->getTransform(m.dynamicID, mat);
        m.x = mat[12];
        m.y = mat[13];
        m.z = mat[14];
        float w = m.reg->maxX - m.reg->minX;
        float a = m.reg->maxY - m.reg->minY;
        float h = m.reg->maxZ - m.reg->minZ;
        glm::mat4x4 translation(
            1,0,0,0,
            0,1,0,0,
            0,0,1,0,
            -w/2, -a/2, -h/2, 1
        );
        glm::mat4x4 dynamic(
            mat[0],mat[1],mat[2],mat[3],
            mat[4],mat[5],mat[6],mat[7],
            mat[8],mat[9],mat[10],mat[11],
            mat[12],mat[13],mat[14],mat[15]
        );
        modelView = view_matrix * dynamic * translation;
    } else {
        glm::mat4x4 translation(
            1,0,0,0,
            0,1,0,0,
            0,0,1,0,
            m.reg->minX, m.reg->minY, m.reg->minZ,1
        );
        modelView = view_matrix * matrix_result * translation;
    }

    /// render
    m.texture2D->apply();
    if (m.texture2D->transparent) {
        glDisable(GL_CULL_FACE);
    } else {
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
    }
    glBindBuffer(GL_ARRAY_BUFFER, m.vbo);
    shader* current = m.material;
    current->bind();
    current->uniformMatrix("u_ModelViewMatrix",glm::value_ptr(modelView));
    current->uniformMatrix("u_ProjectionMatrix",glm::value_ptr(scene_projection_matrix));
    matrix = scene_projection_matrix * modelView;
    current->uniformMatrix("u_Matrix",glm::value_ptr(matrix));
    glActiveTexture( GL_TEXTURE1 );
    glBindTexture(GL_TEXTURE_2D, rtt[oddFrame].rendertexture);
    glActiveTexture( GL_TEXTURE0 );
    current->uniformInt("color_texture", 0);
    current->uniformInt("EnvMap1", 1);
    current->uniformFloat("u_Time", frame / 1000.0f);
    current->uniformFloat("u_res", 1 / (float)res);
    current->uniformFloat("u_width", 1 / (float)screen_width);
    current->uniformFloat("u_height", 1 / (float)screen_height);
    current->uniformFloat("u_test", testUniform);
    current->uniformFloat("u_view", (screen_height * antialiasing) / (float)res);
    current->uniformFloat4("u_kA", m.colora[0], m.colora[1], m.colora[2], 1);
    current->uniformFloat4("u_kD", m.colord[0], m.colord[1], m.colord[2], 1);
    current->uniformFloat4("u_kS", m.colors[0], m.colors[1], m.colors[2], 1);
    current->uniformFloat("u_speed", allCar[cameraCar]->speed / 400.0f + 0.1f);
    /*if ((m.texture2D->alpha < 0.9) || (m.texture2D->transparent)) {
        glEnable(GL_BLEND);
        glDepthMask(false);
        glBlendFunc(GL_SRC_ALPHA,GL_ONE);
        current->uniformFloat("u_Alpha", m.texture2D->alpha);
    } else {
        glDisable(GL_BLEND);
        current->uniformFloat("u_Alpha", 1);
    }*/

    int xm = (camX - mod->minx) / culling - 1;
    int xp = xm + 2;
    int ym = (camZ - mod->minz) / culling - 1;
    int yp = ym + 2;
    if (xm < 0)
        xm = 0;
    if (ym < 0)
        ym = 0;
    if (xp >= mod->cutX)
        xp = mod->cutX - 1;
    if (yp >= mod->cutY)
        yp = mod->cutY - 1;

    current->attrib(sizeof(GLfloat) * m.triangleCount[mod->cutX * mod->cutY] * 3);
    if (mod->cutX * mod->cutY == 1) {
        glDrawArrays(GL_TRIANGLES, 0, m.triangleCount[1] * 3);
    } else {
        for (int i = ym; i <= yp; i++) {
            int l = m.triangleCount[i * mod->cutX + xm] * 3;
            int r = m.triangleCount[i * mod->cutX + xp + 1] * 3;
            glDrawArrays(GL_TRIANGLES, l, r - l);
        }
        int l = m.triangleCount[(mod->cutX - 1) * mod->cutY] * 3;
        int r = m.triangleCount[mod->cutX * mod->cutY] * 3;
        glDrawArrays(GL_TRIANGLES, l, r - l);
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    //unbind shader
    glDepthMask(true);
    glUseProgram(0);
    glDisable(GL_CULL_FACE);
}

/**
 * @brief renderText renders text in GUI mode
 * @param x is position x
 * @param y is position y
 * @param layer is distance from camera
 * @param text is button text
 */
void glphong::renderText(float x, float y, float layer, const char* text) {
    int minus = 0;
    if (glslfont == 0) {
        glslfont = getTexture(fontTexture, 1);
    }
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    for (unsigned int i = 0; i < strlen(text); i++) {
        /// indicies
        GLubyte indices[] = {3,0,1,3,1,2};

        /// vertices
        GLfloat vertices[] = {
            x / 50 - 1, 1 - y / 50 - 1, 0,
            x / 50, 1 - y / 50 - 1, 0,
            x / 50, 1 - y / 50, 0,
            x / 50 - 1, 1 - y / 50, 0
        };

        if (text[i] == '\\') {
            minus = i + 1;
            y += 5;
        } else {
            int cx = text[i] % (int)fontRows;
            int cy = text[i] / (int)fontRows;
            float size = 1 / fontRows;
            GLfloat coords[] = {cx * size, 1 - cy * size - size,
                                cx * size + size, 1 - cy * size - size,
                                cx * size + size, 1 - cy * size,
                                cx * size, 1 - cy * size};

            glslfont->apply();

            glm::mat4x4 gui_modelview_matrix(
                fontSize / 1.1, 0, 0, 0,
                0, fontSize / 1.1, 0, 0,
                0, 0, 1, 0,
                (x / 1.1 + (i - minus) * fontSpaces / 1.1 + fontMoveX + 5) / 50 - 1,
                        1 - (y / 1.1 + fontMoveY + 2) / 50, -layer, 1
            );
            matrix_result = gui_projection_matrix * gui_modelview_matrix;

            /// render
            gui_shader->bind();
            gui_shader->uniformMatrix("u_Matrix", glm::value_ptr(matrix_result));
            gui_shader->attrib(vertices, coords);
            glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(GLubyte), GL_UNSIGNED_BYTE, indices);
        }
    }
    glDisable(GL_BLEND);
    gui_shader->unbind();
}

/**
 * @brief setMode sets rendering mode
 * @param mode is mode index
 */
void glphong::setMode(int mode) {

    if (mode == 0) {
        glBindRenderbuffer(GL_RENDERBUFFER, rtt[oddFrame].rboID);
        glBindFramebuffer(GL_FRAMEBUFFER, rtt[oddFrame].fboID);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, rtt[oddFrame].rendertexture, 0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rtt[oddFrame].rboID);
        glViewport (0, 0, screen_width * antialiasing, screen_height * antialiasing);
        clear(true);
        glEnable(GL_DEPTH_TEST);
        oddFrame = !oddFrame;
        frame++;
        if (frame > 1000) {
            frame = 0;
        }
    }
    if (mode == 1) {
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport (0, 0, (GLsizei) screen_width, (GLsizei) screen_height);
        scene_shader->bind();

        /// indicies
        GLubyte indices[] = {3,0,1,3,1,2};

        /// vertices
        GLfloat vertices[] = {
            -1, -1, 0,
            +1, -1, 0,
            +1, +1, 0,
            -1, +1, 0
        };

        GLfloat coords[] = {0, 0,
                            (screen_width * antialiasing) / (float)res, 0,
                            (screen_width * antialiasing) / (float)res, (screen_height * antialiasing) / (float)res,
                            0, (screen_height * antialiasing) / (float)res
        };

        glBindTexture(GL_TEXTURE_2D, rtt[oddFrame].rendertexture);
        scene_shader->attrib(vertices, coords);
        scene_shader->uniformFloat("u_res", 1 / (float)res);

        glDisable(GL_DEPTH_TEST);
        glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(GLubyte), GL_UNSIGNED_BYTE, indices);
        scene_shader->unbind();
    }
}
