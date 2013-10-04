//----------------------------------------------------------------------------------------
/**
 * \file       open4speed.cpp
 * \author     Vonasek Lubos
 * \date       2013/04/20
 * \brief      Runable part of project.
*/
//----------------------------------------------------------------------------------------

#include "stdafx.h"

int lastFPS = 0;    ///< Viewed FPS
int fps = 0;        ///< Actual FPS
int timestamp = 0;  ///< Timestamp
float ttime = 0;    ///< Timer
float lastttime = 0;///< Last timestamp
GLuint id[2] = {0,1};
timeval start = *(new timeval());
timeval end = *(new timeval());
int curEdge;

/**
 * @brief display updates display
 */
void display(void) {

    /// display scene
    if (race) {

/*#ifndef ANDROID
        if (strcmp(screenRenderer, "glsl") == 0) {
            glGenQueries(2,id);
            glBeginQuery(GL_TIME_ELAPSED, id[0]);
            glBeginQuery(GL_SAMPLES_PASSED, id[1]);
        }
#endif*/

        displayScene();

/*#ifndef ANDROID
        GLint gpu_time = 0;
        GLint fragment_count = 0;
        if (strcmp(screenRenderer, "glsl") == 0) {
            glEndQuery(GL_SAMPLES_PASSED);
            glEndQuery(GL_TIME_ELAPSED);

            /// get scene counters
            glGetQueryObjectiv(id[0], GL_QUERY_RESULT, &gpu_time);
            glGetQueryObjectiv(id[1], GL_QUERY_RESULT, &fragment_count);
        }
        if (strcmp(screenRenderer, "glsl") == 0) {
            /// print counters
            char text[200];
            sprintf(text, "GPU time: %dk, fragments: %dk", gpu_time / 1000, fragment_count / 1000);
            xrenderer->renderText(0, 15, 1, text);
        }

        int i = glGetError();
        if (i != 0) {
            printf("GL_ERROR %d\n", i);
        }
#endif*/

        /// update FPS
        fps++;
        if (timestamp != time(0)) {
            lastFPS = fps;
            timestamp = time(0);
            fps = 0;
            lastttime = ttime;
            ttime = 0;
            if (active) {
                if (!physic->locked) {
                    timeout--;
                    if (timeout == 0) {
                        std::vector<char*> list = getList(timeoutAction);
                        syntaxList.assign(list.begin(), list.end());
                    }
                }
            }
        }
    }
    displayMenu();

    /// draw FPS
    if (debug && race) {
        char text[100];
        sprintf(text, "FPS: %d, CPU: %d%s, edge: %d, key: %d, unif.: %d", lastFPS, (int)lastttime / 10, "%", allCar[cameraCar]->currentEdgeIndex, lastkey, testUniform);
        xrenderer->renderText(0, 0, 1, text);
    }

#ifdef ANDROID
    glFinish();
#else
    glutSwapBuffers();
#endif
}

/**
 * @brief keyboard is called when key pressed
 * @param key is key code
 * @param x is cursor position x
 * @param y is cursor position y
 */
void keyboardDown(unsigned char key, int x, int y) {
    /// disable upper case
    if ((key >= 'A') & (key <= 'Z'))
        key = key - 'A' + 'a';
    /// resend key code
    special((int)key + 128, x, y);

    /// syntax called by escape key
    if ((syntaxList.size() == 0) & (key + 128 == (int)keyBack)) {
        std::vector<char*> list = getList(esc);
        syntaxList.assign(list.begin(), list.end());
    }
    /*if (key == 'r')
        allCar[cameraCar]->onRoof = 100;
    if (key == 'a') {
        curEdge++;
        printf("%d\n", curEdge);
        allCar[cameraCar]->currentEdge = allCar[cameraCar]->edges[curEdge];
        physic->resetCar(allCar[cameraCar]);
    }
    if (key == 's')
        testUniform--;
    if (key == 'd')
        testUniform++;
    if (key == 'q')
        cameraCar++;
    if (key == 'w')
        cameraCar--;*/
    if (cameraCar == -1)
        cameraCar = carCount - 1;
    if (cameraCar == carCount)
        cameraCar = 0;
}

