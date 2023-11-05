#pragma once
#include"Shapes.h"
#include"Figure.h"
#include <Siv3D.hpp>
#define PI 3.141592665




class FriendObject : public SphereObject {
public:
	// BoxObjectのコンストラクタ
	FriendObject(axis3D axis, float radius) :SphereObject(axis, radius) {
		movexz = 0.12f;
		color = ColorF(0.68, 0.87, 0.8944);

	}
	float HP = 100;
	float maxHP = 100;
	int max_jump = 2;
	int left_jump_num = max_jump;
	float movexz;
	int direction = 0;
	int direction_change = 0;
	int Friend_seed = 0;
	int attachdirection = 0;//移動
	Mesh shadowMesh{ MeshData::Disc(1.0) };
	float shadowY = axis.y - radius;
	Texture friendTexture{ Image {U"example/texture/FriendTexture.png"} }; // 移動モーション
	Mesh spriteMesh{ MeshData::TwoSidedPlane(SizeF{ 1.5, 1.5 }).rotate(Quaternion::RotateX(-90_deg)) };




	void Run(Array<BoxObject>Boxes) {//行動決定
		
		bulletsphere->getRigidBody()->setLinearVelocity(btVector3(0, bulletsphere->getRigidBody()->getLinearVelocity().getY(), 0));//斜面で起きる速度対策
		bulletsphere->getRigidBody()->setActivationState(DISABLE_DEACTIVATION);

		//体力表示
		Box{ axis.x,axis.y + 1,axis.z ,1.5,0.25,0.25 }.draw(ColorF{ 1.0,0.0,0.0 });
		Box{ axis.x - (1.5 - 1.5 * HP / maxHP) / 2.0,axis.y + 1,axis.z ,1.5 * HP / maxHP,0.26,0.26 }.draw(ColorF{ 0.0,0.0,1.0 });


		if (GrounHandle(Boxes)) {//着地したら残りジャンプ数リセット
			left_jump_num = max_jump;
			shadowY = axis.y - radius;
			vy = 0;
		}
		shadowMesh.draw(Vec3{ axis.x,shadowY,axis.z }, Quaternion::RotateY(0), ColorF{ 0, 0, 0 });
		unsigned int Friend_seed = static_cast<unsigned int>(std::time(nullptr));
		std::srand(Friend_seed);
		if (direction_change == 0) { direction = rand() % 4; }//4方向ランダムに行動変更
		direction_change += 1;//方向変更までの値を貯める
		if (direction_change > 400) { direction_change = 0; }//これぐらいたまると方向転換


		//移動処理と「axis,bulletの座標の同期」
		btVector3 moveDirection(0, 0, 0);
		btVector3 upDirection(0, 0, 0);
		float speed = 0.08;
		float jumpspeed = 0.8;
		if (direction == 0) {
			moveDirection += btVector3(0, 0, 1);  // 前進
			attachdirection = 0;
		}
		if (direction == 1) {
			moveDirection += btVector3(0, 0, -1);   // 後退
			attachdirection = 4;
		}
		if (direction == 2) {
			moveDirection += btVector3(-1, 0, 0);  // 左へ
			attachdirection = 6;
		}
		if (direction == 3) {
			moveDirection += btVector3(1, 0, 0);   // 右へ
			attachdirection = 2;
		}
		// 入力に応じて移動方向を正規化し、所望の速度を掛ける
		if (moveDirection != btVector3(0, 0, 0)) {
			moveDirection.normalize();
			moveDirection *= speed;
		}



		moveDirection.setY(bulletsphere->getRigidBody()->getLinearVelocity().getY());
		btTransform newTransform;
		newTransform.setIdentity();
		btVector3 btPos = bulletsphere->getRigidBody()->getWorldTransform().getOrigin();
		s3d::Vec3 position(btPos.x(), btPos.y(), btPos.z());
		axis = axis3D(position.x, position.y, position.z);
		newTransform.setOrigin(btVector3(position.x + moveDirection.getX(), position.y, position.z + moveDirection.getZ())); // 新しい位置を指定
		bulletsphere->getRigidBody()->setCenterOfMassTransform(newTransform);
		//移動処理「axis,bulletの座標の同期」

		for (int i = 0; i < Boxes.size() - 4; i++)
		{

			if (shape.intersects(Boxes[i].atarihanteishape) && !(axis.y >= Boxes[i].axis.y + Boxes[i].height / 2)) {
				btVector3 upDirection = btVector3(0, 20, 0);
				upDirection.setX(bulletsphere->getRigidBody()->getLinearVelocity().getX());
				upDirection.setZ(bulletsphere->getRigidBody()->getLinearVelocity().getZ());
				bulletsphere->getRigidBody()->setLinearVelocity(upDirection);
				break;
			}
		}


		display();//表示
		getDrawTexture();
		drawMotion();
	}

