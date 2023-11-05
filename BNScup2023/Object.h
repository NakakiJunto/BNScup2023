#pragma once
//Bulletじゃないやつの座標で使う
struct axis3D { float x, y, z; };

//axis達への関数
axis3D subaxis(axis3D a, axis3D b) {
	float x=b.x-a.x;
	float y = b.y - a.y;
	float z = b.z - a.z;
	return axis3D(x, y, z);
}


axis3D addaxis(axis3D a, axis3D b) {
	float x = a.x + b.x;
	float y = a.y + b.y;
	float z = a.z + b.z;
	return axis3D{ x,y,z };
}

axis3D multifloataxis(axis3D a,float b) {
	float x = a.x * b;
	float y = a.y * b;
	float z = a.z * b;
	return axis3D{ x,y,z };
}

float  axissize(axis3D a) {
	return sqrt(a.x * a.x + a.y * a.y + a.z * a.z);
}

axis3D normalizeaxis(axis3D a) {
	float size = axissize(a);
	a.x /= size;
	a.y /= size;
	a.z /= size;
	return a;
}
//axis達への関数（終わり）

//謎定義
#define pi 3.14159265f
#define radian pi/180.0f ; 



class Object {//座標と色と角度とyの速度を持つ
public:
	Object(float x, float y, float z) {
		axis = { x,y,z };
		vy = 0.0f;
		angle = 0.0f;
	}
	axis3D axis;
	ColorF color = ColorF{ 1.0,0.0,0.0 };
	float vy;
	float angle;
	Quaternion rotation = { 0.0f,0.0f,0.0f,1.0f };

};






