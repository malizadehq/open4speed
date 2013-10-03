//----------------------------------------------------------------------------------------
/**
 * \file       engine.cpp
 * \author     Vonasek Lubos
 * \date       2013/04/20
 * \brief      Common engine functions used in program.
*/
//----------------------------------------------------------------------------------------

#include "stdafx.h"

struct Dynamic {
    float* vertices;
    float* coords;
    int count;
    int frame;
};

int currentFrame = 0;
const int effLen = 6;
Dynamic eff[effLen];
model* arrow = 0;                            ///< GPS arrow model
model* water = 0;

/**
 * @brief displayMenu displaies menu
 * @param gui is instance of menu
 * @param renderer is instance of renderer
 * @param images is storage of textures
 * @param textList is storage of strings
 */
void displayMenu() {

    /// Render background
    if (background >= 0) {
        if (!active) {
            xrenderer->setMode(0);
        }
        xrenderer->renderImage(0, 0, 100, 100, 1, images[background]);
        if (!active) {
            xrenderer->setMode(1);
        }
    }

    /// render car
    if (viewCar) {
        xrenderer->perspective(60, aspect, 0.1, 10);

        /// set camera
        xrenderer->lookAt(cos(allCar[0]->rot * 3.14 / 180.0),2 + sin(allCar[0]->rot * 3.14 / 180.0),-3.5,0,0,0,0,1,0);

        /// render car
        xrenderer->enable[1] = false;
        xrenderer->clear(false);
        xrenderer->pushMatrix();
        xrenderer->rotateY(allCar[0]->rot);

        /// render car skin
        xrenderer->pushMatrix();
        xrenderer->translate(0,0.4 + allCar[0]->wheelY,0);
        xrenderer->renderModel(allCar[0]->skin);
        xrenderer->popMatrix();

        /// render rear right wheel
        xrenderer->pushMatrix();
        xrenderer->translate(-allCar[0]->wheelX,0.0,-allCar[0]->wheelZ1);
        xrenderer->renderModel(allCar[0]->wheel);
        xrenderer->popMatrix();

        /// render rear left wheel
        xrenderer->pushMatrix();
        xrenderer->translate(allCar[0]->wheelX,0.0,-allCar[0]->wheelZ1);
        xrenderer->rotateY(180);
        xrenderer->renderModel(allCar[0]->wheel);
        xrenderer->popMatrix();

        /// render front right wheel
        xrenderer->pushMatrix();
        xrenderer->translate(-allCar[0]->wheelX,0.0,allCar[0]->wheelZ2);
        xrenderer->renderModel(allCar[0]->wheel);
        xrenderer->popMatrix();

        /// render front left wheel
        xrenderer->pushMatrix();
        xrenderer->translate(allCar[0]->wheelX,0.0,allCar[0]->wheelZ2);
        xrenderer->rotateY(180);
        xrenderer->renderModel(allCar[0]->wheel);
        xrenderer->popMatrix();

        xrenderer->popMatrix();
    }

    /// Render buttons
    for (unsigned int i = 0; i < buttons.size(); i++) {

        /// Get button layer
        float layer = buttons[i].layer;
        if (layer > 1)
            layer = 1;

        /// Special text on config button
        if (buttons[i].type == 2) {
            char text[128];

            /// amount of n2o
            if (buttons[i].text == 0) {
                float w = buttons[i].width * allCar[cameraCar]->n2o / 150.0f;
                xrenderer->renderImage(buttons[i].x, buttons[i].y, w, buttons[i].height, layer, images[buttons[i].image]);
            }

            // car speed
            else if (buttons[i].text == 1) {
                sprintf(text, "%dkmh", (int)allCar[cameraCar]->speed);
                xrenderer->renderText(buttons[i].x, buttons[i].y, layer, text);
            }

            // car gear
            else if (buttons[i].text == 2) {
                sprintf(text, "%d", (int)allCar[cameraCar]->currentGear);
                if (allCar[cameraCar]->speed < 5) {
                    text[0] = 'N';
                } else if (allCar[cameraCar]->reverse) {
                    text[0] = 'R';
                }
                xrenderer->renderText(buttons[i].x, buttons[i].y, layer, text);
            }

            // to finish distance
            else if (buttons[i].text == 3) {
                float dst = allCar[cameraCar]->toFinish;
                dst += distance(allCar[cameraCar]->x, allCar[cameraCar]->z, allCar[cameraCar]->currentGoalEdge.bx, allCar[cameraCar]->currentGoalEdge.bz);
                int a = dst / 1000;
                int b = (int)(dst / 100) % 10;
                sprintf(text, "%d.%dkm", a, b);
                xrenderer->renderText(buttons[i].x, buttons[i].y, layer, text);
            }

            // car position
            else if (buttons[i].text == 4) {
                int pos = 0;
                for (int j = 0; j < carCount; j++) {
                    if (allCar[cameraCar]->toFinish >= allCar[j]->toFinish) {
                        pos++;
                    }
                }
                sprintf(text, "%d.", pos);
                xrenderer->renderText(buttons[i].x, buttons[i].y, layer, text);
            }

            // timeout
            else if (buttons[i].text == 5) {
                if (timeout >= 0) {
                    int a = timeout / 60;
                    int b = (timeout % 60) / 10;
                    int c = timeout % 10;
                    sprintf(text, "%d:%d%d", a, b, c);
                    xrenderer->renderText(buttons[i].x, buttons[i].y, layer, text);
                }
            }

        } else if (buttons[i].type == 1) {
            xrenderer->renderButton(buttons[i].x, buttons[i].y, buttons[i].width, buttons[i].height,
                         layer, images[buttons[i].image], configText[buttons[i].text][config[buttons[i].text]]);
        /// Standart text
        } else if (buttons[i].type == 0) {
            if (strcmp(buttons[i].syntax, "RESET") != 0) {
                if (buttons[i].image < 0) {
                    xrenderer->renderText(buttons[i].x, buttons[i].y, layer, textList[buttons[i].text]);
                } else if (buttons[i].text >= 0)
                    xrenderer->renderButton(buttons[i].x, buttons[i].y, buttons[i].width, buttons[i].height,
                             layer, images[buttons[i].image], textList[buttons[i].text]);
            } else if (allCar[cameraCar]->resetAllowed) {
                if ((allCar[cameraCar]->speed < 5) && active && !physic->locked) {
                    if (buttons[i].image < 0) {
                        xrenderer->renderText(buttons[i].x, buttons[i].y, layer, textList[buttons[i].text]);
                    } else if (buttons[i].text >= 0)
                        xrenderer->renderButton(buttons[i].x, buttons[i].y, buttons[i].width, buttons[i].height,
                                 layer, images[buttons[i].image], textList[buttons[i].text]);
                }
            }
        } else {
            xrenderer->renderButton(buttons[i].x, buttons[i].y, buttons[i].width, buttons[i].height,
                     layer, images[buttons[i].image], "");
        }
    }
}