	int drawX = 0;
	int drawY = 0;
	int motionTime = 0;
	int sequence[4] = { 0, 1, 2, 1 };
	void drawMotion() {
		//btVector3 btPos = bulletsphere->getRigidBody()->getWorldTransform().getOrigin();
		Vec3 position(axis.x, axis.y + 0.5, axis.z);
		const ScopedRenderStates3D sampler{ SamplerState::ClampNearest, BlendState::OpaqueAlphaToCoverage };
		spriteMesh.draw(position, Quaternion::RotateY(0), friendTexture(sequence[drawX] * 32, drawY * 32, 32, 32));
	}

	void getDrawTexture() {
		if (attachdirection == 0) {
			if (drawY == 3) {
				drawX = (motionTime / 20 + 1) % 4;
				motionTime += 1;
			}
			else {
				drawY = 3;
				motionTime = 0;
			}
		}
		else if (attachdirection == 4) {
			if (drawY == 0) {
				drawX = (motionTime / 20 + 1) % 4;
				motionTime += 1;
			}
			else {
				drawY = 0;
				motionTime = 0;
			}
		}
		else if (attachdirection >= 1 && attachdirection <= 3) {
			if (drawY == 2) {
				drawX = (motionTime / 20 + 1) % 4;
				motionTime += 1;
			}
			else {
				drawY = 2;
				motionTime = 0;
			}
		}
		else {
			if (drawY == 1) {
				drawX = (motionTime / 20 + 1) % 4;
				motionTime += 1;
			}
			else {
				drawY = 1;
				motionTime = 0;
			}
		}
	}

};





class PlayerObject : public SphereObject {//球体を作るための半径を新たににもつ
public:
	// BoxObjectのコンストラクタ
	PlayerObject(axis3D axis, float radius) :SphereObject(axis, radius) { }
	int max_jump = 1;//最大ジャンプ数-1
	int left_jump_num = max_jump;//残りのジャンプ数
	float movexz = 0.12;;//移動量
	int Score = 0;//スコア
	int healitemnum = 0;//回復アイテム数
	int bombitemnum = 0;//爆弾数
	int attachdirection = 0;//攻撃方向
	bool bombstate = false;//ボムが未爆発
	float bomblife = 100.0f;//ボムが続く長さ

	Box attack;//攻撃判定用ボックス

	Audio healaudio{ U"example/heal.mp3" };
	Audio bombaudio{ U"example/bomb.mp3" };//回復音、爆発音

	Texture explosiontexture{ Image{ U"example/texture/explosion.jpg" } };//爆発に使うエフェクト
	Sphere explosion;

