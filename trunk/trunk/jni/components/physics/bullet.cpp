//----------------------------------------------------------------------------------------
/**
 * \file       bullet.cpp
 * \author     Vonasek Lubos
 * \date       2013/04/20
 * \brief      Physical model for scene, it detects collision and also calculate collision
 *             reactions. Also applies car physical and visual state.
*/
//----------------------------------------------------------------------------------------

#include "stdafx.h"

/**
 * @brief The bulletConfig struct
 */
struct bulletConfig {
    float brakeAspect, engineMaxSpeed, engineSpeedMultiply, gasAspect, gravitation,
    overSpeedBraking, rollInfluence, skinFriction, speedDecrease, steeringAspect,
    steeringSpeedDependency, suspensionCompression, suspensionDamping, suspensionStiffness,
    underSpeedBraking, vehicleMassAspect, vehicleStep, wheelFriction, worldLimit, worldStep,
    worldSubStep;
};

bulletConfig bcfg = *(new bulletConfig());             ///< Configuration of physical engine

btDynamicsWorld* m_dynamicsWorld;           ///< Physical implementation of scene
btDefaultCollisionConfiguration* m_collisionConfiguration;
btCollisionDispatcher* m_dispatcher;
btBroadphaseInterface* m_overlappingPairCache;
btConstraintSolver* m_constraintSolver;

std::vector<btRaycastVehicle*> m_vehicle = *(new std::vector<btRaycastVehicle*>);   ///< Physical implementation of car transformation
std::vector<btRigidBody*> bodies = *(new std::vector<btRigidBody*>());           ///< Physical implementation of dynamic objects
std::vector<btCollisionShape*> shapes = *(new std::vector<btCollisionShape*>());
std::vector<btCompoundShape*> compounds = *(new std::vector<btCompoundShape*>());
std::vector<btTriangleMesh*> meshes = *(new std::vector<btTriangleMesh*>());
std::vector<btCollisionShape*> levelShapes = *(new std::vector<btCollisionShape*>());
std::vector<btRigidBody*> bodies2 = *(new std::vector<btRigidBody*>());           ///< Physical implementation of objects
std::vector<btCollisionShape*> chassisShapes = *(new std::vector<btCollisionShape*>());
std::vector<btVehicleRaycaster*> m_vehicleRayCasters = *(new std::vector<btVehicleRaycaster*>());
std::vector<btRaycastVehicle*> m_RaycastVehicles = *(new std::vector<btRaycastVehicle*>());

float eyemat[16] = {
    1,0,0,0,
    0,1,0,0,
    0,0,1,0,
    0,0,0,1
};

/**
 * @brief Construct physical model
 * @param m is 3D model for physical model
 */
bullet::bullet(model *m) {

    /// Get configuration
    std::vector<char*> atributes = getList("BULLETCFG");
    bcfg.brakeAspect = getConfig("brakeAspect", atributes);
    bcfg.engineMaxSpeed = getConfig("engineMaxSpeed", atributes);
    bcfg.engineSpeedMultiply = getConfig("engineSpeedMultiply", atributes);
    bcfg.gasAspect = getConfig("gasAspect", atributes);
    bcfg.gravitation = getConfig("gravitation", atributes);
    bcfg.overSpeedBraking = getConfig("overSpeedBraking", atributes);
    bcfg.rollInfluence = getConfig("rollInfluence", atributes);
    bcfg.skinFriction = getConfig("skinFriction", atributes);
    bcfg.speedDecrease = getConfig("speedDecrease", atributes);
    bcfg.steeringAspect = getConfig("steeringAspect", atributes);
    bcfg.steeringSpeedDependency = getConfig("steeringSpeedDependency", atributes);
    bcfg.suspensionCompression = getConfig("suspensionCompression", atributes);
    bcfg.suspensionDamping = getConfig("suspensionDamping", atributes);
    bcfg.suspensionStiffness = getConfig("suspensionStiffness", atributes);
    bcfg.underSpeedBraking = getConfig("underSpeedBraking", atributes);
    bcfg.vehicleMassAspect = getConfig("vehicleMassAspect", atributes);
    bcfg.vehicleStep = getConfig("vehicleStep", atributes);
    bcfg.wheelFriction = getConfig("wheelFriction", atributes);
    bcfg.worldLimit = getConfig("worldLimit", atributes);
    bcfg.worldStep = getConfig("worldStep", atributes);
    bcfg.worldSubStep = getConfig("worldSubStep", atributes);

    locked = true;
    m_collisionConfiguration = new btDefaultCollisionConfiguration();
    m_dispatcher = new btCollisionDispatcher(m_collisionConfiguration);
    btVector3 worldMin(-bcfg.worldLimit,-bcfg.worldLimit,-bcfg.worldLimit);
    btVector3 worldMax(bcfg.worldLimit,bcfg.worldLimit,bcfg.worldLimit);
    m_overlappingPairCache = new btAxisSweep3(worldMin,worldMax);
    m_constraintSolver = new btSequentialImpulseConstraintSolver();
    m_dynamicsWorld = new btDiscreteDynamicsWorld(m_dispatcher,m_overlappingPairCache,m_constraintSolver,m_collisionConfiguration);
    m_dynamicsWorld->setGravity(btVector3(0,-bcfg.gravitation,0));

    /// Create scene
    addModel(m);
}

