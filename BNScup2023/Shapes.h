#pragma once
#include"Object.h"
#include <btBulletDynamicsCommon.h>
#include <Siv3D.hpp>
#include <atltrace.h>
#include "Siv3DPhysics3D.h"


class BoxObject : public Object {//ボックスを作るための縦横高さを新たに持つ
public:
	// BoxObjectのコンストラクタ
	BoxObject(axis3D axis, float width, float height, float depth) : Object(axis.x, axis.y, axis.z) {
		//横縦長さからボックスを作る(表示用)
		this->width = width;
		this->height = height;
		this->depth = depth;
		this->shape = Box{ axis.x,axis.y,axis.z, this->width,this->height,this->depth };

		//Bulletのボックス
		Position = btVector3(axis.x, axis.y, axis.z);
		boxBox = new btBoxShape(btVector3(btScalar(width/1.99), btScalar(height/1.99), btScalar(depth/1.99)));
		bulletBox = new PhysicsObject(boxBox, btVector3(axis.x, axis.y, axis.z), 0);  // 位置(0,5,0)に設置

		//BulletとBoxが喧嘩してて当たり判定拾ってくれないので少し大きめの当たり判定用のボックス
		float atarirange = 1.0;
		atarihanteishape= Box{ axis.x,axis.y,axis.z, this->width+ atarirange,this->height+ atarirange,this->depth + atarirange };

	}
	float width;
	float height;
	float depth;
	Box shape;
	Box atarihanteishape;
	bool have_texture = false;//テクスチャの有無でdisplayを変える

	//Bullet用
	btVector3 Position;
	btCollisionShape* boxBox;  
	PhysicsObject* bulletBox;  // これをworldに追加すれば物理演算


	void display() {
		// ボックスを描画
		this->shape = Box{ axis.x,axis.y,axis.z, width,height,depth };
		//shape.draw(rotation, color.removeSRGBCurve());

	}
	void display(Texture texture) {
		// ボックスを描画
		this->shape = Box{ axis.x,axis.y,axis.z, width,height,depth };
		shape.draw(rotation, texture);

	}


};


class SphereObject : public Object {

public:
	// BoxObjectのコンストラクタ
	SphereObject(axis3D axis, float radius) : Object(axis.x, axis.y, axis.z) {
		//半径ともらった座標から球を生成
		this->radius = radius;
		this->shape = Sphere{ axis.x,axis.y,axis.z, this->radius };

		//Bulelt用の情報
		Position = btVector3(axis.x, axis.y, axis.z);
		sphereShape = new btSphereShape(0.5);  // 半径1の球
		bulletsphere = new PhysicsObject(sphereShape, Position, 1.0);  // 位置(0,5,0)に設置
	}
	float radius;
	float returnxz;
	Sphere shape;
	btVector3 Position;
	btCollisionShape* sphereShape;  // 半径1の球
	PhysicsObject* bulletsphere;  // 位置(0,5,0)に設置

	void display() {
		// 球体の表示
		shape = Sphere{ axis.x,axis.y,axis.z, radius };
		//shape.draw(rotation, color.removeSRGBCurve());
	}


	bool GrounHandle(Array<BoxObject>Boxes) {
		for (int i = 0; i < Boxes.size(); i++) {
			if (shape.intersects(Boxes[i].atarihanteishape) && axis.y - radius > Boxes[i].axis.y + Boxes[i].height / 2.0 -1.0) {
				return true;
			}
		}
		return false;
	}
	int WhichGround(Array<BoxObject>Boxes) {//球がどの足場に着地しているか
		for (int i = 0; i < Boxes.size(); i++) {
			if (shape.intersects(Boxes[i].atarihanteishape) && axis.y - radius > Boxes[i].axis.y + Boxes[i].height / 2.0 - 1.0) {
				return i;
			}
		}
		return -1;
	}



};


class ConeObject : public Object {

public:

	ConeObject(axis3D axis, float radius,float top) : Object(axis.x, axis.y, axis.z) {
		//半径ともらった座標から球を生成
		this->radius = radius;
		this->top = top;
		this->shape = Cone{ axis.x,axis.y,axis.z,radius,top };

		ConeShape= new btConeShape(radius*2, top*2);
		bulletcone = new PhysicsObject(ConeShape, btVector3(axis.x,axis.y,axis.z), 0., ColorF{ 0.0, 0., 0. });  // 位置(0,5,0)に設置
	}
	float radius;
	float returnxz;
	float top;
	Cone shape;
	btVector3 Position;
	btCollisionShape* ConeShape;  // 半径1の球
	PhysicsObject* bulletcone;  // 位置(0,5,0)に設置

	void display() {
		// 球体の表示
		shape =Cone{ axis.x,axis.y,axis.z, radius,top };
		//shape.draw(rotation, color.removeSRGBCurve());
	}


	void display(Texture texture) {
		// 球体の表示
		//shape = Cone{ axis.x,axis.y,axis.z, radius,top };
		shape.draw(rotation, texture);
	}


};