	Texture playerTexture{ Image {U"example/texture/PlayerTexture.png"} }; // 移動モーション
	Mesh spriteMesh{ MeshData::TwoSidedPlane(SizeF{ 1.5, 1.5 }).rotate(Quaternion::RotateX(-90_deg)) };
	Mesh shadowMesh{ MeshData::Disc(1) };
	Texture effectTexture{ Image {U"example/texture/Effect.png"} };
	Mesh effectMesh{ MeshData::TwoSidedPlane(SizeF{ 1.5, 1.5 }).rotate(Quaternion::RotateX(-90_deg)) };
	float effectTime = 0.0f;
	void PlayerManipulate(Array<BoxObject>Boxes, FriendObject* Friend) {

		bulletsphere->getRigidBody()->setActivationState(DISABLE_DEACTIVATION);


		//攻撃(方向ごとに攻撃位置が変わる)
		if (KeyEnter.down()) {
			float attack_place = 1.0;
			float attack_range = 2.0;
			if (attachdirection == 0) { attack = Box{ axis.x,axis.y,axis.z + attack_place,attack_range,attack_range,attack_range }; }
			else if (attachdirection == 1) { attack = Box{ axis.x + attack_place,axis.y,axis.z + attack_place,attack_range,attack_range,attack_range }; }
			else if (attachdirection == 2) { attack = Box{ axis.x + attack_place ,axis.y,axis.z ,attack_range,attack_range,attack_range }; }
			else if (attachdirection == 3) { attack = Box{ axis.x + attack_place,axis.y,axis.z - attack_place + attack_place,attack_range,attack_range,attack_range }; }
			else if (attachdirection == 4) { attack = Box{ axis.x ,axis.y,axis.z - attack_place + attack_place,attack_range,attack_range,attack_range }; }
			else if (attachdirection == 5) { attack = Box{ axis.x - attack_place,axis.y,axis.z - attack_place + attack_place,attack_range,attack_range,attack_range }; }
			else if (attachdirection == 6) { attack = Box{ axis.x - attack_place,axis.y,axis.z + attack_place,attack_range,attack_range,attack_range }; }
			else if (attachdirection == 7) { attack = Box{ axis.x - attack_place,axis.y,axis.z + attack_place,attack_range,attack_range,attack_range }; }
			effectTime += Scene::DeltaTime();
		}
		if (effectTime != 0.0f) {
			drawEffect();
		}



		////移動とaxisの同期(移動の最中に斜面対策をしたり、攻撃方向を定めたり)
		btVector3 moveDirection(0, 0, 0);
		btVector3 upDirection(0, 0, 0);
		float speed = 0.1;
		float jumpspeed = 0.8;
		if (KeyW.pressed()) {
			moveDirection += btVector3(0, 0, 1);  // 前進
			attachdirection = 0;
			bulletsphere->getRigidBody()->setLinearVelocity(btVector3(0, bulletsphere->getRigidBody()->getLinearVelocity().getY(), 0));
		}
		if (KeyS.pressed()) {
			moveDirection += btVector3(0, 0, -1);   // 後退
			attachdirection = 4;
			bulletsphere->getRigidBody()->setLinearVelocity(btVector3(0, bulletsphere->getRigidBody()->getLinearVelocity().getY(), 0));
		}
		if (KeyA.pressed()) {
			moveDirection += btVector3(-1, 0, 0);  // 左へ
			attachdirection = 6;
			bulletsphere->getRigidBody()->setLinearVelocity(btVector3(0, bulletsphere->getRigidBody()->getLinearVelocity().getY(), 0));
		}
		if (KeyD.pressed()) {
			moveDirection += btVector3(1, 0, 0);   // 右へ
			attachdirection = 2;
			bulletsphere->getRigidBody()->setLinearVelocity(btVector3(0, bulletsphere->getRigidBody()->getLinearVelocity().getY(), 0));
		}

		//同時押し
		if (KeyA.pressed() && KeyW.pressed()) { attachdirection = 7; }
		if (KeyA.pressed() && KeyS.pressed()) { attachdirection = 5; }
		if (KeyD.pressed() && KeyW.pressed()) { attachdirection = 1; }
		if (KeyD.pressed() && KeyS.pressed()) { attachdirection = 3; }
		// 入力に応じて移動方向を正規化し、所望の速度を掛ける
		if (moveDirection != btVector3(0, 0, 0)) {
			moveDirection.normalize();
			moveDirection *= speed;
		}
		moveDirection.setY(bulletsphere->getRigidBody()->getLinearVelocity().getY());
		btTransform newTransform;
		newTransform.setIdentity();
		btVector3 btPos = bulletsphere->getRigidBody()->getWorldTransform().getOrigin();
		s3d::Vec3 position(btPos.x(), btPos.y(), btPos.z());
		axis = axis3D(position.x, position.y, position.z);
		newTransform.setOrigin(btVector3(position.x + moveDirection.getX(), position.y, position.z + moveDirection.getZ())); // 新しい位置を指定
		bulletsphere->getRigidBody()->setCenterOfMassTransform(newTransform);
		////移動とaxisの同期(移動の最中に斜面対策をしたり、攻撃方向を定めたり)



		//2段ジャンプ
		if (KeySpace.down() && left_jump_num > 0) {
			left_jump_num -= 1;
			upDirection += btVector3(0, 20, 0);
		}
		//着地したら残りジャンプ数リセット
		if (GrounHandle(Boxes) && left_jump_num < 2) {
			left_jump_num = max_jump;
			vy = 0;
		}
		//2弾ジャンプ時に速さを上げる
		if (upDirection != btVector3(0, 0, 0)) {
			upDirection.setX(bulletsphere->getRigidBody()->getLinearVelocity().getX());
			upDirection.setZ(bulletsphere->getRigidBody()->getLinearVelocity().getZ());
			bulletsphere->getRigidBody()->setLinearVelocity(upDirection);
		}


		if (KeyC.down()) {//回復アイテム使用
			if (healitemnum > 0) {
				Friend->HP += 50;
				if (Friend->HP > 100) { Friend->HP = 100; }
				healaudio.play();
				healitemnum--;
			}
		}

		if (KeyX.down() && !bombstate) {//爆破アイテム使用
			if (bombitemnum > 0) {
				bombstate = true;
				bombaudio.play();
				bombitemnum--;
			}
		}
		if (bombstate) {//爆発状態なら
			//どんどん爆風が広がる
			explosion = Sphere{ axis.x,axis.y,axis.z,50 / bomblife }.draw(explosiontexture);
			bomblife -= 2;



			if (bomblife < 10) {//とあるところを超えると爆発ストップ
				bombstate = false;//未爆発状態へ
				explosion = Sphere{ axis.x,axis.y,axis.z,0 }.draw(ColorF(1.0, 0.0, 0.0));
				bomblife = 100;
			}
		}

		display();//表示
		// 移動方向に応じてモーションを描画

		getDrawTexture();
		float shadowY = 0.5;
		if (GrounHandle(Boxes)) {
			shadowY = axis.y - 0.5;
		}
		drawMotion(shadowY);



	}
	void resultdisplay(Array<BoxObject> box) {//リザルト画面の行動のコード(落下反射の繰り返し)
		axis.y = (box[0].axis.y + box[0].height / 2 + Periodic::Jump0_1(1s) * 1) + 0.2;
		display();

		attachdirection = 4;
		getDrawTexture();
		drawMotion(-10000);
	}




