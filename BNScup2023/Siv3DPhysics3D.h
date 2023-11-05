#pragma once
#include <Siv3D.hpp>
#include <btBulletDynamicsCommon.h>
#include <math.h>

class PlayerPhysicsObject {
private:
	btRigidBody* rigidBody;
	btCollisionShape* shape;
	s3d::Texture texture;
	s3d::Mesh spriteMesh;

public:
	PlayerPhysicsObject(btCollisionShape* shape, const btVector3& position, float mass, s3d::Texture texture)
		: shape(shape), texture(texture) {
		btTransform startTransform;
		startTransform.setIdentity();
		startTransform.setOrigin(position);

		bool isDynamic = (mass != 0.f);
		btVector3 localInertia(0, 0, 0);
		if (isDynamic) {
			shape->calculateLocalInertia(mass, localInertia);
		}

		btMotionState* motionState = new btDefaultMotionState(startTransform);
		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, shape, localInertia);
		rigidBody = new btRigidBody(rbInfo);
		rigidBody->setAngularFactor(btVector3(0, 1, 0));

		btCylinderShape* cylinderShape = static_cast<btCylinderShape*>(shape);
		float width = cylinderShape->getRadius() * 2;
		float height = cylinderShape->getHalfExtentsWithMargin().getY() * 2;
		spriteMesh = Mesh{ MeshData::TwoSidedPlane(SizeF{ width, height }).rotate(Quaternion::RotateX(-90_deg)) };
	}

	// キーボードの入力をチェック
	void HandleInput() {
		btVector3 moveDirection(0, 0, 0);
		btVector3 upDirection(0, 0, 0);
		float speed = 10.0;

		if (KeyW.pressed()) {
			moveDirection += btVector3(0, 0, 1);  // 前進
		}
		if (KeyS.pressed()) {
			moveDirection += btVector3(0, 0, -1);   // 後退
		}
		if (KeyA.pressed()) {
			moveDirection += btVector3(-1, 0, 0);  // 左へ
		}
		if (KeyD.pressed()) {
			moveDirection += btVector3(1, 0, 0);   // 右へ
		}
		if (KeySpace.down()) {
			upDirection += btVector3(0, 20, 0);   // 上へ
		}

		// 入力に応じて移動方向を正規化し、所望の速度を掛ける
		if (moveDirection != btVector3(0, 0, 0)) {
			moveDirection.normalize();
			moveDirection *= speed;
			moveDirection.setY(rigidBody->getLinearVelocity().getY());
			// btRigidBodyの速度を設定
			rigidBody->setLinearVelocity(moveDirection);
		}
		else {
			moveDirection.setY(rigidBody->getLinearVelocity().getY());
			// btRigidBodyの速度を設定
			rigidBody->setLinearVelocity(moveDirection);
		}

		if (upDirection != btVector3(0, 0, 0)) {
			upDirection.setX(rigidBody->getLinearVelocity().getX());
			upDirection.setZ(rigidBody->getLinearVelocity().getZ());
			rigidBody->setLinearVelocity(upDirection);
		}
	}

	btRigidBody* getRigidBody() {
		return rigidBody;
		// return rigidBody.get();
	}

	Vec3 getPosition() {
		btVector3 btPos = rigidBody->getWorldTransform().getOrigin();
		s3d::Vec3 position(btPos.x(), btPos.y(), btPos.z());
		return position;
	}

	void draw(Vec3 direction) const {
		btVector3 btPos = rigidBody->getWorldTransform().getOrigin();
		s3d::Vec3 position(btPos.x(), btPos.y(), btPos.z());
		float x = direction.x;
		float z = direction.z;
		float theta = ((atan2(z, x) * 180.0f) / 3.14159265) - 90.0f;
		spriteMesh.draw(position, Quaternion::RotateY(theta), texture);
	}
};