/**
 * @brief displayScene displaies race scene
 * @param renderer is instance of renderer
 * @param gui is instance of menu
 * @param track is instance of track
 * @param physics is instance of physical engine
 */
void displayScene() {

    /// update camera direction
    if (active) {
        if (direction * 180 / 3.14 - allCar[cameraCar]->rot > 180)
            direction -= 6.28;
        else if (direction * 180 / 3.14 - allCar[cameraCar]->rot < -180)
            direction += 6.28;
        float gap = allCar[cameraCar]->rot * 3.14 / 180.0 - direction;
        if (allCar[cameraCar]->control->getDistance() < 0) {
            direction += gap / 2.0;
        } else {
            direction += gap / 15.0;
        }
    } else {
        direction += 0.01;
        if (direction > 6.28) {
            direction -= 6.28;
        }
    }

    /// fix camera direction
    while (true) {
        if (allCar[cameraCar]->rot < 0)
            allCar[cameraCar]->rot += 360;
        else if (allCar[cameraCar]->rot > 360)
            allCar[cameraCar]->rot -= 360;
        else
            break;
    }

    xrenderer->setMode(0);

    /// set camera    
    float view = allCar[cameraCar]->getView();
    if (allCar[cameraCar]->control->getDistance() < 0) {
        view = maximalPerspective;
    }
    xrenderer->perspective(view, aspect, 0.1, viewDistance);
    physic->getTransform(allCar[cameraCar], 0, mat);
    xrenderer->pushMatrix();
    float cameraX = allCar[cameraCar]->x - sin(direction) * allCar[cameraCar]->control->getDistance() * 2 / (view / 90);
    float cameraY = allCar[cameraCar]->y + allCar[cameraCar]->control->getDistance() / 2 / (view / 90);
    float cameraZ = allCar[cameraCar]->z - cos(direction) * allCar[cameraCar]->control->getDistance() * 2 / (view / 90);
    xrenderer->lookAt(cameraX,cameraY,cameraZ,cameraX + sin(direction),cameraY + sin(-10 * 3.14 / 180),cameraZ + cos(direction),0,1,0);

    /// render track
    xrenderer->enable[1] = false;
    xrenderer->renderModel(trackdata);
    if (trackdata2 != 0) {
        xrenderer->renderModel(trackdata2);
    }

    /// render skydome
    xrenderer->pushMatrix();
    xrenderer->translate(allCar[cameraCar]->x, 0, allCar[cameraCar]->z);
    xrenderer->scale(viewDistance * 0.9f);
    xrenderer->renderModel(skydome);
    xrenderer->popMatrix();

    /// render cars
    for (int i = carCount - 1; i >= 0; i--) {

        ///render car skin
        physic->getTransform(allCar[i],0, mat);
        xrenderer->pushMatrix();
        xrenderer->multMatrix(mat);
        xrenderer->translate(0, allCar[i]->wheelY, 0);
        xrenderer->enable[1] = allCar[i]->control->getNitro() && (allCar[i]->n2o > 1);
        xrenderer->renderModel(allCar[i]->skin);
        xrenderer->popMatrix();

        /// render wheels
        for (int j = 1; j <= 4; j++) {

            physic->getTransform(allCar[i],j,mat);
            xrenderer->pushMatrix();
            xrenderer->multMatrix(mat);
            if (j % 2 == 1)
              xrenderer->rotateY(180);
            xrenderer->renderModel(allCar[i]->wheel);
            xrenderer->popMatrix();
        }
    }


    /// render smoke effects
    if (water == 0) {
        water = getModel("gfx/water.o4s");
        for (int i = 0; i < effLen; i++) {
            eff[i] = *(new Dynamic());
            eff[i].vertices = new float[4095 * 3];
            eff[i].coords = new float[4095 * 2];
            eff[i].count = 0;
        }
    }
    for (int k = 0; k < effLen; k++) {
        if (active || (k != (currentFrame + effLen - 1) % effLen)) {
            water->models[0].texture2D->setFrame(eff[k].frame);
            xrenderer->renderDynamic(eff[k].vertices, eff[k].coords, water->models[0].material, water->models[0].texture2D, eff[k].count / 3);
        }
    }

    // update water
    eff[currentFrame].count = 0;
    for (int i = carCount - 1; i >= 0; i--) {
        for (int j = 1; j <= 4; j++) {
            physic->getTransform(allCar[i],j, mat);
            if (active) {
                physic->getTransform(allCar[i],j, mat);
                for (int k = 0; k < allCar[i]->speed / 25 - 1; k++) {
                    float x = mat[12] + (rand() % 50 - 25) * 0.01f + sin(allCar[i]->rot * 3.14 / 180) * k * 0.5f;
                    float y = mat[13] - 0.2f;
                    float z = mat[14] + (rand() % 50 - 25) * 0.01f + cos(allCar[i]->rot * 3.14 / 180) * k * 0.5f;
                    float s = 2;
                    if (allCar[i]->speed > 150) {
                        s = allCar[i]->speed / 75.0f;
                    }
                    for (int l = 0; l < water->models[0].triangleCount[1] * 3; l++) {
                        eff[currentFrame].vertices[eff[currentFrame].count * 3 + 0] = x + s * (water->models[0].vertices[l * 3 + 0] + water->models[0].reg->minX);
                        eff[currentFrame].vertices[eff[currentFrame].count * 3 + 1] = y + s * (water->models[0].vertices[l * 3 + 1] + water->models[0].reg->minY);
                        eff[currentFrame].vertices[eff[currentFrame].count * 3 + 2] = z + s * (water->models[0].vertices[l * 3 + 2] + water->models[0].reg->minZ);
                        eff[currentFrame].coords[eff[currentFrame].count * 2 + 0] = water->models[0].coords[l * 2 + 0];
                        eff[currentFrame].coords[eff[currentFrame].count * 2 + 1] = water->models[0].coords[l * 2 + 1];
                        eff[currentFrame].count++;
                    }
                }
            }
        }
    }

    // render RTT
    xrenderer->setMode(1);

    /// render GPS arrow
    xrenderer->pushMatrix();
    float a = angle(allCar[cameraCar]->currentEdge, allCar[cameraCar]);
    xrenderer->translate(allCar[cameraCar]->x, allCar[cameraCar]->y + allCar[cameraCar]->control->getDistance() + 0.5, allCar[cameraCar]->z);
    xrenderer->scale(allCar[cameraCar]->control->getDistance());
    if (a != 0)
        xrenderer->rotateY(a * 180 / 3.14);
    xrenderer->renderModel(arrow);
    xrenderer->popMatrix();
    xrenderer->popMatrix();

    if (active) {
        for (int k = 0; k < effLen; k++) {
            eff[k].frame++;
        }
        eff[currentFrame].frame = 0;
        currentFrame++;
        if (currentFrame >= effLen) {
            currentFrame = 0;
        }
    }
}