	int drawX = 0;
	int drawY = 0;
	int motionTime = 0;
	int sequence[4] = { 0, 1, 2, 1 };


	void drawMotion(float shadowY) {
		//btVector3 btPos = bulletsphere->getRigidBody()->getWorldTransform().getOrigin();
		Vec3 position(axis.x, axis.y + 0.4, axis.z);
		Vec3 shadowPos(axis.x, shadowY, axis.z);
		const ScopedRenderStates3D sampler{ SamplerState::ClampNearest, BlendState::OpaqueAlphaToCoverage };
		spriteMesh.draw(position, Quaternion::RotateY(0), playerTexture(sequence[drawX] * 32, drawY * 32, 32, 32));
		shadowMesh.draw(shadowPos, Quaternion::RotateY(0), ColorF{ 0, 0, 0 });
	}

	void drawEffect() {
		const ScopedRenderStates3D sampler{ SamplerState::ClampNearest, BlendState::OpaqueAlphaToCoverage };
		effectTime += Scene::DeltaTime();
		if (effectTime < 0.15f) {
			Vec3 position;
			if (attachdirection == 0) {
				position = Vec3{ axis.x, axis.y + 0.4, axis.z + 2 };
				effectMesh.draw(position, Quaternion::RotateY(0), effectTexture);
			}
			else if (attachdirection == 4) {
				position = Vec3{ axis.x, axis.y + 0.4, axis.z - 2 };
				effectMesh.draw(position, Quaternion::RotateY(0), effectTexture.mirrored());
			}
			else if (attachdirection >= 1 && attachdirection <= 3) {
				position = Vec3{ axis.x + 2, axis.y + 0.4, axis.z };
				effectMesh.draw(position, Quaternion::RotateY(0), effectTexture);
			}
			else {
				position = Vec3{ axis.x - 2, axis.y + 0.4, axis.z };
				effectMesh.draw(position, Quaternion::RotateY(0), effectTexture.mirrored());
			}
		}
		else {
			effectTime = 0.0f;
		}
	}


	void getDrawTexture() {

		if (attachdirection == 0) {
			if (drawY == 3) {
				drawX = (motionTime / 20 + 1) % 4;
				motionTime += 1;
			}
			else {
				drawY = 3;
				motionTime = 0;
			}
		}
		else if (attachdirection == 4) {
			if (drawY == 0) {
				drawX = (motionTime / 20 + 1) % 4;
				motionTime += 1;
			}
			else {
				drawY = 0;
				motionTime = 0;
			}
		}
		else if (attachdirection >= 1 && attachdirection <= 3) {
			if (drawY == 2) {
				drawX = (motionTime / 20 + 1) % 4;
				motionTime += 1;
			}
			else {
				drawY = 2;
				motionTime = 0;
			}
		}
		else {
			if (drawY == 1) {
				drawX = (motionTime / 20 + 1) % 4;
				motionTime += 1;
			}
			else {
				drawY = 1;
				motionTime = 0;
			}
		}
	}
};





