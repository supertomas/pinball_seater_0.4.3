#pragma once
#include<Siv3D.hpp>
struct EnemyData
{
	RectF rect;
	Vec2 error;
	int32 angle;
};


class Enemy
{
public:
	Enemy(String _data, Vec2 _pos, Vec2 _size, Vec2 _error, int32 _angle, P2World _world, const P2Body& _object, bool _collided) :
		data(_data),
		pos(_pos),
		size(_size),
		error(_error),
		angle(_angle),
		world(_world),
		object(_object),
		collied(_collided)
	{

	}

	Vec2 getPos()
	{
		return pos;
	}

	Vec2 getSize()
	{
		return size;
	}

	int32 getAngle()
	{
		return angle;
	}
	Quad getRegion() const
	{
		RectF rect(pos.x, pos.y, size.x, size.y);
		return rect.rotated(angle * 1_deg);
	}

	Quad getCollision()
	{
		RectF rect(pos.x, pos.y, size.x + error.x, size.y + error.y);
		return rect.rotated(angle * 1_deg);
	}
	void update()
	{
		collied = false;
		for (auto [pair, collision] : world.getCollisions())
		{
			if (pair.b == object.id())
			{
				collied = true;
				effect.update();
			}
			else if (pair.a == object.id())
			{
				collied = true;
			}
			else
			{
				collied = false;
			}
		}
	}
	void draw() const
	{
		if (!collied)
		{
			getRegion()(TextureAsset(data)).draw();
		}
		else
		{
			getRegion()(TextureAsset(data)).draw(Palette::Greenyellow);
		}
	}
private:
	String data;
	Vec2 pos;
	Vec2 size;
	Effect effect;
	Vec2 error;
	int32 angle;
	P2World world;
	P2Body object;
	bool collied;

};