/**
 * @brief loadScene loads scene from Open4speed config file
 * @param track is instance of last track
 * @param gui is instance of menu
 * @param atributes is track information
 * @param playerCar  is index of choosen car
 * @return loaded scene
 */
void loadScene(std::vector<char*> atributes) {

    /// clear previous scene
    for (int i = 0; i < carCount; i++)
        delete allCar[i];
    delete trackdata;
    if (trackdata2 != 0)
        delete trackdata2;
    trackdata2 = 0;
    delete skydome;
    carCount = 0;

    /// load track
    currentTrack = variable;
    trackdata = getModel(getConfigStr("track_model1", atributes));
    int trackIndex = getConfig("race_track", atributes);
    std::vector<edge> e = trackdata->edges[trackIndex];
    if (strlen(getConfigStr("track_model2", atributes)) > 0) {
        trackdata2 = getModel(getConfigStr("track_model2", atributes));
        e = trackdata2->edges[trackIndex];
    }

    /// load sky
    skydome = getModel(getConfigStr("sky_model", atributes));
    if (arrow == 0) {
        arrow = getModel("gfx/arrow.o4s");
    }
    arrow->models[0].texture2D = skydome->models[1].texture2D;

    /// load player car
    std::vector<char*> carList = getList("CARS");
    allCar[0] = new car(getInput(), e, carList[playerCar], transmission);

    /// load race informations
    allCar[0]->lapsToGo = getConfig("laps", atributes) - 1;
    allCar[0]->finishEdge = getConfig("finish", atributes);
    allCar[0]->currentEdgeIndex = getConfig("player_start", atributes);
    allCar[0]->setStart(allCar[0]->edges[getConfig("player_start", atributes)], 0);

    /// load opponents
    opponentCount = getConfig("opponent_count", atributes);
    for (int i = 0; i < opponentCount; i++) {

        /// racer ai
        if (getConfig(getTag(i + 1, "opponent%d_type"), atributes) == 0) {
            allCar[i + 1] = new car(new airacer(), e, carList[getConfig(getTag(i + 1, "opponent%d_car"), atributes)], true);
            allCar[i + 1]->finishEdge = allCar[0]->finishEdge;
            allCar[i + 1]->lapsToGo = allCar[0]->lapsToGo;
            allCar[i + 1]->setStart(allCar[i + 1]->edges[getConfig(getTag(i + 1, "opponent%d_start"), atributes)], 0);
            allCar[i + 1]->currentEdgeIndex = getConfig(getTag(i + 1, "opponent%d_start"), atributes);

        /// traffic ai
        } else {
            allCar[i + 1] = new car(new aitraffic(), trackdata->edges[(int)getConfig(getTag(i + 1, "opponent%d_track"), atributes)],
                                    carList[getConfig(getTag(i + 1, "opponent%d_car"), atributes)], true);
            allCar[i + 1]->finishEdge = -1;
            allCar[i + 1]->lapsToGo = 32768;
            allCar[i + 1]->setStart(allCar[i + 1]->edges[getConfig(getTag(i + 1, "opponent%d_start"), atributes)], 3);
        }
    }
}