class ItemObject :public BoxObject {//アイテムオブジェクト
public:
	ItemObject(axis3D axis, float width = 1.0f, float height = 1.0f, float depth = 1.0f) :BoxObject(axis, width, height, depth) {
		color = ColorF(1.0, 1.0, 0.0, 1.0);
		move_angle = 0.5f;
		can_use = true;//使用可能（falseだと画面から消えて、座標を通っても拾えない）
	}
	float move_angle;
	Mesh shadowMesh{ MeshData::Disc(0.5) };
	bool can_use;
	int type = -1;//回復か、爆弾か
	Texture kusuritexture{ Image{ U"example/texture/kusuri.png" } };
	Texture bombtexture{ Image{ U"example/texture/bomb.png" } };
	Model coin{ U"example/obj/crystal2.obj" };
	void Run(PlayerObject* Player) {
		shadowMesh.draw(Vec3{ axis.x,axis.y - height / 2,axis.z }, Quaternion::RotateY(0), ColorF{ 0, 0, 0 });
		angle += move_angle * radian;//回転させる
		if (angle > pi * 2) { angle -= pi * 2; };//高制限つけないと壊れる
		rotation = Quaternion::RotateY(angle);

		if (Player->shape.intersects(shape)) {//プレイヤーが触れたときにアイテムが増える
			can_use = false;
			if (type == 0) { Player->healitemnum++; }
			else if (type == 1) { Player->bombitemnum++; }
			else if (type == 2) { Player->Score += 200; }
		}
		if (can_use) {
			if (type == 0) { display(kusuritexture); }
			else if (type == 1) { display(bombtexture); }
			else if (type == 2) { display(); coin.draw(Vec3{ axis.x,axis.y,axis.z }, rotation); }
		}
	}
};

class HealItemObject :public ItemObject {//typeだけ新しくつける
public:
	HealItemObject(axis3D axis) :ItemObject(axis) {
		type = 0;
	}

};

class BombItemObject :public ItemObject {//typeだけ新しくつける
public:

	BombItemObject(axis3D axis) :ItemObject(axis) {
		type = 1;
	}

};
class CoinItemObject :public ItemObject {//typeだけ新しくつける
public:

	CoinItemObject(axis3D axis) :ItemObject(axis) {
		type = 2;
	}

};




class ItemManager {//アイテムのスポーンをしてくれる
public:
	ItemManager(PhysicsWorld* physicsworld) {
		this->physicsworld = physicsworld;
	}
	Array<ItemObject> itemarray;//ここにアイテムを入れていく
	float move_angle;//アイテムが回転するので
	bool can_use;//アイテムが使えるかどうか
	PhysicsWorld* physicsworld;
	Audio audio{ U"example/shot.mp3" };//取得音
	Audio coinaudio{ U"example/getcoin.mp3" };//取得音
	void spawn(axis3D axis) {//指定座標に相手を生成して配列に入れる

		int x = rand() % 100;
		if (x <= 15) {
			HealItemObject item = HealItemObject(axis);
			item.bulletBox = new PhysicsObject(item.boxBox, btVector3(axis.x, axis.y, axis.z), 1);
			physicsworld->addObject(item.bulletBox, 1);

			itemarray << item;
		}
		else if (x > 15 && x <= 30) {
			BombItemObject item = BombItemObject(axis);
			item.bulletBox = new PhysicsObject(item.boxBox, btVector3(axis.x, axis.y, axis.z), 1);
			itemarray << item;
			physicsworld->addObject(item.bulletBox, 1);
		}
		else if (x > 30 && x <= 99) {
			CoinItemObject item = CoinItemObject(axis);
			item.bulletBox = new PhysicsObject(item.boxBox, btVector3(axis.x, axis.y, axis.z), 1);
			itemarray << item;
			physicsworld->addObject(item.bulletBox, 1);
		}

	}
	void draw(PlayerObject* Player) {//描画は回転しながら表示
		for (int i = 0; i < itemarray.size(); i++) {
			btVector3 position = itemarray[i].bulletBox->getRigidBody()->getWorldTransform().getOrigin();
			itemarray[i].axis = axis3D{ (float)position.getX(),(float)position.getY(),(float)position.getZ() };
			itemarray[i].Run(Player);
			if (itemarray[i].shape.intersects(Player->shape)) {
				if (itemarray[i].type == 2) { coinaudio.play(); }//取得音
				else { audio.play(); }

				itemarray.erase(itemarray.begin() + i); // プレイヤーが触れたら配列から削除
				// 音声ファイルを読み込んで Audio を作成する

			}
		}
	}
	void reset() {//resetでは配列を完全初期化
		itemarray = Array<ItemObject>();
	}
};



