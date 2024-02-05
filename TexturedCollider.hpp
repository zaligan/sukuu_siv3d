#pragma once
# include <Siv3D.hpp>
# include"Figure.h"

namespace ColliderDrawType
{
	/// @brief コライダーの描画方法を持つ列挙型です
	enum RenderOrder
	{
		//表示しません
		None,

		//Textureの上にフレームを表示します
		AboveFrame,

		//Textureの上に塗りつぶして表示します
		AboveFill,

		//Textureの下にフレームを表示します
		BelowFrame,

		//Textureの下に塗りつぶして表示します
		BelowFill,
	};
}

/// @brief テクスチャーと図形をまとめたクラスです（当たり判定とテクスチャーをまとめて扱うのに便利です）
struct TexturedCollider
{
	//図形(Circle, RectF, Quad, Triangle, Ellipse, Polygon)
	Figure collider;

	//テクスチャー
	Texture texture;

	//図形に対してテクスチャーの中心を合わせるオフセット値です
	Vec2 offsetPos;

	//テクスチャーの大きさです
	double scaleOfTexture = 1.0;

	TexturedCollider() {};

	/// @brief TexturedColliderを作成します
	/// @param collider 衝突判定をする図形です
	/// @param texture 描画するテクスチャーです
	/// @param offsetPos テクスチャーの位置をコライダーに合わせる値です
	/// @param scaleOfTexture テクスチャーの大きさです
	TexturedCollider(Figure collider, Texture texture, Vec2 offsetPos, double scaleOfTexture) :
		collider(collider),
		texture(texture),
		offsetPos(offsetPos),
		scaleOfTexture(scaleOfTexture)
	{};

	TexturedCollider(Figure collider, Texture texture, Vec2 offsetPos) :
		TexturedCollider(collider, texture, offsetPos, 1.0)
	{}

	TexturedCollider(Figure collider, Texture texture, double scaleOfTexture) :
		TexturedCollider(collider, texture, Vec2{ 0,0 }, scaleOfTexture)
	{}

	TexturedCollider(Figure collider, Texture texture) :
		TexturedCollider(collider, texture, Vec2{ 0,0 })
	{}

	/// @brief テクスチャーを描画します
	void draw()const
	{
		texture.scaled(scaleOfTexture).drawAt(collider.center());
	}

	/// @brief テクスチャーとコライダーを描画します
	/// @param drawType コライダーの描画タイプを選択します
	/// @param thickness コライダーの描画タイプがFrameの時の枠線の太さです
	/// @param color コライダーの色です
	void draw(ColliderDrawType::RenderOrder drawType, double thickness = 1.0, const ColorF& color = Palette::White)const
	{
		switch (drawType)
		{
		case ColliderDrawType::None:
			texture.scaled(scaleOfTexture).drawAt(collider.center());
			break;

		case ColliderDrawType::AboveFrame:
			texture.scaled(scaleOfTexture).drawAt(collider.center());
			collider.drawFrame(thickness, color);
			break;

		case ColliderDrawType::AboveFill:
			texture.scaled(scaleOfTexture).drawAt(collider.center());
			collider.draw(color);
			break;

		case ColliderDrawType::BelowFrame:
			collider.drawFrame(thickness,color);
			texture.scaled(scaleOfTexture).drawAt(collider.center());
			break;

		case ColliderDrawType::BelowFill:
			collider.draw(color);
			texture.scaled(scaleOfTexture).drawAt(collider.center());
			break;

		default:
			break;
		}
	}

	void draw(ColliderDrawType::RenderOrder d, const ColorF& color = Palette::White)const
	{
		draw(d, 1.0, color);
	}

	[[nodiscard]]
	constexpr bool intersects(const TexturedCollider& other)const
	{
		return collider.intersects(other.collider);
	}

	[[nodiscard]]
	constexpr bool intersects(const Figure& other)const
	{
		return collider.intersects(other);
	}
};
