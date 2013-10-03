//----------------------------------------------------------------------------------------
/**
 * \file       bullet.h
 * \author     Vonasek Lubos
 * \date       2013/04/20
 * \brief      Physical model for scene, it detects collision and also calculate collision
 *             reactions. Also applies car physical and visual state.
*/
//----------------------------------------------------------------------------------------

#ifndef BULLET_H
#define BULLET_H

#include "btBulletDynamicsCommon.h"
#include "BulletDynamics/Vehicle/btRaycastVehicle.h"
#include "stdafx.h"

/**
 * @brief The bullet physics implementation class
 */
class bullet:public physics {
public:

    /**
     * @brief Construct physical model
     * @param m is 3D model for physical model
     */
    bullet(model *m);

    /**
     * @brief bullet destructor
     */
    virtual ~bullet();

    /**
     * @brief addCar adds car into physical model
     * @param c is car instance
     */
    void addCar(car* c);

    /**
     * @brief addModel adds model into physical model
     * @param m is 3D model for physical model
     */
    void addModel(model *m);

    /**
     * @brief getMultiply gets engine speed multiply
     * @return count of multiply
     */
    int getMultiply();

    /**
     * @brief getTransform counts OpenGL matrix of transformation
     * @param c is instance of car
     * @param index is index of object
     * @return transformation matrix
     */
    void getTransform(car* c, int index, float* m);

    /**
     * @brief getTransform counts OpenGL matrix of transformation
     * @param index is index of object
     * @return transformation matrix
     */
    void getTransform(int index, float* m);

    /**
     * @brief resetCar updates car state
     * @param c is instance of car
     */
    void resetCar(car* c);

    /**
     * @brief updateCar updates car state
     * @param c is instance of car
     */
    void updateCar(car* c);

    /**
     * @brief updateWorld updates world state
     */
    void updateWorld();
};

#endif // BULLET_H