class TreasureBoxObject :public BoxObject {//宝箱（目の前でエンターを推せばアイテムがでる）を実装したが使わなそうなのでとりまほったらかし
public:
	TreasureBoxObject(axis3D axis, float width, float height, float depth) :BoxObject(axis, width, height, depth) {
		color = ColorF(1.0, 0.0, 1.0, 1.0);
		can_use = true;
		item.can_use = false;
	}
	bool can_use;
	float atarihantei_range = 2.0f;
	ItemObject item = ItemObject(axis, width - atarihantei_range, height - atarihantei_range, depth - atarihantei_range);

	Box atarihantei = Box(axis.x, axis.y, axis.z, width + atarihantei_range, height + atarihantei_range, depth + atarihantei_range);

	void Run(PlayerObject Player) {
		rotation = Quaternion::RotateY(angle);
		if (Player.shape.intersects(atarihantei) && KeyEnter.down()) { can_use = false; item.can_use = true; }

		if (can_use) { display(); }
		else { item.Run(&Player); }
	}
};




class EnemyObject : public SphereObject {//敵
public:
	float HP = 100.0f;//HP
	float maxHP = 100.0f;//HP最大値
	// BoxObjectのコンストラクタ
	EnemyObject(axis3D axis, float radius) :SphereObject(axis, radius) {
		movexz = 0.12;
		color = ColorF(1.0, 0, 1.0);
	}

	int max_jump = 2;
	int left_jump_num = max_jump;
	float movexz;
	int direction = 0;
	float kagenti = 40.0 / 2500.0;
	int direction_change = 0;
	float speed = rand() % 200 / 2500.0 + kagenti;
	//攻撃ヒット音
	Audio hitaudio{ U"example/hit.mp3" };

	int attachdirection = 0;//移動

	Texture friendTexture{ Image {U"example/texture/EnemyTexture.png"} }; // 移動モーション
	Mesh spriteMesh{ MeshData::TwoSidedPlane(SizeF{ 1.5, 1.5 }).rotate(Quaternion::RotateX(-90_deg)) };



	void Walljump() {//ジャンプについての動きをまだ決めていない

	}


	void reset(int* Enemy_seed) {//リセット時は座標をランダムに決定
		*Enemy_seed += 100;
		std::srand(*Enemy_seed);
		speed = rand() % 200 / 2500.0 + kagenti;
		HP = 100;
		axis.x = rand() % 100 - 50;
		axis.z = rand() % 100 - 50;
		axis.y = 35.0f;

		//rigidbodyは上のランダム値の場所へ移動
		btTransform newTransform;
		newTransform.setIdentity();
		newTransform.setOrigin(btVector3(axis.x, axis.y, axis.z)); // 新しい位置を指定
		bulletsphere->getRigidBody()->setCenterOfMassTransform(newTransform);
		vy = -9.8;
		bulletsphere->getRigidBody()->setActivationState(DISABLE_DEACTIVATION);
	}