class PhysicsObject {
private:
	btRigidBody* rigidBody;
	btCollisionShape* shape;
	//std::unique_ptr<btRigidBody> rigidBody;
	//std::unique_ptr<btCollisionShape> shape;
	s3d::ColorF color;
	s3d::Texture texture;
	float isTexture = false;

public:
	PhysicsObject(btCollisionShape* shape, const btVector3& position, float mass, s3d::ColorF color = s3d::ColorF(1.0, 1.0, 1.0))
		: shape(shape), color(color) {
		btTransform startTransform;
		startTransform.setIdentity();
		startTransform.setOrigin(position);

		bool isDynamic = (mass != 0.f);
		btVector3 localInertia(0, 0, 0);
		if (isDynamic) {
			shape->calculateLocalInertia(mass, localInertia);
		}

		btMotionState* motionState = new btDefaultMotionState(startTransform);
		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, shape, localInertia);
		rigidBody = new btRigidBody(rbInfo); //std::make_unique<btRigidBody>(rbInfo);
	}

	PhysicsObject(btCollisionShape* shape, const btVector3& position, float mass, s3d::Texture texture)
		: shape(shape), texture(texture) {
		btTransform startTransform;
		startTransform.setIdentity();
		startTransform.setOrigin(position);

		bool isDynamic = (mass != 0.f);
		btVector3 localInertia(0, 0, 0);
		if (isDynamic) {
			shape->calculateLocalInertia(mass, localInertia);
		}

		btMotionState* motionState = new btDefaultMotionState(startTransform);
		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, shape, localInertia);
		rigidBody = new btRigidBody(rbInfo); //std::make_unique<btRigidBody>(rbInfo);
		isTexture = true;
	}

	btRigidBody* getRigidBody() {
		return rigidBody;
		// return rigidBody.get();
	}

	void draw() const {
		btVector3 btPos = rigidBody->getWorldTransform().getOrigin();
		// s3d::Vec3 position(btPos.x(), btPos.y(), btPos.z());

		if (shape->getShapeType() == BOX_SHAPE_PROXYTYPE) {
			s3d::Vec3 position(btPos.x(), btPos.y(), btPos.z());
			btBoxShape* boxShape = static_cast<btBoxShape*>(shape);
			btVector3 halfExtents = boxShape->getHalfExtentsWithoutMargin();
			//Vec3 position = s3d::Vec3(rigidBody->getWorldTransform().getOrigin());
			//btVector3 btPos = rigidBody->getWorldTransform().getOrigin();
			//s3d::Vec3 position(btPos.x(), btPos.y(), btPos.z());
			if (isTexture) {
				s3d::Box(position, halfExtents.x() * 2, halfExtents.y() * 2, halfExtents.z() * 2).draw(texture);
			}
			else {
				s3d::Box(position, halfExtents.x() * 2, halfExtents.y() * 2, halfExtents.z() * 2).draw(color);
			}
		}
		else if (shape->getShapeType() == SPHERE_SHAPE_PROXYTYPE) {
			s3d::Vec3 position(btPos.x(), btPos.y(), btPos.z());
			btSphereShape* sphereShape = static_cast<btSphereShape*>(shape);
			float radius = sphereShape->getRadius();
			//Vec3 position = s3d::Vec3(rigidBody->getWorldTransform().getOrigin());
			//btVector3 btPos = rigidBody->getWorldTransform().getOrigin();
			//s3d::Vec3 position(btPos.x(), btPos.y(), btPos.z());
			if (isTexture) {
				s3d::Sphere(position, radius).draw(texture);
			}
			else {
				s3d::Sphere(position, radius).draw(color);
			}
		}
		else if (shape->getShapeType() == CYLINDER_SHAPE_PROXYTYPE) {
			s3d::Vec3 position(btPos.x(), btPos.y(), btPos.z());
			btCylinderShape* cylinderShape = static_cast<btCylinderShape*>(shape);
			float radius = cylinderShape->getRadius();
			float height = cylinderShape->getHalfExtentsWithoutMargin().getY() * 2;
			if (isTexture) {
				s3d::Cylinder(position, radius, height).draw(texture);
			}
			else {
				s3d::Cylinder(position, radius, height).draw(color);
			}
		}
		else if (shape->getShapeType() == CONE_SHAPE_PROXYTYPE) {
			btConeShape* coneShape = static_cast<btConeShape*>(shape);
			float radius = coneShape->getRadius();
			float height = coneShape->getHeight();
			s3d::Vec3 position(btPos.x(), btPos.y() - (height / 2.), btPos.z());
			if (isTexture) {
				s3d::Cone(position, radius, height).draw(texture);
			}
			else {
				s3d::Cone(position, radius, height).draw(color);
			}
		}
		else if (shape->getShapeType() == CAPSULE_SHAPE_PROXYTYPE) {
			btCapsuleShape* capsuleShape = static_cast<btCapsuleShape*>(shape);
			float radius = capsuleShape->getRadius();
			float height = capsuleShape->getHalfHeight() * 2;
			// Siv3D does not have a built-in Capsule shape, so you might have to
			// create your own logic for rendering, or use combination of cylinder and sphere.
			// This is just a placeholder:
			// s3d::Capsule(position, radius, height).draw();

		}
		// 他の形状についても必要に応じて追加...
	}
};

class PhysicsWorld {
private:
	btBroadphaseInterface* broadphase;
	btDefaultCollisionConfiguration* collisionConfiguration;
	btCollisionDispatcher* dispatcher;
	btSequentialImpulseConstraintSolver* solver;
	btDiscreteDynamicsWorld* dynamicsWorld;
	/*
	std::unique_ptr<btBroadphaseInterface> broadphase;
	std::unique_ptr<btDefaultCollisionConfiguration> collisionConfiguration;
	std::unique_ptr<btCollisionDispatcher> dispatcher;
	std::unique_ptr<btSequentialImpulseConstraintSolver> solver;
	std::unique_ptr<btDiscreteDynamicsWorld> dynamicsWorld;
	*/

	Array<PhysicsObject*> objects;

public:
	PhysicsWorld() {
		broadphase = new btDbvtBroadphase(); // std::make_unique<btDbvtBroadphase>();
		collisionConfiguration = new btDefaultCollisionConfiguration(); // std::make_unique<btDefaultCollisionConfiguration>();
		dispatcher = new btCollisionDispatcher(collisionConfiguration); // std::make_unique<btCollisionDispatcher>(collisionConfiguration.get());
		solver = new btSequentialImpulseConstraintSolver; // std::make_unique<btSequentialImpulseConstraintSolver>();
		dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration); // std::make_unique<btDiscreteDynamicsWorld>(dispatcher.get(), broadphase.get(), solver.get(), collisionConfiguration.get());
		dynamicsWorld->setGravity(btVector3(0, -30, 0));
	}
	void addObject(PhysicsObject* obj, int i) {
		objects.push_back(obj);
		if (i == 1) {//球体は1
			dynamicsWorld->addRigidBody(obj->getRigidBody(), 1, 2);
		}
		else {
			dynamicsWorld->addRigidBody(obj->getRigidBody(), 2, 1);
		}
		//dynamicsWorld->addRigidBody(objects.back()->getRigidBody());
		//return objects.back().get();
	}

	void addObject(PlayerPhysicsObject* obj) {
		dynamicsWorld->addRigidBody(obj->getRigidBody(), 1, 2);
	}

	void stepSimulation(float deltaTime) {
		dynamicsWorld->stepSimulation(deltaTime);
	}

	void draw() {
		for (const auto& obj : objects) {
			obj->draw();
		}
	}
};