/**
 * @brief bullet destructor
 */
bullet::~bullet() {
    while(!m_vehicle.empty()) {
        delete m_vehicle[0];
        m_vehicle.erase(m_vehicle.begin());
    }
    while(!bodies.empty()) {
        delete bodies[0];
        bodies.erase(bodies.begin());
    }
    while(!shapes.empty()) {
        delete shapes[0];
        shapes.erase(shapes.begin());
    }
    while(!compounds.empty()) {
        delete compounds[0];
        compounds.erase(compounds.begin());
    }
    while(!meshes.empty()) {
        delete meshes[0];
        meshes.erase(meshes.begin());
    }
    while(!levelShapes.empty()) {
        delete levelShapes[0];
        levelShapes.erase(levelShapes.begin());
    }
    while(!bodies2.empty()) {
        delete bodies2[0];
        bodies2.erase(bodies2.begin());
    }
    while(!chassisShapes.empty()) {
        delete chassisShapes[0];
        chassisShapes.erase(chassisShapes.begin());
    }
    while(!m_vehicleRayCasters.empty()) {
        delete m_vehicleRayCasters[0];
        m_vehicleRayCasters.erase(m_vehicleRayCasters.begin());
    }
    while(!m_RaycastVehicles.empty()) {
        //delete m_RaycastVehicles[0];
        m_RaycastVehicles.erase(m_RaycastVehicles.begin());
    }

    //delete m_dynamicsWorld;
    delete m_collisionConfiguration;
    delete m_dispatcher;
    delete m_overlappingPairCache;
    delete m_constraintSolver;
}

/**
 * @brief addCar adds car into physical model
 * @param c is car instance
 */