	//行動決定
	void Run(PlayerObject* Player, Array<BoxObject>Boxes, FriendObject* Friend, Array<EnemyObject> Enemies, int* Enemy_seed, ItemManager* itemmanager) {

		bulletsphere->getRigidBody()->setActivationState(DISABLE_DEACTIVATION);
		bulletsphere->getRigidBody()->setLinearVelocity(btVector3(0, bulletsphere->getRigidBody()->getLinearVelocity().getY(), 0));//斜面対策

		//HP表示
		Box{ axis.x,axis.y + 1,axis.z ,1.5,0.25,0.25 }.draw(ColorF{ 1.0,0.0,0.0 });
		Box{ axis.x - (1.5 - 1.5 * HP / maxHP) / 2.0,axis.y + 1,axis.z ,1.5 * HP / maxHP,0.26,0.26 }.draw(ColorF{ 0.0,1.0,0.0 });


		float substractaxissize = axissize(subaxis(axis, Friend->axis));//Friendへ近づくように
		int randomteisuu;//皆密集しないように動きにランダム性を加えて集まらないようにする
		float virtualx;
		float virtualz;
		axis3D move;
		if (substractaxissize < radius * 10) {//近ければ座標を追っていく
			move = subaxis(axis, Friend->axis);
		}
		else {//遠ければFriendの座標付近へ向かう
			randomteisuu = 10;
			virtualx = Friend->axis.x + rand() % randomteisuu;
			virtualz = Friend->axis.z + rand() % randomteisuu;
			axis3D virtual_friend_axis = axis3D{ virtualx ,Friend->axis.y,virtualz };
			move = subaxis(axis, virtual_friend_axis);
		}

		axis3D move_enemy = { 0.0,0.0,0.0 };//敵同士が近づかないように遠ざけさせるための値
		for (int i = 0; i < Enemies.size(); i++)
		{
			move_enemy = addaxis(subaxis(Enemies[i].axis, axis), move_enemy);
		}

		int randomteisuuvelocity = 10;
		int randomvelocity = rand() % randomteisuuvelocity + 5;
		//敵と遠ざかるための移動
		move_enemy = multifloataxis(normalizeaxis(move_enemy), Scene::DeltaTime() * 1.0);


		//Friendを追っかけるように移動
		move = multifloataxis(normalizeaxis(move), Scene::DeltaTime() * randomvelocity);


		//方向を調べていく
		bool direction0 = false;
		bool direction1 = false;
		bool direction2 = false;
		bool direction3 = false;
		bool direction4 = false;
		if (move.x + move_enemy.x > 0) { direction0 = true; }
		else if (move.x + move_enemy.x < 0) { direction1 = true; }

		if (move.z + move_enemy.x > 0) { direction2 = true; }
		else if (move.z + move_enemy.x < 0) { direction3 = true; }

		if (move.y + move_enemy.y < 0) { direction4 = true; }

		//移動と座標の同期
		btVector3 moveDirection(0, 0, 0);
		btVector3 upDirection(0, 0, 0);
		*Enemy_seed += 100;
		std::srand(*Enemy_seed);

		float jumpspeed = 0.8;

		if (direction0) {
			moveDirection += btVector3(1, 0, 0);  // 前進
		}
		if (direction1) {
			moveDirection += btVector3(-1, 0, 0);   // 後退
		}
		if (direction2) {
			moveDirection += btVector3(0, 0, 1);  // 左へ
		}
		if (direction3) {
			moveDirection += btVector3(0, 0, -1);   // 右へ
		}

		// 入力に応じて移動方向を正規化し、所望の速度を掛ける
		if (moveDirection != btVector3(0, 0, 0)) {
			moveDirection.normalize();
			moveDirection *= speed;
			moveDirection.setY(bulletsphere->getRigidBody()->getLinearVelocity().getY());
			// btbulletsphere->getRigidBody()の速度を設定
			//bulletsphere->getRigidBody()->setLinearVelocity(moveDirection);//ここをelse下の4行で処理
		}

		for (int i = 0; i < Boxes.size() - 4; i++)
		{
			if (shape.intersects(Boxes[i].atarihanteishape) && !(axis.y >= Boxes[i].axis.y + Boxes[i].height / 2)) {
				btVector3 upDirection = btVector3(0, 20, 0);
				upDirection.setX(bulletsphere->getRigidBody()->getLinearVelocity().getX());
				upDirection.setZ(bulletsphere->getRigidBody()->getLinearVelocity().getZ());
				bulletsphere->getRigidBody()->setLinearVelocity(upDirection);
				break;
			}
		}

		//moveDirection.setY(bulletsphere->getRigidBody()->getLinearVelocity().getY());
		btTransform newTransform;
		newTransform.setIdentity();
		btVector3 btPos = bulletsphere->getRigidBody()->getWorldTransform().getOrigin();
		s3d::Vec3 position(btPos.x(), btPos.y(), btPos.z());
		axis = axis3D(position.x, position.y, position.z);
		newTransform.setOrigin(btVector3(position.x + moveDirection.getX(), position.y, position.z + moveDirection.getZ())); // 新しい位置を指定
		bulletsphere->getRigidBody()->setCenterOfMassTransform(newTransform);






		//Friendに触れたらFriendの体力減少
		if (shape.intersects(Friend->shape)) {
			Friend->HP -= 0.325;//0.5;
		}

		//プレイヤーの攻撃が当たるとダメージ
		if (shape.intersects(Player->attack)) {

			HP -= 20.0;
			hitaudio.play();
			axis3D move = subaxis(Player->axis, axis);
			//方向を調べていく
			bool direction0 = false;
			bool direction1 = false;
			bool direction2 = false;
			bool direction3 = false;
			if (move.x > 0) { direction0 = true; }
			else if (move.x < 0) { direction1 = true; }

			if (move.z > 0) { direction2 = true; }
			else if (move.z < 0) { direction3 = true; }

			//移動と座標の同期
			btVector3 moveDirection(0, 0, 0);

			if (direction0) {
				moveDirection += btVector3(1, 0, 0);  // 前進
			}
			if (direction1) {
				moveDirection += btVector3(-1, 0, 0);   // 後退
			}
			if (direction2) {
				moveDirection += btVector3(0, 0, 1);  // 左へ
			}
			if (direction3) {
				moveDirection += btVector3(0, 0, -1);   // 右へ
			}
			moveDirection.normalize();
			moveDirection *= 2.0;
			btTransform newTransform;
			newTransform.setIdentity();
			btVector3 btPos = bulletsphere->getRigidBody()->getWorldTransform().getOrigin();
			s3d::Vec3 position(btPos.x(), btPos.y(), btPos.z());
			axis = axis3D(position.x, position.y, position.z);
			newTransform.setOrigin(btVector3(position.x + moveDirection.getX(), position.y, position.z + moveDirection.getZ())); // 新しい位置を指定
			bulletsphere->getRigidBody()->setCenterOfMassTransform(newTransform);
			moveDirection.setY(bulletsphere->getRigidBody()->getLinearVelocity().getY());
			btVector3 upDirection = btVector3(0, 10, 0);
			upDirection.setX(bulletsphere->getRigidBody()->getLinearVelocity().getX());
			upDirection.setZ(bulletsphere->getRigidBody()->getLinearVelocity().getZ());
			bulletsphere->getRigidBody()->setLinearVelocity(upDirection);

		}
		else if (shape.intersects(Player->explosion)) { HP -= 100; }//爆発なら即死

		//死んだらプレイヤーのスコアが上がって、初期化
		if (HP <= 0.0f) {
			Player->Score += 100;
			int x = rand() % 1;
			axis3D submove = subaxis(Player->axis, axis);
			axis.x += submove.x;
			axis.z += submove.z;
			if (x == 0) { itemmanager->spawn(axis); }
			reset(Enemy_seed);

		}









		//移動と座標の同期（終わり）

		display();//表示

		getDrawTexture(moveDirection);
		drawMotion();


		if (GrounHandle(Boxes)) {
			left_jump_num = max_jump;
			vy = 0;
		}//着地したら残りジャンプ数リセット
	}

