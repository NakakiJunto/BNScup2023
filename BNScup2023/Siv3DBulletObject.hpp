#pragma once
#include <Siv3D.hpp>
#include <btBulletDynamicsCommon.h>

class Object {
public:
	btCollisionShape* shape;
	btTransform trans;
	btRigidBody* body;

	void draw(ColorF color) {
		if (shape->getShapeType() == BOX_SHAPE_PROXYTYPE) {
			btBoxShape* box = static_cast<btBoxShape*>(shape);
			btVector3 center = trans.getOrigin();
			float h = 2.0f * float(box->getHalfExtentsWithoutMargin().getX());
			float w = 2.0f * float(box->getHalfExtentsWithoutMargin().getY());
			float d = 2.0f * float(box->getHalfExtentsWithoutMargin().getX());
			Box{ float(center.getX()), float(center.getY()), float(center.getZ()) , h, w, d }.draw(color.removeSRGBCurve());
		}
		else if (shape->getShapeType() == SPHERE_SHAPE_PROXYTYPE) {
			btSphereShape* sphere = static_cast<btSphereShape*>(shape);
			btVector3 center = trans.getOrigin();
			Sphere{ float(center.getX()), float(center.getY()), float(center.getZ()) , float(sphere->getRadius()) }.draw(color.removeSRGBCurve());
		}
	}
};

class Siv3DBulletSphere : public Object {
	//btCollisionShape* shape;
	//btTransform trans;
	btScalar r;
public:
	Siv3DBulletSphere(btScalar r, btVector3 position, double m) {
		this->r = r;
		// コリジョン形状の設定
		shape = new btSphereShape(r);
		// 位置の設定
		trans.setIdentity();
		trans.setOrigin(position);
		// 重さの設定
		btScalar mass(m);

		bool isDynamic = (mass != 0.f);
		btVector3 localInertia(0, 0, 0);
		if (isDynamic)
			shape->calculateLocalInertia(mass, localInertia);

		btDefaultMotionState* myMotionState = new btDefaultMotionState(trans);
		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, shape, localInertia);
		body = new btRigidBody(rbInfo);
	}
};

class Siv3DBulletBox : public Object {
	//btCollisionShape* shape;
	//btTransform trans;
	btVector3 scale;
public:
	Siv3DBulletBox(btVector3 scale, btVector3 position, double m) {
		this->scale = scale;
		// コリジョン形状の設定
		shape = new btBoxShape(scale);
		// 位置の設定
		trans.setIdentity();
		trans.setOrigin(position);
		// 重さの設定
		btScalar mass(m);

		bool isDynamic = (mass != 0.f);
		btVector3 localInertia(0, 0, 0);
		if (isDynamic)
			shape->calculateLocalInertia(mass, localInertia);

		btDefaultMotionState* myMotionState = new btDefaultMotionState(trans);
		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, shape, localInertia);
		body = new btRigidBody(rbInfo);
	}
};
