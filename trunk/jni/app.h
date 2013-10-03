//----------------------------------------------------------------------------------------
/**
 * \file       app.h
 * \author     Vonasek Lubos
 * \date       2013/04/20
 * \brief      Runable part of project.
*/
//----------------------------------------------------------------------------------------

#include <jni.h>
#include <sys/time.h>
#include <time.h>
#include <stdint.h>
#include "zip.h"

extern JNIEnv* instance;            ///< JNI instance
extern zip* APKArchive;             ///< APK archive instance