	int drawX = 0;
	int drawY = 0;
	int motionTime = 0;
	int sequence[4] = { 0, 1, 2, 1 };
	void drawMotion() {
		//btVector3 btPos = bulletsphere->getRigidBody()->getWorldTransform().getOrigin();
		Vec3 position(axis.x, axis.y + 0.5, axis.z);
		const ScopedRenderStates3D sampler{ SamplerState::ClampNearest, BlendState::OpaqueAlphaToCoverage };
		spriteMesh.draw(position, Quaternion::RotateY(0), friendTexture(sequence[drawX] * 32, drawY * 32, 32, 32));
	}

	void getDrawTexture(btVector3 direction) {
		if (direction.x() > 0) {
			attachdirection = 2;
		}
		else if (direction.x() < 0) {
			attachdirection = 6;
		}

		if (attachdirection >= 1 && attachdirection <= 3) {
			if (drawY == 2) {
				drawX = (motionTime / 20 + 1) % 4;
				motionTime += 1;
			}
			else {
				drawY = 2;
				motionTime = 0;
			}
		}
		else {
			if (drawY == 1) {
				drawX = (motionTime / 20 + 1) % 4;
				motionTime += 1;
			}
			else {
				drawY = 1;
				motionTime = 0;
			}
		}
	}

};
