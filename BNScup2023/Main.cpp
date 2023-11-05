
#include <btBulletDynamicsCommon.h>
#include <Siv3D.hpp>
#include <atltrace.h>
#include "Siv3DPhysics3D.h"
#include<siv3D.hpp>
#include"Object.h"
#include"Shapes.h"
#include"Character.h"
#include"Figure.h"




struct Boxinfo
{
	axis3D axis;//座標
	axis3D size;//大きさ
};//ステージ作るための値だけ格納するやつ

struct Treeinfo
{
	axis3D axis;//座標
	float scale;//木の高さ（でかすぎると破壊されるので2未満に）
};//木作るための値だけ格納するやつ

struct Coneinfo {
	axis3D axis;//座標
	float radius;//木の高さ（でかすぎると破壊されるので2未満に）
	float top;
};

void transport_stage_select(PlayerObject* Player) {
	btTransform newTransform;
	newTransform.setIdentity();
	newTransform.setOrigin(btVector3(0,0,-30)); // 新しい位置を指定
	Player->bulletsphere->getRigidBody()->setCenterOfMassTransform(newTransform);
	s3d::Vec3 position(0, 0, -30);
	Player->axis = axis3D(position.x, position.y, position.z);

};//ステージセレクト場面来た時に行う初期化(プレイヤーの座標の初期化のみ)



void transport_stage11(PlayerObject* Player, FriendObject* Friend,Array<EnemyObject>* Enemies,Array<Treeinfo> Enemyinfo,ItemManager *itemmanager,PhysicsWorld physicsworld) {
	btTransform newTransform;
	newTransform.setIdentity();
	newTransform.setOrigin(btVector3(0, 0, -30)); // 新しい位置を指定
	Player->bulletsphere->getRigidBody()->setCenterOfMassTransform(newTransform);
	s3d::Vec3 position(0, 0, -30);
	Player->axis = axis3D(position.x, position.y, position.z);
	Player->bombitemnum = 0;
	Player->healitemnum = 0;
	//ここまでプレイヤーの初期化

	Friend->HP = Friend->maxHP;
	newTransform.setIdentity();
	int x = rand() % 30;
	int z = rand() % 30;
	newTransform.setOrigin(btVector3(x, 0, z)); // 新しい位置を指定
	Friend->bulletsphere->getRigidBody()->setCenterOfMassTransform(newTransform);
	s3d::Vec3 position2(x, 0, z);
	Friend->axis = axis3D(position2.x, position2.y, position2.z);
	//ここまでFriendの座標初期化

	itemmanager->reset();
	//アイテムマネージャーの初期化

	*Enemies = Array<EnemyObject>();
	for (int i = 0; i < Enemyinfo.size(); i++)
	{
		EnemyObject x = EnemyObject(Enemyinfo[i].axis, Enemyinfo[i].scale);
		physicsworld.addObject(x.bulletsphere,1);
		Enemies->push_back(x);//(ポインタ的に<<使えなかった（）)
	}
	//敵の初期化




};//プレイヤーの座標の初期化+仲間のHP初期化+敵の初期化+味方の初期化,アイテムのリセット