/**
 * @brief keyboard is called when key released
 * @param key is key code
 * @param x is cursor position x
 * @param y is cursor position y
 */
void keyboardUp(unsigned char key, int x, int y) {
    /// disable upper case
    if ((key >= 'A') & (key <= 'Z'))
        key = key - 'A' + 'a';
    /// resend key code
    specialUp((int)key + 128, x, y);
}

/**
 * @brief mouseClick is called on mouse click
 * @param button is index of pressed button
 * @param state is state of pressed button
 * @param x is mouse position x
 * @param y is mouse position y
 */
void mouseClick(int button, int state, int x, int y) {
    /// cancel if script is in progress
    if (syntaxList.size() > 0)
        return;

    /// set mouse button state
    if (button==0 && state==0)
        click = true;

    /// action on mouse button release
    if (button==0 && state==1 && click) {
        click = false;

        /// count position on screen
        x = x * 100 / (float)screen_width;
        y = y * 100 / (float)screen_height;

        /// check all buttons
        for (unsigned int i = 0; i < buttons.size(); i++)

            /// check if button is clicked
            if ((x > buttons[i].x) & (y > buttons[i].y) & (x < buttons[i].x + buttons[i].width)
                & (y < buttons[i].y + buttons[i].height)) {
                    exec = i;
                    for (unsigned int j = 0; j < syntaxList.size(); j++) {
                        delete syntaxList[j];
                    }
                    syntaxList = getList(buttons[i].syntax);
                }
    }
}

/**
 * @brief mouseMove is called when mouse moves
 * @param x is mouse position x
 * @param y is mouse position y
 */
void mouseMove(int x, int y) {
    mouseX = x;
    mouseY = y;
}

/**
 * @brief idle is called automatically by display manager
 * @param v is time information
 */
