#pragma once
#include <btBulletDynamicsCommon.h>
#include <vector>
#include "Siv3DBulletObject.hpp"

class Siv3DDynamicsWorld {
public:
	btDiscreteDynamicsWorld* dynamicsWorld;
	btAlignedObjectArray<btCollisionShape*> collisionShapes;
	std::vector<Object*> ObjList;
	Siv3DDynamicsWorld() {
		btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();
		btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collisionConfiguration);
		btBroadphaseInterface* overlappingPairCache = new btDbvtBroadphase();
		btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver;
		dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);
		dynamicsWorld->setGravity(btVector3(0, -10, 0));
	}

	// 剛体の追加
	void add(Object* obj/*btCollisionShape* shape, btRigidBody* body*/) {
		ObjList.push_back(obj);
		collisionShapes.push_back(obj->shape);
		dynamicsWorld->addRigidBody(obj->body);
	}

	// 剛体の削除
	void remove(btRigidBody* body) {
		dynamicsWorld->removeRigidBody(body);
	}

	void update() {

	}
};