void bullet::addCar(car* c) {

    /// Create car skin
    /**
     * experiment
    btConvexHullShape* compound = new btConvexHullShape();
    btTransform localTrans;
    localTrans.setIdentity();
    localTrans.setOrigin(btVector3(0, -c->wheelY / 2.0, 0));
    for (unsigned int i = 0; i < c->skin->models.size(); i++) {
        for (int j = 0; j < c->skin->models[i].triangleCount[0] * 3; j++) {
            compound->addPoint(btVector3(c->skin->models[i].vertices[j * 3 + 0],
                                         c->skin->models[i].vertices[j * 3 + 1],
                                         c->skin->models[i].vertices[j * 3 + 2]));
        }
    }*/

    btCollisionShape* chassisShape = new btBoxShape(btVector3(c->skin->width / 2.0f, c->skin->aplitude / 2.0f,c->skin->height / 2.0f));
    chassisShapes.push_back(chassisShape);
    btCompoundShape* compound = new btCompoundShape();
    compounds.push_back(compound);
    btTransform localTrans;
    localTrans.setIdentity();
    localTrans.setOrigin(btVector3(0, c->wheelY, 0));
    compound->addChildShape(localTrans,chassisShape);

    /// Set car physical values
    btVector3 localInertia(0,0,0);
    float mass = bcfg.vehicleMassAspect * c->mass;
    compound->calculateLocalInertia(mass,localInertia);
    btRigidBody* m_carChassis = new btRigidBody(mass,0,compound,localInertia);
    bodies2.push_back(m_carChassis);
    m_dynamicsWorld->addRigidBody(m_carChassis);

    /// Set car default transform
    btVehicleRaycaster* m_vehicleRayCaster = new btDefaultVehicleRaycaster(m_dynamicsWorld);
    m_vehicleRayCasters.push_back(m_vehicleRayCaster);
    btTransform tr = *(new btTransform());
    tr.setIdentity();
    tr.setOrigin(btVector3(c->x,c->y - 0.1f,c->z));
    btQuaternion qn = *(new btQuaternion());
    qn.setW(btScalar(cos(c->rot * 3.14 / 180 / 2.0f)));
    qn.setX(btScalar(0));
    qn.setY(btScalar(sin(c->rot * 3.14 / 180 / 2.0f)));
    qn.setZ(btScalar(0));
    tr.setRotation(qn);

    /// Create car
    m_carChassis->setCenterOfMassTransform(tr);
    btRaycastVehicle::btVehicleTuning m_tuning = *(new btRaycastVehicle::btVehicleTuning());
    btRaycastVehicle *m_RaycastVehicle = new btRaycastVehicle(m_tuning,m_carChassis,m_vehicleRayCaster);
    m_RaycastVehicles.push_back(m_RaycastVehicle);
    m_vehicle.push_back(m_RaycastVehicle);
    m_carChassis->setActivationState(DISABLE_DEACTIVATION);
    //m_carChassis->setFriction(bcfg.skinFriction);
    m_dynamicsWorld->addVehicle(m_vehicle[c->index - 1]);
    m_vehicle[c->index - 1]->setCoordinateSystem(0,1,2);

    /// Set wheels connections
    btVector3 wheelDirectionCS0(0,-1,0);
    btVector3 wheelAxleCS(-1,0,0);
    btScalar suspensionRestLength(0.5);
    btVector3 connectionPointCS0(-c->wheelX, 0, -c->wheelZ1);
    m_vehicle[c->index - 1]->addWheel(connectionPointCS0, wheelDirectionCS0, wheelAxleCS, suspensionRestLength, c->wheel->aplitude / 2.0f, m_tuning, true);
    connectionPointCS0 = btVector3(c->wheelX, 0, -c->wheelZ1);
    m_vehicle[c->index - 1]->addWheel(connectionPointCS0, wheelDirectionCS0, wheelAxleCS, suspensionRestLength, c->wheel->aplitude / 2.0f, m_tuning, true);
    connectionPointCS0 = btVector3(-c->wheelX, 0, c->wheelZ2);
    m_vehicle[c->index - 1]->addWheel(connectionPointCS0, wheelDirectionCS0, wheelAxleCS, suspensionRestLength, c->wheel->aplitude / 2.0f, m_tuning, false);
    connectionPointCS0 = btVector3(c->wheelX, 0, c->wheelZ2);
    m_vehicle[c->index - 1]->addWheel(connectionPointCS0, wheelDirectionCS0, wheelAxleCS, suspensionRestLength, c->wheel->aplitude / 2.0f, m_tuning, false);

    /// Set wheels parameters
    for (int i = 0; i < m_vehicle[c->index - 1]->getNumWheels(); i++) {
        btWheelInfo& wheel = m_vehicle[c->index - 1]->getWheelInfo(i);
        wheel.m_suspensionStiffness = bcfg.suspensionStiffness;
        wheel.m_wheelsDampingRelaxation = bcfg.suspensionDamping;
        wheel.m_wheelsDampingCompression = bcfg.suspensionCompression;
        wheel.m_frictionSlip = bcfg.wheelFriction;
        wheel.m_rollInfluence = bcfg.rollInfluence;
    }
}

/**
 * @brief addModel adds model into physical model
 * @param m is 3D model for physical model
 */