void idle(int v) {

    if ((!busy) & (unlock < time(NULL))) {
        while (syntaxList.size() > 0) {
            char syntax[16];
            sscanf(syntaxList[0], "%s", &syntax[0]);
            logi("exec:", syntaxList[0]);
            if (strcmp("active", syntax) == 0) {
                int value;
                sscanf(syntaxList[0], "%s %d", &syntax[0], &value);
                if (value == 1)
                    active = true;
                else
                    active = false;
            }

            if (strcmp("addButton", syntax) == 0) {
                button b = *(new button());
                b.syntax[0] = '\0';
                b.layer = 0.96;
                char value[16];
                sscanf(syntaxList[0], "%s img %d %s %d %fx%f-%fx%f %s", &syntax[0], &b.image, &value[0], &b.text, &b.x, &b.y,
                       &b.width, &b.height, &b.syntax[0]);
                b.type = -1;
                if (strcmp("text", value) == 0)
                    b.type = 0;
                if (strcmp("cfg", value) == 0)
                    b.type = 1;
                if (strcmp("mon", value) == 0)
                    b.type = 2;
                buttons.push_back(b);
            }

            if (strcmp("addImage", syntax) == 0) {
                char value[64];
                sscanf(syntaxList[0], "%s %s", &syntax[0], &value[0]);
                texture* image = getTexture(value, 1);
                images.push_back(image);
            }

            if (strcmp("clearButtons", syntax) == 0) {
                buttons.clear();
            }

            if (strcmp("debug", syntax) == 0) {
                int value;
                sscanf(syntaxList[0], "%s %d", &syntax[0], &value);
                if (value == 1)
                    debug = true;
                else
                    debug = false;
            }

            if (strcmp("detail", syntax) == 0) {
                if (variable == 0) {
                    antialiasing = 0.6f;
                } else if (variable == 1) {
                    antialiasing = 0.7f;
                } else if (variable == 2) {
                    antialiasing = 0.8f;
                } else if (variable == 3) {
                    antialiasing = 0.9f;
                } else if (variable == 4) {
                    antialiasing = 1.0f;
                }
                xrenderer = getRenderer();
            }

            if (strcmp("esc", syntax) == 0) {
                sscanf(syntaxList[0], "%s %s", &syntax[0], &esc[0]);
            }

            if (strcmp("finish", syntax) == 0) {
                sscanf(syntaxList[0], "%s %s", &syntax[0], &finish[0]);
            }

            if (strcmp("forceRender", syntax) == 0) {
                /// cancel executing syntaxes
                delete[] syntaxList[0];
                syntaxList.erase(syntaxList.begin());
                break;
            }

            if (strcmp("fxFlyIn", syntax) == 0) {
                for (unsigned int i = 0; i < buttons.size(); i++)
                    buttons[i].layer = -0.2 * i;
            }

            if (strcmp("fxFlyOut", syntax) == 0) {
                /// set buttons layer for animation
                for (unsigned int j = 0; j < buttons.size(); j++)
                    buttons[(exec + j) % buttons.size()].layer = 1 + 0.2 * j;
                /// cancel executing syntaxes
                busy = true;
                delete[] syntaxList[0];
                syntaxList.erase(syntaxList.begin());
                break;
            }

            if (strcmp("getCfg", syntax) == 0) {
                int value;
                sscanf(syntaxList[0], "%s %d", &syntax[0], &value);
                variable = config[value];
            }

            if (strcmp("ifBgr", syntax) == 0) {
                int value;
                char newsyntax[16];
                sscanf(syntaxList[0], "%s %d %s", &syntax[0], &value, &newsyntax[0]);
                if (variable > value) {
                    syntaxList[0][0] = '\0';
                    sprintf(syntaxList[0], "include %s", &newsyntax[0]);
                    syntax[0] = '\0';
                    strcpy(syntax, "include");
                }
            }

            if (strcmp("ifEql", syntax) == 0) {
                int value;
                char newsyntax[16];
                sscanf(syntaxList[0], "%s %d %s", &syntax[0], &value, &newsyntax[0]);
                if (variable == value) {
                    syntaxList[0][0] = '\0';
                    sprintf(syntaxList[0], "include %s", &newsyntax[0]);
                    syntax[0] = '\0';
                    strcpy(syntax, "include");
                }
            }

            if (strcmp("ifSml", syntax) == 0) {
                int value;
                char newsyntax[16];
                sscanf(syntaxList[0], "%s %d %s", &syntax[0], &value, &newsyntax[0]);
                if (variable < value) {
                    syntaxList[0][0] = '\0';
                    sprintf(syntaxList[0], "include %s", &newsyntax[0]);
                    syntax[0] = '\0';
                    strcpy(syntax, "include");
                }
            }

            if (strcmp("include", syntax) == 0) {
                char value[32];
                sscanf(syntaxList[0], "%s %s", &syntax[0], &value[0]);
                delete[] syntaxList[0];
                syntaxList.erase(syntaxList.begin());
                std::vector<char*> newList = getList(value);
                for (unsigned int i = 0; i < syntaxList.size(); i++) {
                    newList.push_back(syntaxList[i]);
                }
                syntaxList.clear();
                syntaxList = newList;
                break;
            }

            if (strcmp("initSound", syntax) == 0) {
                crash = getSound(soundCrash, false, 8);
                engine = getSound(soundEngine, true, 8);
                noise = getSound("sfx/n2o.ogg", true, 8);
            }

            if (strcmp("loadCar", syntax) == 0) {
                float rotation = 0;
                if (allCar[0] != 0) {
                    rotation = allCar[0]->rot;
                }
                for (int i = 0; i < carCount; i++) {
                    delete allCar[i];
                }
                carCount = 0;
                playerCar = variable;
                allCar[0] = new car(0, (std::vector<edge>)0, carList[variable], false);
                allCar[0]->rot = rotation;
            }

            if (strcmp("loadScene", syntax) == 0) {
                delete physic;
                currentTrack = variable;
#ifdef ZIP_ARCHIVE
                std::vector<char*> atributes = getFullList(zip_fopen(APKArchive, prefix(trackList[currentTrack]), 0));
#else
                std::vector<char*> atributes = getFullList(fopen(prefix(trackList[currentTrack]), "r"));
#endif
                loadScene(atributes);
                curEdge = getConfig("player_start", atributes);

                /// create instance of physical engine
                physic = getPhysics(trackdata);
                if (trackdata2 != 0) {
                    physic->addModel(trackdata2);
                }
                for (int i = 0; i <= opponentCount; i++)
                    physic->addCar(allCar[i]);
            }

            if (strcmp("locked", syntax) == 0) {
                int value;
                sscanf(syntaxList[0], "%s %d", &syntax[0], &value);
                if (value == 1)
                    physic->locked = true;
                else
                    physic->locked = false;
            }

            if (strcmp("minus", syntax) == 0) {
                int value;
                sscanf(syntaxList[0], "%s %d", &syntax[0], &value);
                variable -= value;
            }

            if (strcmp("minusCfg", syntax) == 0) {
                int value;
                sscanf(syntaxList[0], "%s %d", &syntax[0], &value);
                variable -= config[value];
            }

            if (strcmp("minusListSize", syntax) == 0) {
                char value[32];
                sscanf(syntaxList[0], "%s %s", &syntax[0], &value[0]);
                variable -= getList(value).size();
            }

            /*if (strcmp("mouse", syntax) == 0) {
               int value;
               sscanf(syntaxList[0], "%s %d", &syntax[0], &value);
               if (value == 1)
                   glutSetCursor(GLUT_CURSOR_INHERIT);
               else
                   glutSetCursor(GLUT_CURSOR_NONE);
           }*/

            if (strcmp("musicPlay", syntax) == 0) {
                music = getSound(musicList[variable], true, 1);
                music->play(0);
            }

            if (strcmp("musicSetVolume", syntax) == 0) {
                music->setVolume(0, variable / 10.0);
            }

            if (strcmp("musicStop", syntax) == 0) {
                music->stop(0);
            }

            if (strcmp("muteSound", syntax) == 0) {
                for (int i = 0; i < carCount; i++) {
                    engine->stop(i);
                    noise->stop(i);
                }
            }

            if (strcmp("nav", syntax) == 0) {
                int value;
                sscanf(syntaxList[0], "%s %d", &syntax[0], &value);
                allCar[cameraCar]->currentEdge = allCar[cameraCar]->edges[value];
                allCar[cameraCar]->currentGoalEdge = allCar[cameraCar]->edges[value];
                allCar[cameraCar]->finishEdge = value;
                allCar[cameraCar]->lapsToGo = -1;
            }

            if (strcmp("plus", syntax) == 0) {
                int value;
                sscanf(syntaxList[0], "%s %d", &syntax[0], &value);
                variable += value;
            }

            if (strcmp("physicalEngine", syntax) == 0) {
                sscanf(syntaxList[0], "%s %s", &syntax[0], &physicalEngine[0]);
            }

            if (strcmp("race", syntax) == 0) {
                int value;
                sscanf(syntaxList[0], "%s %d", &syntax[0], &value);
                if (value == 1)
                    race = true;
                else
                    race = false;
            }

            if (strcmp("reset", syntax) == 0) {
                if (allCar[cameraCar]->resetAllowed) {
                    if ((allCar[cameraCar]->speed < 5) && active && !physic->locked) {
                        allCar[cameraCar]->resetRequested = true;
                    }
                }
            }

            if (strcmp("rnd", syntax) == 0) {
                variable = time(NULL) % variable;
            }

            if (strcmp("quit", syntax) == 0) {
                exit(0);
            }

            if (strcmp("saveCfg", syntax) == 0) {
                FILE* file = fopen(prefixEx("config"), "w");
                for (int i = 0; i < configSize; i++) {
                    fprintf(file,"%d\n", config[i]);
                }
                fclose(file);
            }

            if (strcmp("set", syntax) == 0) {
                sscanf(syntaxList[0], "%s %d", &syntax[0], &variable);
            }

            if (strcmp("setCfg", syntax) == 0) {
                int value;
                sscanf(syntaxList[0], "%s %d", &syntax[0], &value);
                config[value] = variable;
            }

            if (strcmp("setBackground", syntax) == 0) {
                sscanf(syntaxList[0], "%s %d", &syntax[0], &background);
            }

            if (strcmp("setVolume", syntax) == 0) {
                soundVolume = variable;
            }

            if (strcmp("sleep", syntax) == 0) {
                int value;
                sscanf(syntaxList[0], "%s %d", &syntax[0], &value);
                usleep(value * 1000);
                delete[] syntaxList[0];
                syntaxList.erase(syntaxList.begin());
                break;
            }

            if (strcmp("sub", syntax) == 0) {
                float value;
                sscanf(syntaxList[0], "%s %f", &syntax[0], &value);
                variable = (int)(variable * value);
            }

            if (strcmp("timeout", syntax) == 0) {
                sscanf(syntaxList[0], "%s %d %s", &syntax[0], &timeout, &timeoutAction[0]);
            }

            if (strcmp("transmission", syntax) == 0) {
                if (variable == 1)
                    transmission = true;
                else
                    transmission = false;
            }

            if (strcmp("viewCar", syntax) == 0) {
                int value;
                sscanf(syntaxList[0], "%s %d", &syntax[0], &value);
                if (value == 1)
                    viewCar = true;
                else
                    viewCar = false;
            }

            if (strcmp("wait", syntax) == 0) {
                int value;
                sscanf(syntaxList[0], "%s %d", &syntax[0], &value);
                unlock = time(NULL) + value;
                delete[] syntaxList[0];
                syntaxList.erase(syntaxList.begin());
                break;
            }

            delete[] syntaxList[0];
            syntaxList.erase(syntaxList.begin());
        }
    }

    /// update car during race
    if (active) {

        /// if race finished show result
        if (allCar[0]->lapsToGo == -1) {
            variable = 0;
            for (int j = 0; j < carCount; j++) {
                if (allCar[cameraCar]->toFinish >= allCar[j]->toFinish) {
                    variable++;
                }
            }
            for (unsigned int j = 0; j < syntaxList.size(); j++) {
                delete[] syntaxList[j];
            }
            syntaxList = getList(finish);
        }
        if (allCar[0]->lapsToGo == -1) {
            if (distance(allCar[cameraCar]->x, allCar[cameraCar]->z, allCar[cameraCar]->currentGoalEdge.bx, allCar[cameraCar]->currentGoalEdge.bz) < 25) {
                syntaxList = getList(finish);
            }
        }

        for (int i = 0; i < carCount; i++) {

            /// update current edge for navigation
            if (!physic->locked) {
                if ((distance(allCar[i]->currentEdge, allCar[i]) < allCar[i]->control->getUpdate()) & (absf(allCar[i]->currentEdge.by - allCar[i]->y) < 30)) {
                    std::vector<int> nEdges = nextEdge(allCar[i]->edges, allCar[i]->currentEdgeOriginal);
                    if (nEdges.size() > 0) {
                        allCar[i]->currentEdgeOriginal = allCar[i]->edges[nEdges[0]];
                        allCar[i]->currentEdge = sidemoveEdge(allCar[i]->currentEdgeOriginal, allCar[i]->edgeSideMove);
                    }
                }
            }
            allCar[i]->update();
        }

        gettimeofday(&start, NULL);
        for (int j = 0; j < physic->getMultiply(); j++) {
            for (int i = 0; i < carCount; i++)
                physic->updateCar(allCar[i]);

            /// update scene
            physic->updateWorld();
        }
        gettimeofday(&end, NULL);
		float seconds  = end.tv_sec  - start.tv_sec;
        float useconds = end.tv_usec - start.tv_usec;

        ttime += ((seconds) * 1000 + useconds/1000.0) + 0.5;
    }

    /// render menu
    updateMenu();

    /// call update
#ifndef ANDROID
    glutPostRedisplay();
    glutTimerFunc(40,idle,0);
#endif
}