/**
 * @brief updateMenu updates menu state
 * @param gui is instance of menu
 * @return updated instance
 */
void updateMenu() {
    /// rotate car if it is avaible
    if (viewCar)
        allCar[0]->rot++;

    /// check all buttons
    for (unsigned int i = 0; i < buttons.size(); i++) {

        /// input animation
        if (!busy) {
            if (buttons[i].layer < 0.96)
                buttons[i].layer += 0.15;

        /// get mouse pointer position
        float x = mouseX * 100 / (float)screen_width;
        float y = mouseY * 100 / (float)screen_height;

        /// focus button if it is selected
        if ((x > buttons[i].x) & (y > buttons[i].y) & (x < buttons[i].x + buttons[i].width)
            & (y < buttons[i].y + buttons[i].height) & (strcmp(buttons[i].syntax, "NONE") != 0)) {
#ifndef ANDROID
            buttons[i].layer -= 0.01;
#endif
            if ((buttons[i].layer > 0.96) & (buttons[i].layer < 0.97))
                buttons[i].layer = 0.97;
        } else {
#ifndef ANDROID
            buttons[i].layer += 0.01;
#endif
            if (buttons[i].layer > 1)
                buttons[i].layer = 1;
        }

      /// output animation
      } else if (buttons[i].image > 0) {
          buttons[i].layer -= 0.15;
      }
    }

    /// update busy status
    if (busy) {
        busy = false;
        for (unsigned int i = 0; i < buttons.size(); i++) {
            if ((buttons[i].layer > 0.25) && (buttons[i].image > 0))
                busy = true;
        }
    }
}