void bullet::addModel(model *m) {
    for (unsigned int i = 0; i < m->models.size(); i++) {
        if (m->models[i].dynamic) {

            /// Create object
            float w = m->models[i].reg->maxX - m->models[i].reg->minX;
            float a = m->models[i].reg->maxY - m->models[i].reg->minY;
            float h = m->models[i].reg->maxZ - m->models[i].reg->minZ;
            btCollisionShape* shape = new btBoxShape(btVector3(w / 2, a / 2, h / 2));
            shapes.push_back(shape);
            btCompoundShape* compound = new btCompoundShape();
            compounds.push_back(compound);
            btTransform localTrans;
            localTrans.setIdentity();
            compound->addChildShape(localTrans,shape);

            /// Set object physical values
            btVector3 localInertia(0,0,0);
            float mass = bcfg.vehicleMassAspect * w * a * h;
            compound->calculateLocalInertia(mass,localInertia);
            btRigidBody* body = new btRigidBody(mass,0,compound,localInertia);
            bodies.push_back(body);
            m_dynamicsWorld->addRigidBody(body);

            /// Set object default transform
            btTransform tr = *(new btTransform());
            tr.setIdentity();
            tr.setOrigin(btVector3(m->models[i].reg->minX+w/2, m->models[i].reg->minY+a/2, m->models[i].reg->minZ+h/2));

            /// Create object
            body->setCenterOfMassTransform(tr);
            body->setFriction(bcfg.skinFriction);
            m->models[i].dynamicID = bodies.size();

            /// set default position
            getTransform(m->models[i].dynamicID, mat);
            m->models[i].x = mat[12];
            m->models[i].y = mat[13];
            m->models[i].z = mat[14];
        } else if (m->models[i].touchable) {
            btTriangleMesh* mesh = new btTriangleMesh();
            meshes.push_back(mesh);
            for (int j = 0; j < m->models[i].triangleCount[m->cutX * m->cutY]; j++) {
                btVector3 a = btVector3(m->models[i].vertices[j * 9 + 0], m->models[i].vertices[j * 9 + 1], m->models[i].vertices[j * 9 + 2]);
                btVector3 b = btVector3(m->models[i].vertices[j * 9 + 3], m->models[i].vertices[j * 9 + 4], m->models[i].vertices[j * 9 + 5]);
                btVector3 c = btVector3(m->models[i].vertices[j * 9 + 6], m->models[i].vertices[j * 9 + 7], m->models[i].vertices[j * 9 + 8]);
                mesh->addTriangle(a, b, c);
            }
            //btTriangleIndexVertexArray* m_indexVertexArrays = new btTriangleIndexVertexArray(m->models[i].triangleCount[m->cutX * m->cutX], btindices, 3*sizeof(int), m->models[i].triangleCount[m->cutX * m->cutX] * 3,&m->models[i].vertices[0],3*sizeof(float));
            btCollisionShape* levelShape = new btBvhTriangleMeshShape(mesh,true);
            levelShapes.push_back(levelShape);
            btVector3 localInertia(0,0,0);
            btTransform localTrans = *(new btTransform());
            localTrans.setIdentity();
            localTrans.setOrigin(btVector3(m->models[i].reg->minX, m->models[i].reg->minY, m->models[i].reg->minZ));
            btRigidBody* body = new btRigidBody(0,0,levelShape,localInertia);
            bodies2.push_back(body);
            body->setCenterOfMassTransform(localTrans);
            m_dynamicsWorld->addRigidBody(body);
        }
    }
}

/**
 * @brief getMultiply gets engine speed multiply
 * @return count of multiply
 */
int bullet::getMultiply() {
    return bcfg.engineSpeedMultiply;
}

/**
 * @brief getTransform counts OpenGL matrix of transformation
 * @param c is instance of car
 * @param index is index of object
 * @return transformation matrix
 */
void bullet::getTransform(car* c, int index, float* m) {

    /// count angle
    btQuaternion qn = m_vehicle[c->index - 1]->getRigidBody()->getOrientation();
    float rot = getRotation(qn.x(),qn.y(),qn.z(),qn.w());
    if (!isnan(rot)) {
        c->rot=rot;
    }

    /// get position
    float x = m_vehicle[c->index - 1]->getRigidBody()->getCenterOfMassPosition().getX();
    float y = m_vehicle[c->index - 1]->getRigidBody()->getCenterOfMassPosition().getY();
    float z = m_vehicle[c->index - 1]->getRigidBody()->getCenterOfMassPosition().getZ();
    if (!isnan(x) && !isnan(y) && !isnan(z)) {
        c->x=x;
        c->y=y;
        c->z=z;
    }

    /// get matrix
    if (index > 0) {
      m_vehicle[c->index - 1]->getWheelInfo(index - 1).m_worldTransform.getOpenGLMatrix(m);
    } else {
      m_vehicle[c->index - 1]->getRigidBody()->getCenterOfMassTransform().getOpenGLMatrix(m);
    }
    for (int i = 0; i < 16; i++) {
        if (isnan(m[i])) {
            for (int j = 0; j < 16; j++) {
                m[j] = eyemat[j];
            }
            return;
        }
    }
}

/**
 * @brief getTransform counts OpenGL matrix of transformation
 * @param index is index of object
 * @return transformation matrix
 */
void bullet::getTransform(int index, float* m) {

    /// get matrix
    bodies[index]->getCenterOfMassTransform().getOpenGLMatrix(m);
    for (int i = 0; i < 16; i++) {
        if (isnan(m[i])) {
            for (int j = 0; j < 16; j++) {
                m[j] = eyemat[j];
            }
            return;
        }
    }
}

/**
 * @brief resetCar updates car state
 * @param c is instance of car
 */
