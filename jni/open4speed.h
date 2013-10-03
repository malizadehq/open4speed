//--------------------------    --------------------------------------------------------------
/**
 * \file       open4speed.h
 * \author     Vonasek Lubos
 * \date       2012/04/25
 * \brief      Runable part of project.
*/
//----------------------------------------------------------------------------------------

#include "jni.h"

/**
 * @brief display updates display
 */
void display(void);

/**
 * @brief keyboard is called when key pressed
 * @param key is key code
 * @param x is cursor position x
 * @param y is cursor position y
 */
void keyboardDown(unsigned char key, int x, int y);

/**
 * @brief keyboard is called when key released
 * @param key is key code
 * @param x is cursor position x
 * @param y is cursor position y
 */
void keyboardUp(unsigned char key, int x, int y);

/**
 * @brief mouseClick is called on mouse click
 * @param button is index of pressed button
 * @param state is state of pressed button
 * @param x is mouse position x
 * @param y is mouse position y
 */
void mouseClick(int button, int state, int x, int y);

/**
 * @brief mouseMove is called when mouse moves
 * @param x is mouse position x
 * @param y is mouse position y
 */
void mouseMove(int x, int y);

/**
 * @brief idle is called automatically by display manager
 * @param v is time information
 */
void idle(int v);

/**
 * @brief reshape rescales window
 * @param w is new window width
 * @param h is new window hegiht
 */
void reshape (int w, int h);

/**
 * @brief main loads data and prepares scene
 * @param argc is amount of arguments
 * @param argv is array of arguments
 * @return exit code
 */
int main(int argc, char** argv);
