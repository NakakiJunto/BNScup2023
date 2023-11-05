///-----includes_start-----
#include <btBulletDynamicsCommon.h>
#include <Siv3D.hpp>
#include <atltrace.h>
#include "Siv3DPhysics3D.h"

/// This is a Hello World program for running a basic Bullet physics simulation
void Main()
{
	const Texture grassTexture{ U"GrassTexture.png", TextureDesc::MippedSRGB };
	const Texture characterTexture{ U"example/texture/Player.png", TextureDesc::MippedSRGB };

	// 3D物理ワールドを作成
	PhysicsWorld physicsWorld;

	// 球を追加してワールドに追加
	btCollisionShape* sphereShape = new btSphereShape(1.0);  // 半径1の球
	PhysicsObject* sphere = new PhysicsObject(sphereShape, btVector3(0, 10, 0), 1.0);  // 位置(0,5,0)に設置
	physicsWorld.addObject(sphere);

	btCollisionShape* cylinderShape = new btCylinderShape({2.0, 2.0, 2.0});
	PlayerPhysicsObject* player = new PlayerPhysicsObject(cylinderShape, btVector3(5, 10, 0), 1.0, characterTexture);
	physicsWorld.addObject(player);

	// 地面を作成
	{
		// 平面を追加してワールドに追加
		btCollisionShape* boxShape = new btBoxShape(btVector3(btScalar(100.), btScalar(5.), btScalar(100.)));
		PhysicsObject* ground = new PhysicsObject(boxShape, btVector3(0., 0., 0.), 0., grassTexture);  // 位置(0,5,0)に設置

		physicsWorld.addObject(ground);
	}

	/*
	{
		// 平面を追加してワールドに追加
		btCollisionShape* boxShape = new btBoxShape(btVector3(btScalar(10.), btScalar(50.), btScalar(1.)));  // 
		std::unique_ptr<PhysicsObject> ground = std::make_unique<PhysicsObject>(boxShape, btVector3(0, -56, 0), 0., ColorF{1.0, 0., 0.});  // 位置(0,5,0)に設置
		physicsWorld.addObject(std::move(ground));
	}

	{
		// 平面を追加してワールドに追加
		btCollisionShape* boxShape = new btBoxShape(btVector3(btScalar(1.), btScalar(50.), btScalar(10.)));  // 
		std::unique_ptr<PhysicsObject> ground = std::make_unique<PhysicsObject>(boxShape, btVector3(0, -56, 0), 0., ColorF{ 1.0, 0., 0. });  // 位置(0,5,0)に設置
		physicsWorld.addObject(std::move(ground));
	}
	*/

	/*
	{
		// 円柱を追加してワールドに追加
		btCollisionShape* cylinderShape = new btCylinderShape(btVector3(btScalar(1.), btScalar(60.), btScalar(1.)));  // 
		std::unique_ptr<PhysicsObject> ground = std::make_unique<PhysicsObject>(cylinderShape, btVector3(0, -56, 0), 0., ColorF{ 1.0, 0., 0. });  // 位置(0,5,0)に設置
		physicsWorld.addObject(std::move(ground));
	}
	*/

	/*
	{
		// 円錐を追加してワールドに追加
		btCollisionShape* coneShape = new btConeShape(2., 2.);
		std::unique_ptr<PhysicsObject> ground = std::make_unique<PhysicsObject>(coneShape, btVector3(1, 0, 0), 0., ColorF{ 1.0, 0., 0. });  // 位置(0,5,0)に設置
		physicsWorld.addObject(std::move(ground));
	}

	{
		// 円錐を追加してワールドに追加
		btCollisionShape* coneShape = new btConeShape(3., 2.);
		std::unique_ptr<PhysicsObject> ground = std::make_unique<PhysicsObject>(coneShape, btVector3(1, -2, 0), 0., ColorF{ 1.0, 0., 0. });  // 位置(0,5,0)に設置
		physicsWorld.addObject(std::move(ground));
	}

	{
		// 円錐を追加してワールドに追加
		btCollisionShape* coneShape = new btConeShape(4., 2.);
		std::unique_ptr<PhysicsObject> ground = std::make_unique<PhysicsObject>(coneShape, btVector3(1, -4, 0), 0., ColorF{ 1.0, 0., 0. });  // 位置(0,5,0)に設置
		physicsWorld.addObject(std::move(ground));
	}
	*/


	// ウインドウとシーンを 1280x720 にリサイズ
	Window::Resize(1280, 720);

	// 背景色 (リニアレンダリング用なので removeSRGBCurve() で sRGB カーブを除去）
	const ColorF backgroundColor = ColorF{ 0.4, 0.6, 0.8 }.removeSRGBCurve();

	// 3D シーンを描く、マルチサンプリング対応レンダーテクスチャ
	// リニア色空間のレンダリング用に TextureFormat::R8G8B8A8_Unorm_SRGB
	// 奥行きの比較のための深度バッファも使うので HasDepth::Yes
	// マルチサンプル・レンダーテクスチャなので、描画内容を使う前に resolve() が必要
	const MSRenderTexture renderTexture{ Scene::Size(), TextureFormat::R8G8B8A8_Unorm_SRGB, HasDepth::Yes };

	// 3D シーンのデバッグ用カメラ
	// 縦方向の視野角 30°, カメラの位置 (10, 16, -32)
	// 前後移動: [W][S], 左右移動: [A][D], 上下移動: [E][X], 注視点移動: アローキー, 加速: [Shift][Ctrl]
	Vec3 cameraPos = Vec3{ player->getPosition().x, 10, player->getPosition().z - 40 };
	Vec3 focusPos = player->getPosition();
	DebugCamera3D camera{ renderTexture.size(), 30_deg, cameraPos, focusPos };

	while (System::Update()) {
		// デバッグカメラの更新 (カメラの移動スピード: 2.0)
		//camera.update(10.0);

		// 3D シーンにカメラを設定
		Graphics3D::SetCameraTransform(camera);

		// キーボードの入力をチェック
		btVector3 moveDirection(0, 0, 0);
		btVector3 upDirection(0, 0, 0);

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
		if (KeySpace.pressed()) {
			upDirection += btVector3(0, 10, 0);   // 上へ
		}

		// 入力に応じて移動方向を正規化し、所望の速度を掛ける
		float speed = 10.0;
		if (moveDirection != btVector3(0, 0, 0)) {
			moveDirection.normalize();
			moveDirection *= speed;
			moveDirection.setY(sphere->getRigidBody()->getLinearVelocity().getY());
			// btRigidBodyの速度を設定
			sphere->getRigidBody()->setLinearVelocity(moveDirection);
		}
		else {
			moveDirection.setY(sphere->getRigidBody()->getLinearVelocity().getY());
			// btRigidBodyの速度を設定
			sphere->getRigidBody()->setLinearVelocity(moveDirection);
		}

		if (upDirection != btVector3(0, 0, 0)) {
			upDirection.setX(sphere->getRigidBody()->getLinearVelocity().getX());
			upDirection.setZ(sphere->getRigidBody()->getLinearVelocity().getZ());
			sphere->getRigidBody()->setLinearVelocity(upDirection);
		}

		/*
		// スペースキーでのジャンプ
		if (KeySpace.down()) {
			//float jumpForce = 10.0;  // ジャンプの力
			//sphere->getRigidBody()->applyCentralForce(btVector3(0, jumpForce, 0));
			//sphere->getRigidBody()->applyCentralImpulse(btVector3(0, jumpForce, 0));
			sphere->getRigidBody()->setLinearVelocity({0.,10.,0.});
		}
		*/

		player->HandleInput();


		// 物理ワールドの更新
		physicsWorld.stepSimulation((float)Scene::DeltaTime());

		// 3D描画
		{
			// renderTexture を背景色で塗りつぶし、
			// renderTexture を 3D 描画のレンダーターゲットに
			const ScopedRenderTarget3D target{ renderTexture.clear(backgroundColor) };

			physicsWorld.draw();
			player->draw(focusPos - cameraPos);

			// Box{ focusPos.x,2,focusPos.z,10 }.draw(ColorF{ 0.8, 0.6, 0.4 }.removeSRGBCurve());
		}

		cameraPos = Vec3{ player->getPosition().x, 10, player->getPosition().z - 40 };
		focusPos = player->getPosition();
		camera = DebugCamera3D{ renderTexture.size(), 30_deg, cameraPos, focusPos };

		// 3D シーンを 2D シーンに描画
		{



			// renderTexture を resolve する前に 3D 描画を実行する
			Graphics3D::Flush();

			// マルチサンプル・テクスチャのリゾルブ
			renderTexture.resolve();

			// リニアレンダリングされた renderTexture をシーンに転送
			Shader::LinearToScreen(renderTexture);
		}
	}

	/*
	///-----stepsimulation_end-----

	//cleanup in the reverse order of creation/initialization

	///-----cleanup_start-----

	//remove the rigidbodies from the dynamics world and delete them
	for (i = dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--)
	{
		btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[i];
		btRigidBody* body = btRigidBody::upcast(obj);
		if (body && body->getMotionState())
		{
			delete body->getMotionState();
		}
		dynamicsWorld->removeCollisionObject(obj);
		delete obj;
	}

	//delete collision shapes
	for (int j = 0; j < collisionShapes.size(); j++)
	{
		btCollisionShape* shape = collisionShapes[j];
		collisionShapes[j] = 0;
		delete shape;
	}

	//delete dynamics world
	delete dynamicsWorld;

	//delete solver
	delete solver;

	//delete broadphase
	delete overlappingPairCache;

	//delete dispatcher
	delete dispatcher;

	delete collisionConfiguration;

	//next line is optional: it will be cleared by the destructor when the array goes out of scope
	collisionShapes.clear();
	*/
}