void bullet::resetCar(car* c) {
    c->resetRequested = false;
    c->setStart(c->currentEdge, c->edgeSideMove);
    btTransform tr;
    tr.setIdentity();
    tr.setOrigin(btVector3(c->x,c->y - 0.1f,c->z));
    btQuaternion q;
    q.setRotation(btVector3(0,1,0), c->rot * 3.14 / 180.0);
    tr.setRotation(q);
    m_vehicle[c->index - 1]->getRigidBody()->setCenterOfMassTransform(tr);
}

/**
 * @brief updateCar updates car state
 * @param c is instance of car
 */
void bullet::updateCar(car* c) {

    /// get direction
    if ((int)m_vehicle[c->index - 1]->getCurrentSpeedKmHour() < 0)
        c->reverse = true;
    if ((int)m_vehicle[c->index - 1]->getCurrentSpeedKmHour() > 0)
        c->reverse = false;

    if (absf(m_vehicle[c->index - 1]->getCurrentSpeedKmHour()) < 5) {
        if (c->control->getGas() > 0)
            c->reverse = false;
        if (c->control->getBrake() > 0)
            c->reverse = true;
    }

    /// Set power
    float acc = c->acceleration;
    float max = c->gears[c->currentGear].max;
    if (c->n2o < 150) {
        c->n2o += 0.1f;
    }
    if (c->control->getNitro()) {
        if (c->n2o >= 1) {
            acc *= 2;
            max *= 1.5f;
            c->n2o--;
        }
    }
    float gEngineForce = c->control->getGas() * (bcfg.engineMaxSpeed - c->speed) * acc * bcfg.gasAspect;
    float gBreakingForce = c->control->getBrake() * c->brakePower * bcfg.brakeAspect;
    if (c->reverse) {
        gEngineForce = c->control->getBrake() * (bcfg.engineMaxSpeed - c->speed) * acc * bcfg.gasAspect;
        gBreakingForce = c->control->getGas() * c->brakePower * bcfg.brakeAspect;
    }


    if (c->speed < c->gears[c->currentGear].min) {
        gEngineForce = 0;
        gBreakingForce = bcfg.underSpeedBraking;
    }

    /// Limit power
    if (max < c->speed) {
        gEngineForce = 0;
        gBreakingForce = bcfg.overSpeedBraking;
    }

    /// Apply power
    if (!locked) {
        float gVehicleSteering = c->control->getSteer() * c->steering;
            gVehicleSteering /= (bcfg.steeringAspect + c->speed * bcfg.steeringSpeedDependency);

        m_vehicle[c->index - 1]->setSteeringValue(gVehicleSteering,2);
        m_vehicle[c->index - 1]->setSteeringValue(gVehicleSteering,3);

        if (c->reverse) {
            m_vehicle[c->index - 1]->applyEngineForce(-gEngineForce,0);
            m_vehicle[c->index - 1]->applyEngineForce(-gEngineForce,1);
        } else {
            m_vehicle[c->index - 1]->applyEngineForce(gEngineForce,0);
            m_vehicle[c->index - 1]->applyEngineForce(gEngineForce,1);
        }
    }
    m_vehicle[c->index - 1]->setBrake(gBreakingForce + bcfg.speedDecrease, 0);
    m_vehicle[c->index - 1]->setBrake(gBreakingForce + bcfg.speedDecrease, 1);

    for (int i = 0; i < 4; i++)
        m_vehicle[c->index - 1]->updateWheelTransform(i,true);

    /// Other updates
    m_vehicle[c->index - 1]->updateVehicle(btScalar(bcfg.vehicleStep));

    /// Reset car
    if ((c->speed < 5) && active && !physic->locked) {
        if (c->onRoof > 30) {
            c->resetAllowed = true;
            if (c->index - 1 != cameraCar) {
                c->resetRequested = true;
            }
            c->onRoof = 0;
        } else if (c->speed < 5) {
            c->onRoof++;
        }
    } else {
        c->resetAllowed = false;
    }

    if (c->resetAllowed && c->resetRequested) {
        for (int i = 0; i < carCount; i++) {
            if (i != c->index - 1) {
                if (dist(c->currentEdge.ax, c->currentEdge.ay, c->currentEdge.az, allCar[i]->x, allCar[i]->y, allCar[i]->z) < 10) {
                    return;
                }
            }
        }
        resetCar(c);
    }
}

/**
 * @brief updateWorld updates world state
 */
void bullet::updateWorld() {
     m_dynamicsWorld->stepSimulation(bcfg.worldStep, bcfg.worldSubStep);
}