void Main()
{
	// ウインドウ
	int window_width = 1920;
	int window_length = 1080;
	Window::Resize(window_width, window_length);



	// 背景色
	const ColorF title = ColorF{ 0.4, 0.6, 0.8 }.removeSRGBCurve();
	const ColorF backgroundColor = ColorF{ 0.4, 0.6, 0.8 }.removeSRGBCurve();



	//使うテクスチャ
	const Texture grasstexture{ U"example/texture/grass.jpg", TextureDesc::MippedSRGB };
	const Texture titletexture{ Image{ U"example/texture/titlekai.png" } };
	const Texture startbuttontexture{ Image{ U"example/texture/StartButton.png" } };
	const Texture rocktexture{ Image{ U"example/texture/rock.jpeg" } };
	Texture kusuritexture{ Image{ U"example/texture/kusuri.png" } };
	Texture bombtexture{ Image{ U"example/texture/bomb.png" } };
	Texture tiletexture{ Image{ U"example/texture/tile.jpg" } };
	Texture doukututexture{ Image{ U"example/texture/doukutu.jpg" } };

	//おまじない
	const MSRenderTexture renderTexture{ Scene::Size(), TextureFormat::R8G8B8A8_Unorm_SRGB, HasDepth::Yes };



	// 3D シーンのデバッグ用カメラ
	// 縦方向の視野角 30°, カメラの位置 (10, 16, -32)
	// 前後移動: [W][S], 左右移動: [A][D], 上下移動: [E][X], 注視点移動: アローキー, 加速: [Shift][Ctrl]
	DebugCamera3D camera{ renderTexture.size(), 30_deg, Vec3{ 10, 16, -64 } };



	//プレイヤー(ステージセレクト用とプレイ用で分けてる(1つの物体につき2つ以上のワールドに入れるなと怒られた))
	PlayerObject Player_stage = PlayerObject(axis3D(0, 0, -30), 0.5);
	PlayerObject Player = PlayerObject(axis3D(0, 0, -30), 0.5);



	//宝箱(使わないかも)
	TreasureBoxObject TreasureBox = TreasureBoxObject(axis3D(-8.0, 2.001, -5.0), 3.0, 3.0, 3.0);



	//ステージセレクトの情報（始まり）(透明な外壁必要かも)
	//足場と床の情報

	PhysicsWorld physicsworld_stage_select;
	physicsworld_stage_select.addObject(Player_stage.bulletsphere,1);//物理演算世界


	float field_range0 = 86;//地面の大きさ

	//足場と床（ここに座標、大きさを追加すれば、勝手に足場と床が増える）
	Array<Boxinfo> box_informations0 = { {axis3D(-15.0, 0.0, 0.0), axis3D(7.0,13.0,7.0)}, {axis3D(15.0, 0.0, 0.0), axis3D(7.0,13.0,7.0)}, {axis3D(0.0, 0.0, 0.0), axis3D(7.0,13.0,7.0)},
		{axis3D(0.0, 0.0, 0.0), axis3D(field_range0, 0.5, field_range0)} };
	int box_num0 = box_informations0.size();


	//当たり判定用の配列の初期化
	Array<BoxObject> Boxes0;
	for (int i = 0; i < box_num0; i++) {
		BoxObject x= BoxObject(box_informations0[i].axis, (float)box_informations0[i].size.x, (float)box_informations0[i].size.y, (float)box_informations0[i].size.z);
		physicsworld_stage_select.addObject(x.bulletBox,2);
		Boxes0 << x;
	}

	//一番左の台の上においてる。触れるとステージ1へ
	BoxObject tostage11 = BoxObject(axis3D(-15.0, 0.0, 0.0), 3.5f, 13.0f, 3.5f);

	//テクスチャ表示させるもの達を格納
	Array<BoxObject> TextureBoxes0;
	for (int i = 0; i < box_num0; i++) {
		TextureBoxes0 << Boxes0[i];
	}
	//ステージセレクトの情報（終わり）


	//ステージ1の設定(始まり)
	PhysicsWorld physicsworld1;//地面の大きさ
	physicsworld1.addObject(Player.bulletsphere,1);//物理演算世界

	float field_range11 = 128;//地面の大きさ

	//足場と床の情報（ここに座標、大きさを追加すれば、勝手に足場と床が増える）
	Array<Boxinfo> box_informations11 = { {axis3D(0.0, -1.0, 0.0), axis3D(field_range11, 2.0, field_range11)},
										{axis3D(-38.0, 0.0, 38.0), axis3D(52.0,5.0,50.0)},{axis3D(-45.0, 0.0, 60.0), axis3D(27.0,12.0,25.0)}
										,{axis3D(50.0, 0.0, 60.0), axis3D(30.0,6.0,30.0)}
	};
	int box_num11 = box_informations11.size();


	//岩の情報(座標と大きさを加えるだけで配置可能)
	Array<Boxinfo> rock_box_informations11 = { {axis3D(0.0, 0.0, 0.0), axis3D(3.5,6.5,3.5)} };
	int rock_box_num11 = rock_box_informations11.size();




	//木の情報（座標と高さを入れるだけで木が生える）
	Array<Treeinfo> Treeinformations11 = { { axis3D(-20.0, 0.0, -25.0),2.5},{ axis3D(-30.0, 0.0, -20.0),2.5},{ axis3D(-60.0, 0.0, -20.0),2.5},
		{ axis3D(-45.0, 0.0, -45.0),2.5},{ axis3D(10.0, 0.0, -30.0),2.5},{ axis3D(25.0, 0.0, 0.0),2.5},
		{ axis3D(-25.0, 0.0, -20.0),2.5},{ axis3D(-25.0, 0.0, 5.0),2.5},
	};//座標とスケールをぶち込む。スケールは2以上にしないように調整
	int tree_num11 = Treeinformations11.size();

	//木のあたり判定は幹の部分のみ
	Array<TreeObject> TreeObjects11;
	for (int i = 0; i < tree_num11; i++) {
		TreeObjects11 << TreeObject(Treeinformations11[i].axis, Treeinformations11[i].scale);
	}



	//当たり判定させるやつら、物理計算さえるやつら(始まり)
	Array<BoxObject> Boxes11;
	for (int i = 0; i < box_num11; i++) {//草の足場
		BoxObject x= BoxObject(box_informations11[i].axis, (float)box_informations11[i].size.x, (float)box_informations11[i].size.y, (float)box_informations11[i].size.z);
		Boxes11 << x;
		physicsworld1.addObject(x.bulletBox,2);
	}
	for (int i = 0; i < tree_num11; i++) {//木（の幹）
		BoxObject x= TreeObjects11[i].Treebody;
		Boxes11 << x;
		physicsworld1.addObject(x.bulletBox,2);
	}
	for (int i = 0; i < rock_box_num11; i++) {//岩の足場
		BoxObject x= BoxObject(rock_box_informations11[i].axis, (float)rock_box_informations11[i].size.x, (float)rock_box_informations11[i].size.y, (float)rock_box_informations11[i].size.z);
		Boxes11 << x;
		physicsworld1.addObject(x.bulletBox,2);
	}

	//透明な壁(ステージ外の壁)（4方向）
	float atumi = 100.0;
	float atumiidou = 45.0;
	Array<Boxinfo> Wall = { { axis3D(0.0, 0.0, field_range11 / 2.0 + 4.0 + atumiidou), axis3D(field_range11,100,atumi) }, { axis3D(-field_range11 / 2.0 - atumiidou - 4, 0.0, 0.0),
		axis3D(atumi,100.0, field_range11) }, { axis3D(field_range11 / 2.0 + atumiidou + 4.0, 0.0, 0.0), axis3D(atumi,100.0, field_range11) },
		{ axis3D(0.0, 0.0, -field_range11 / 2.0 - 4.0 - atumiidou), axis3D(field_range11,100,atumi) }};
	for (int i = 0; i < 4; i++) {
		BoxObject x= BoxObject(Wall[i].axis, (float)Wall[i].size.x, (float)Wall[i].size.y, (float)Wall[i].size.z);
		Boxes11 << x;
		physicsworld1.addObject(x.bulletBox,2);
	}


	Array<Coneinfo> Coneinformations = { {axis3D{3.5, 0.0, 0}, 10, 6.5 },{axis3D{40, 0.0, -20}, 10, 8.4 } };
	Array<ConeObject> Cones11;
	{
		for (int i = 0; i < Coneinformations.size(); i++)
		{
			// 円錐を追加してワールドに追加
			ConeObject x = ConeObject(Coneinformations[i].axis,Coneinformations[i].radius, Coneinformations[i].top);
			Cones11 << x;
			physicsworld1.addObject(x.bulletcone, 2);
		}


	}

	//当たり判定させるやつら、物理計算さえるやつら(終わり)


	//テクスチャ組を格納(始まり)
	Array<BoxObject> GrassTextureBoxes11;
	for (int i = 0; i < box_num11 + tree_num11; i++) {
		GrassTextureBoxes11 << Boxes11[i];
	}
	Array<BoxObject> RockTextureBoxes11;
	for (int i = box_num11 + tree_num11; i < box_num11 + tree_num11 + 1; i++) {
		RockTextureBoxes11 << Boxes11[i];
	}
	//テクスチャ組を格納(終わり)

	//味方NPCの宣言と物理演算参加
	FriendObject Friend = FriendObject(axis3D(2.39, 1.0, -24), 0.5);
	physicsworld1.addObject(Friend.bulletsphere,1);


	//敵の情報（ここに値を入れるだけで敵増加）(使う値が同じだったのでTreeinfoを使用)
	Array<Treeinfo> Enemyinfo = { {axis3D(2.39, 1.0, -24), 0.5}, {axis3D(2.39, 1.0, -34), 0.5},{axis3D(22.39, 1.0, 15), 0.5}, {axis3D(42.39, 1.0, -34), 0.5} ,{axis3D(-20.39, 1.0, 34), 0.5},
	{axis3D(15.39, 1.0, 14), 0.5},{axis3D(-14.39, 1.0, -25), 0.5},{axis3D(35.39, 1.0, 35), 0.5 } ,{axis3D(-25, 1.0, -25), 0.5 },{axis3D(50, 1.0, 50), 0.5 } ,
	{axis3D(-20, 1.0, -10), 0.5 },{axis3D(30, 1.0, -10), 0.5 },{axis3D(-40, 1.0, 40), 0.5 },{axis3D(-40, 1.0, 0), 0.5 }
	,{axis3D(30, 1.0, -10), 0.5 },{axis3D(-40, 1.0, 40), 0.5 },{axis3D(-40, 1.0, 0), 0.5 }
	};

	//敵の宣言と物理演算参加
	Array<EnemyObject> Enemies;
	for (int i = 0; i < Enemyinfo.size(); i++) {
		EnemyObject x= EnemyObject(Enemyinfo[i].axis, Enemyinfo[i].scale);
		physicsworld1.addObject(x.bulletsphere,1);
		Enemies << x;
	}

	//ステージ1の設定（終わり）

	int Scene_num = 0;//ステージを切り替える番号



	int Enemy_seed = 0;//Enemyのrand変更用

	

	//カメラの初期位置
	Vec3 focusPosition = Vec3{ Player.axis.x, box_num11 + tree_num11, Player.axis.z };
	Vec3 eyePosition = Vec3{ Player.axis.x, box_num11 + tree_num11 + 5, Player.axis.z - 40 };

	//アイテムスポーン役
	ItemManager itemmanager = ItemManager(&physicsworld1);

	

	//フォント集
	Font font = Font{ 50 };
	Font Resultfont = Font{ 150 };
	Font Resultminifont = Font{ 40 };



	Audio titleaudio{ U"example/titleBGM.mp3" };//取得音
	Audio resultaudio{ U"example/result.mp3" };//取得音
	Audio battleaudio{ U"example/battleBGM.mp3" };//取得音

	// メインループ
	while (System::Update())
	{

		// 3D シーンにカメラを設定
		Graphics3D::SetCameraTransform(camera);



		if (Scene_num == 0) {//タイトル画面
			titleaudio.play();
			
			// renderTexture を resolve する前に 3D 描画を実行する
			Graphics3D::Flush();

			// マルチサンプル・テクスチャのリゾルブ
			renderTexture.resolve();

			// リニアレンダリングされた renderTexture をシーンに転送
			Shader::LinearToScreen(renderTexture);

			//ボタンの長さ
			int button_width = 300;


			if (SimpleGUI::Button(U"Red", Vec2{ 960 - startbuttontexture.width() / 4, 540 + startbuttontexture.height() }, startbuttontexture.width() / 2.0))
			{//ボタン押されたらステージセレクトへ
				Scene_num = 1;
				camera = DebugCamera3D{ renderTexture.size(), 30_deg, Vec3{ Player.axis.x, Player.axis.y+5, Player.axis.z - 40 },Vec3{ Player.axis.x, Player.axis.y, Player.axis.z} };
				transport_stage_select(&Player_stage);
				titleaudio.stop();
			}

			titletexture.draw();
			startbuttontexture.draw(960 - startbuttontexture.width() / 2, 540 + startbuttontexture.height() / 2);
		}



		else if (Scene_num == 1) {//ステージセレクト
			//カメラの処理
			focusPosition = Vec3{ Player_stage.axis.x, box_num11 + tree_num11, Player_stage.axis.z };
			if (MouseL.pressed()) {
				// Calculate the angle of rotation based on cursor movement (horizontal rotation)
				double rotationAngle = Cursor::Delta().x / 70.0;

				// Calculate the new camera position relative to the Player_stage
				double camX = eyePosition.x - Player_stage.axis.x;
				double camZ = eyePosition.z - Player_stage.axis.z;

				// Apply rotation to the camera position
				double newCamX = camX * cos(rotationAngle) - camZ * sin(rotationAngle);
				double newCamZ = camX * sin(rotationAngle) + camZ * cos(rotationAngle);

				// Update the camera position based on the new position relative to the Player_stage
				eyePosition.x = Player_stage.axis.x + newCamX;
				eyePosition.z = Player_stage.axis.z + newCamZ;

			}
			else { eyePosition = Vec3{ Player_stage.axis.x, box_num11 + tree_num11 + 5, Player_stage.axis.z - 40 }; }
			camera = DebugCamera3D{ renderTexture.size(), 30_deg, eyePosition, focusPosition, };
			//カメラの処理


			//背景色
			const ScopedRenderTarget3D target{ renderTexture.clear(ColorF(0.0,0.0,0.0)) };

			//プレイヤー操作からの物理演算計算
			Player_stage.PlayerManipulate(Boxes0, &Friend);
			physicsworld_stage_select.stepSimulation(Scene::DeltaTime());

			//テクスチャ表示
			for (int i = 0; i < box_num0; i++) {
				TextureBoxes0[i].display(rocktexture);
			}

			//ステージ移動部分に到着
			if (Player_stage.shape.intersects(tostage11.atarihanteishape)) {
				Scene_num = 2;

				System::Sleep(1000); // ロード時間的な
				transport_stage11(&Player, &Friend,&Enemies,Enemyinfo,&itemmanager,physicsworld1);//ステージ1への初期化処理

			}


			// 3D シーンを 2D シーンに描画
			// renderTexture を resolve する前に 3D 描画を実行する
			Graphics3D::Flush();
			// マルチサンプル・テクスチャのリゾルブ
			renderTexture.resolve();
			// リニアレンダリングされた renderTexture をシーンに転送
			Shader::LinearToScreen(renderTexture);

		}






		else if (Scene_num == 2) {
			battleaudio.play();
			//カメラの初期化
			camera = DebugCamera3D{ renderTexture.size(), 30_deg, Vec3{ Player.axis.x, 10, Player.axis.z - 40 },Vec3{ Player.axis.x,Player.axis.y, Player.axis.z} };



			{
				// renderTexture を背景色で塗りつぶし、
				// renderTexture を 3D 描画のレンダーターゲットに
				const ScopedRenderTarget3D target{ renderTexture.clear(backgroundColor) };

				//プレイヤー操作
				Player.PlayerManipulate(Boxes11, &Friend);
				//味方キャラ操作
				Friend.Run(Boxes11);
				//敵プレイヤー操作
				for (int i = 0; i < Enemies.size(); i++) {
					Enemies[i].Run(&Player, Boxes11, &Friend, Enemies, &Enemy_seed, &itemmanager);
				}


				//物理演算
				physicsworld1.stepSimulation((float)Scene::DeltaTime());
				//physicsworld1.draw();//デバッグ用
				
				//当たり判定時
				for (int i = 0; i < tree_num11; i++) { TreeObjects11[i].display(); }

				//物体表示(キャラ以外)
				for (int i = 0; i < Boxes11.size() -4 ; i++)
				{
					Boxes11[i].display();
				}

				for (int i = 0; i < box_num11; i++) {
					GrassTextureBoxes11[i].display(grasstexture);
				}

				for (int i = 0; i < rock_box_num11; i++) {
					RockTextureBoxes11[i].display(rocktexture);
				}

				for (int i = 0; i < Coneinformations.size(); i++) {
					Cones11[i].display(rocktexture);
				}

				//落ちているアイテムの表示
				itemmanager.draw(&Player);
			}
			//if (Key0.down()) { Friend.HP = 0; }デバッグ用
			if (Friend.HP <= 0) {//味方NPC死亡時
				
				Scene_num = 3;
				itemmanager.reset();//アイテムを全て消去

				//プレイヤーをリザルトの座標へ
				Player.axis = axis3D{ 1.0,1.5,0.0 };
				Player.vy = 0;

			}

			// 3D シーンを 2D シーンに描画+味方体力などの2D描画
			{
				// renderTexture を resolve する前に 3D 描画を実行する
				Graphics3D::Flush();

				// マルチサンプル・テクスチャのリゾルブ
				renderTexture.resolve();

				// リニアレンダリングされた renderTexture をシーンに転送
				Shader::LinearToScreen(renderTexture);

				font(U"Score:").draw(0, 0, ColorF(1.0, 1.0, 1.0));
				font(Player.Score).draw(170, 0, ColorF(1.0, 1.0, 1.0));

				font(U"Friend HP").draw(0, 100, ColorF(1.0, 1.0, 1.0));
				Rect{ 250,130,150,20 }.draw(ColorF(1.0, 0.0, 0.0));
				Rect{ 250,130,150 * (int)Friend.HP / (int)Friend.maxHP,20 }.draw(ColorF(0.0, 0.0, 1.0));


				font(U"Heal Item").draw(1400, 0, ColorF(1.0, 1.0, 1.0));
				kusuritexture.draw(1700, 15);
				font(U"x").draw(1775, 0, ColorF(1.0, 1.0, 1.0));
				font(Player.healitemnum).draw(1800, 0, ColorF(1.0, 1.0, 1.0));

				font(U"Bomb Item").draw(1400, 100, ColorF(1.0, 1.0, 1.0));
				bombtexture.draw(1700, 110);
				font(U"x").draw(1775, 100, ColorF(1.0, 1.0, 1.0));
				font(Player.bombitemnum).draw(1800, 100, ColorF(1.0, 1.0, 1.0));

			}


		}
		else if (Scene_num == 3) {//リザルト画面
			battleaudio.stop();
			resultaudio.play();
			//カメラの初期化
			focusPosition = Vec3{ 0, 0, 0};
			eyePosition = Vec3{ 0, 0, -7}; 
			camera = DebugCamera3D{ renderTexture.size(), 30_deg, eyePosition, focusPosition, };


			//背景塗りつぶし（何か良い画像を募集中）
			const ScopedRenderTarget3D target{ renderTexture.clear(ColorF(0.0,1.0,0.0)) };

			//土台の情報(.objでもいいかも)(当たり判定をやらせるため配列化)
			BoxObject haikei = BoxObject(axis3D(0.0, 0.0, 15.0), 25.0, 15.0, 0.01);
			BoxObject ResultBox = BoxObject(axis3D(1.0, -1.5, 0.0), 2.0, 2.0, 2.0);
			Array<BoxObject> a;
			a << ResultBox;
			ResultBox.color = ColorF(1.0, 0.0, 0.0);

			//土台とプレイヤー表示
			haikei.shape.draw(doukututexture);
			ResultBox.shape.draw(tiletexture);
			Player.resultdisplay(a);

			{
				// renderTexture を resolve する前に 3D 描画を実行する
				Graphics3D::Flush();

				// マルチサンプル・テクスチャのリゾルブ
				renderTexture.resolve();

				// リニアレンダリングされた renderTexture をシーンに転送
				Shader::LinearToScreen(renderTexture);



				Resultfont(U"Your Score is").draw(0, 300, ColorF(1.0, 1.0, 0.0));
				Resultfont(Player.Score).draw(100, 500, ColorF(1.0, 1.0, 0.0));
				Resultminifont(U"To continue the game,please push R").draw(20, 800, ColorF(1.0, 1.0, 0.0));
			}

			if (KeyR.down()) {//Rキーでタイトルへ（好きにキー変えてください）
				Player.Score = 0;
				Scene_num = 0;
				resultaudio.stop();
			}

		}


	}
}
