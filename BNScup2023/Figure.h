#pragma once
#include"Shapes.h"
#include"Character.h"





class TreeObject {//MainのBoxesにTreebodyを追加することで当たり判定が適用できる。
public:
	TreeObject(axis3D pos, float scale) : axis(pos), Treebody(pos, width, height* scale, width) {//インストラクタの横に:X(Y)とするとXをYで初期化できるらしい。引数もこれですぐ使える
		this->scale = scale;
		Treebody.width = width;
		Treebody.height = height * this->scale;
		Treebody.depth = depth;
	}

	axis3D axis;
	BoxObject Treebody;
	float scale = 1.0f;
	float width = 2.0f;
	float depth = 2.0f;
	float height = 7.0f;
	const Texture treesurface{ U"example/texture/tree.jpg", TextureDesc::MippedSRGB };
	const Texture grass{ U"example/texture/grass.jpg", TextureDesc::MippedSRGB };

	void display() {//葉っぱとかの部分を描画

		Treebody.shape = Box{ axis.x,axis.y + height * scale / 2.0 ,axis.z,width + 0.3,height * scale + 0.3,depth + 0.3 };
		Treebody.shape.draw(treesurface);
		Cone{ Vec3{axis.x, axis.y + height * scale ,axis.z}, 4 * scale, 4 * scale }.draw(grass);
		Cone{ Vec3{axis.x, (axis.y + height * scale) / 1.7,axis.z}, 4 * scale, 4 * scale }.draw(grass);

	}
};