/**
 * @brief reshape rescales window
 * @param w is new window width
 * @param h is new window hegiht
 */
void reshape (int w, int h) {
   aspect = (GLfloat) w/(GLfloat) h;
   screen_width = w;
   screen_height = h;
   xrenderer = getRenderer();
}

/**
 * @brief main loads data and prepares scene
 * @param argc is amount of arguments
 * @param argv is array of arguments
 * @return exit code
 */
int main(int argc, char** argv) {
#ifndef ANDROID
    APKArchive = zip_open("../out/production/open4speedHD/open4speedHD.apk", 0, NULL);
#endif

    /// load game config
    if (exists(prefixEx("config"))) {
        FILE* file = fopen(prefixEx("config"), "r");
        for (int i = 0; i < configSize; i++) {
            fscanf(file,"%d\n", &config[i]);
        }
        fclose(file);
    } else {
        for (int i = 0; i < configSize; i++) {
            config[i] = 0;
        }
    }
    for (int i = 0; i < configSize; i++) {
        configText[i] = getList(getTag(i, "TEXTCFG%d"));
    }

    /// set screen
    background = -1;
    busy = false;
    click = false;
    debug = false;
    race = false;
    unlock = 0;

    std::vector<char*> atributes = getList("CONFIG");
    aiCameraDistance = getConfig("aiCameraDistance", atributes);
    aiRacerFutureStep = getConfig("aiRacerFutureStep", atributes);
    aiRacerFutureStepCount = getConfig("aiRacerFutureStepCount", atributes);
    aiRacerReverseDelay = getConfig("aiRacerReverseDelay", atributes);
    aiRacerReverseDuration = getConfig("aiRacerReverseDuration", atributes);
    aiRacerSafeAngle = getConfig("aiRacerSafeAngle", atributes);
    aiRacerSafeDistance = getConfig("aiRacerSafeDistance", atributes);
    aiRacerSafeDistanceSpeedDependency = getConfig("aiRacerSafeDistanceSpeedDependency", atributes);
    aiRacerSafeGapGas = getConfig("aiRacerSafeGapGas", atributes);
    aiRacerSafeGapSteering = getConfig("aiRacerSafeGapSteering", atributes);
    aiRacerSafeSpeed = getConfig("aiRacerSafeSpeed", atributes);
    aiRacerSteeringGapLevel1 = getConfig("aiRacerSteeringGapLevel1", atributes);
    aiRacerSteeringGapLevel2 = getConfig("aiRacerSteeringGapLevel2", atributes);
    aiRacerTrackUpdate = getConfig("aiRacerTrackUpdate", atributes);
    aiRacerTrackUpdateSpeedDependency = getConfig("aiRacerTrackUpdateSpeedDependency", atributes);
    aiTrafficFutureStep = getConfig("aiTrafficFutureStep", atributes);
    aiTrafficSafeAngle = getConfig("aiTrafficSafeAngle", atributes);
    aiTrafficSafeDistance = getConfig("aiTrafficSafeDistance", atributes);
    aiTrafficSafeDistanceSpeedDependency = getConfig("aiTrafficSafeDistanceSpeedDependency", atributes);
    aiTrafficSafeGap = getConfig("aiTrafficSafeGap", atributes);
    aiTrafficSafeSpeed = getConfig("aiTrafficSafeSpeed", atributes);
    aiTrafficSpeedBrake = getConfig("aiTrafficSpeedBrake", atributes);
    aiTrafficSpeedNoGas = getConfig("aiTrafficSpeedNoGas", atributes);
    aiTrafficSteeringGapLevel1 = getConfig("aiTrafficSteeringGapLevel1", atributes);
    aiTrafficSteeringGapLevel2 = getConfig("aiTrafficSteeringGapLevel2", atributes);
    aiTrafficTrackUpdate = getConfig("aiTrafficTrackUpdate", atributes);
    fontMoveX = getConfig("fontMoveX", atributes);
    fontMoveY = getConfig("fontMoveY", atributes);
    fontRows = getConfig("fontRows", atributes);
    fontSize = getConfig("fontSize", atributes);
    fontSpaces = getConfig("fontSpaces", atributes);
    fontTexture = getConfigStr("fontTexture", atributes);
    gameMode = getConfigStr("gameMode", atributes);
    inputController = getConfigStr("inputController", atributes);
    keyBack = getConfig("keyBack", atributes);
    keyCameraDown = getConfig("keyCameraDown", atributes);
    keyCameraUp = getConfig("keyCameraUp", atributes);
    keyDown = getConfig("keyDown", atributes);
    keyGearDown = getConfig("keyGearDown", atributes);
    keyGearUp = getConfig("keyGearUp", atributes);
    keyLeft = getConfig("keyLeft", atributes);
    keyRight = getConfig("keyRight", atributes);
    keyUp = getConfig("keyUp", atributes);
    maximalPerspective = getConfig("maximalPerspective", atributes);
    minimalPerspective = getConfig("minimalPerspective", atributes);
    perspectiveSpeedDependency = getConfig("perspectiveSpeedDependency", atributes);
    perspectiveSpeedFollow = getConfig("perspectiveSpeedFollow", atributes);
    playerCameraMaximalDistance = getConfig("playerCameraMaximalDistance", atributes);
    playerCameraMinimalDistance = getConfig("playerCameraMinimalDistance", atributes);
    playerCameraStep = getConfig("playerCameraStep", atributes);
    playerDefaultCameraDistance = getConfig("playerDefaultCameraDistance", atributes);
    playerTrackUpdate = getConfig("playerTrackUpdate", atributes);
    physicalEngine = getConfigStr("physicalEngine", atributes);
    screenRenderer = getConfigStr("renderer", atributes);
    soundCrash = getConfigStr("soundCrash", atributes);
    soundCrashMinimalSpeed = getConfig("soundCrashMinimalSpeed", atributes);
    soundCrashOnSpeedChange = getConfig("soundCrashOnSpeedChange", atributes);
    soundEngine = getConfigStr("soundEngine", atributes);
    soundEngineFreqAspect = getConfig("soundEngineFreqAspect", atributes);
    soundMaximalDistance = getConfig("soundMaximalDistance", atributes);
    speedAspect = getConfig("speedAspect", atributes);
    viewDistance = getConfig("viewDistance", atributes);

    cameraDistance = playerDefaultCameraDistance;

#ifndef ANDROID
    /// init glut
    glutInit(&argc, argv);

    /// set screen mode
    if (strlen(gameMode) > 0) {
        glutGameModeString(gameMode);
        glutEnterGameMode();
    } else {
        glutInitWindowSize(640,480);
        glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB);
        glutCreateWindow("Open4speed");
        glutFullScreen();
    }

    /// set handlers
    glutReshapeFunc(reshape);
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboardDown);
    glutKeyboardUpFunc(keyboardUp);
    glutMouseFunc(mouseClick);
    glutPassiveMotionFunc(mouseMove);
#endif

    /// load menu data
    carList = getList("CARS");
    musicList = getList("MUSIC");
    syntaxList = getList("INIT");
    textList = getList("TEXTS");
    trackList = getList("TRACKS");

    /// set renderer
    xrenderer = getRenderer();

    /// start loop
#ifndef ANDROID
    glutTimerFunc(0,idle,0);
    glutMainLoop();
    return 0;
#endif
}
