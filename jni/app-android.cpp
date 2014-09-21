//----------------------------------------------------------------------------------------
/**
 * \file       app-android.cpp
 * \author     Vonasek Lubos
 * \date       2013/04/20
 * \brief      Runable part of project.
*/
//----------------------------------------------------------------------------------------

#include "app.h"
#include "open4speed.h"
#include "components/input/keyboard.h"

JNIEnv* instance;            ///< JNI instance
zip* APKArchive;             ///< APK archive instance

extern "C" {

static void loadAPK (const char* apkPath) {
  APKArchive = zip_open(apkPath, 0, NULL);
  logi("Opening ", apkPath);

  //Just for debug, print APK contents
  int numFiles = zip_get_num_files(APKArchive);
  for (int i=0; i<numFiles; i++) {
    const char* name = zip_get_name(APKArchive, i, 0);
  }
}

/**
 * @brief Java_com_tridos_o4s_O4SJNI_nativeInit is init method
 * @param env is instance of JNI
 * @param am is asset manager
 */
void Java_com_tridos_o4s_O4SJNI_nativeInit( JNIEnv*  env, jclass cls, jstring apkPath ) {
  instance = env;
  const char* str;
  jboolean isCopy;
  str = env->GetStringUTFChars(apkPath, &isCopy);
  loadAPK(str);
  main(0, 0);
}

/**
 * @brief Java_com_tridos_o4s_O4SJNI_nativeResize is resize method
 * @param env is instance of JNI
 * @param thiz is asset manager
 * @param w is display width
 * @param h is display height
 */
void Java_com_tridos_o4s_O4SJNI_nativeResize( JNIEnv*  env, jobject  thiz, jint w, jint h ) {
  reshape(w, h);
}

/**
 * @brief Java_com_tridos_o4s_O4SJNI_nativeClick is click method
 * @param env is instance of JNI
 * @param thiz is asset manager
 * @param x is position x
 * @param y is position y
 */
void Java_com_tridos_o4s_O4SJNI_nativeClick( JNIEnv*  env, jobject  thiz, jint x, jint y ) {
  mouseClick(0, 0, x, y);
  mouseClick(0, 1, x, y);
}

/**
 * @brief Java_com_tridos_o4s_O4SJNI_nativeKey is key press method
 * @param env is instance of JNI
 * @param thiz is asset manager
 * @param code is key code
 */
void Java_com_tridos_o4s_O4SJNI_nativeKey( JNIEnv*  env, jobject  thiz, jint code ) {
  special(code, 0, 0);
}

/**
 * @brief Java_com_tridos_o4s_O4SJNI_nativeKeyUp is key release method
 * @param env is instance of JNI
 * @param thiz is asset manager
 * @param code is key code
 */
void Java_com_tridos_o4s_O4SJNI_nativeKeyUp( JNIEnv*  env, jobject  thiz, jint code ) {
  specialUp(code, 0, 0);
}

/**
 * @brief Java_com_tridos_o4s_O4SJNI_nativeLoop is loop method
 * @param env is instance of JNI
 * @param thiz is asset manager
 */
void Java_com_tridos_o4s_O4SJNI_nativeLoop( JNIEnv*  env, jobject  thiz ) {
  idle(0);
  display();
}

/**
 * @brief Java_com_tridos_o4s_O4SJNI_nativeBack is back key method
 * @param env is instance of JNI
 * @param thiz is asset manager
 */
void Java_com_tridos_o4s_O4SJNI_nativeBack( JNIEnv*  env, jobject  thiz ) {
  keyboardDown(27, 0, 0);
}
}
