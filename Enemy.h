#pragma once
#include<Siv3D.hpp>
struct EnemyData
{
	RectF rect;
	Vec2 error;
	int32 angle;
};

class Circle_Light
{
public:
	bool m_shine_circle = false;
	Stopwatch shine_timer;

	Circle_Light(Vec2 _pos, double_t _r, ColorF _color, Circle _ball) :
		pos(_pos),
		r(_r),
		color(_color),
		ball(_ball)
	{

	}

	Circle getRegion() const
	{
		return Circle(pos, r);
	}

	void update(Circle circle)
	{
		//isActive = false;

		if (circle.intersects(getRegion()))
		{
			if (circle.y < before_pos)
			{
				shine_timer.restart();
				m_shine_circle = true;
			}
		}
		if (shine_timer.s() > 15.0)
		{
			shine_timer.reset();
			m_shine_circle = false;
		}
		before_pos = circle.y;
	}
	void draw() const
	{
		if (m_shine_circle)
		{
			getRegion().draw(color);
		}
		else
		{
			getRegion().draw();
		}
	}
private:
	Vec2 pos;
	double_t r;
	ColorF color;
	Circle ball;
	double_t before_pos;
};
class Enemy
{
public:
	//Vec2 bulletPos;

	Enemy(String _EnemyNames, Vec2 _pos, Vec2 _size, Vec2 _error, int32 _angle, P2World _world,const P2Body& _object, bool _collided) :
		EnemyNames(_EnemyNames),
		pos(_pos),
		size(_size),
		error_enemy_body(_error),
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
		RectF rect(pos.x, pos.y, size.x + error_enemy_body.x, size.y + error_enemy_body.y);
		return rect.rotated(angle * 1_deg);
	}
	RectF getInkRegion(Vec2 Ink_Pos) const
	{
		return RectF((Ink_Pos.x - InkSize.x / 2) , (Ink_Pos.y - InkSize.y), InkSize.x, InkSize.y);
	}
	RectF InkIntersectRegion(Vec2 Ink_Pos) const
	{
		return RectF((Ink_Pos.x - InkSize.x / 4) + error_sumi.x, (Ink_Pos.y - InkSize.y)+error_sumi.y , InkSize.x / 2, InkSize.y);
	}

	Circle getBubbleRegion(Vec2 bulletPos) const
	{
		return Circle({ bulletPos.x, bulletPos.y }, bubble_radius);
	}

	Circle BubbleIntersectRegion(Vec2 bulletPos) const
	{
		return Circle(bulletPos, 0.7);
	}

	void update(bool light,Circle circle,bool* Intersect_Sumi,int32* score)
	{

		double deltaTime = Scene::DeltaTime();

		if (light && EnemyNames == U"octopus")
		{
			InkShotTimer += deltaTime;
			if (InkShotTimer >= InkShotCoolTime)
			{
				InkShotTimer -= InkShotCoolTime;
				ink_poses << Vec2{pos.x-1.2, pos.y+7.8};
				direction_ink << Random(0, 2);
			}
		}
		else if (light && EnemyNames == U"crab")
		{
			if(!BubbleDurationTimer.isStarted())
			{
				BubbleSpawnTimer.start();
			}
			if (BubbleSpawnTimer.ms() > 1500)
			{
				BubbleDurationTimer.restart();
				BubbleSpawnTimer.reset();
			}
			if (BubbleDurationTimer.ms() <= 1000 && BubbleDurationTimer.ms() > 0)
			{
				BubbleShotTimer += deltaTime;
				if (BubbleShotTimer >= BubbleShotCoolTime)
				{
					BubbleShotTimer -= BubbleShotCoolTime;
					BubblePos << Vec2{pos.x + 2.3,pos.y + 3};
					direction_bubble << Random(0, 6);
				}
			}
			else if (BubbleDurationTimer.ms() > 1000)
			{
				BubbleDurationTimer.reset();
				BubbleSpawnTimer.restart();
			}
		}
		for (size_t i = 0; i < ink_poses.size(); i++)
		{
			if (circle.intersects(InkIntersectRegion(ink_poses[i])))
			{
				*Intersect_Sumi = true;
				SumiTimer.restart();
			}

			if (direction_ink[i] == 0)
			{
				ink_poses[i].x += deltaTime * (-18.0);
			}
			else if (direction_ink[i] == 1)
			{
				ink_poses[i].x += deltaTime * (-13.0);
			}
			else if (direction_ink[i] == 2)
			{
				ink_poses[i].x += deltaTime * (-8.0);
			}
		}
		for (auto& ink_pos : ink_poses)
		{
			ink_pos.y += deltaTime * InkSpeed;
		}

		for (size_t i = 0; i < BubblePos.size(); i++)
		{
			if (direction_bubble[i] == 0)
			{
				BubblePos[i].x += deltaTime * (26);
			}
			if (direction_bubble[i] == 1)
			{
				BubblePos[i].x += deltaTime * (20);
			}
			else if (direction_bubble[i] == 2)
			{
				BubblePos[i].x += deltaTime * (15);
			}
			else if (direction_bubble[i] == 3)
			{
				BubblePos[i].x += deltaTime * (9);
			}
			else if (direction_bubble[i] == 4)
			{
				BubblePos[i].x += deltaTime * (5);
			}
			else if (direction_bubble[i] == 5)
			{
				BubblePos[i].x += deltaTime * (0);
			}
			else if (direction_bubble[i] == 6)
			{
				BubblePos[i].x += deltaTime * (-5);
			}

		}
		for (auto& enemyBullet : BubblePos)
		{
			enemyBullet.y += deltaTime * BubbleSpeed;
		}
		// 画面外に出た自機ショットは消滅
		
		// 画面外に出た自機ショットは消滅
		//enemyBullets.remove_if([](const Vec2& b) {return b.y > 700; });
		for (auto it = ink_poses.begin(); it != ink_poses.end();)
		{
			// 円がクリックされたらその地点を表す要素を削除
			if (it->y < -30)
			{
				// 現在のイテレータが指す要素を削除し、イテレータを進める
				it = ink_poses.erase(it);
				direction_ink.erase(direction_ink.begin());
			}
			else
			{
				// イテレータを進める
				++it;
			}
		}

		for (size_t i = 0; i < BubblePos.size(); i++)
		{
			if (BubbleIntersectRegion(BubblePos[i]).intersects(circle))
			{
				BubblePos.erase(BubblePos.begin()+i);
				*score += 2;

			}
		}


		for (auto it = BubblePos.begin(); it != BubblePos.end();)
		{
			// 円がクリックされたらその地点を表す要素を削除
			if (it->y < -40)
			{
				// 現在のイテレータが指す要素を削除し、イテレータを進める
				it = BubblePos.erase(it);
				direction_bubble.erase(direction_bubble.begin());
			}
			else
			{
				// イテレータを進める
				++it;
			}
		}
		if (SumiTimer.s() > 6)
		{
			*Intersect_Sumi = false;
			SumiTimer.reset();
		}
	}
	void draw() const
	{
		if (!collied)
		{
			getRegion()(TextureAsset(EnemyNames)).draw();
		}
		else
		{
			getRegion()(TextureAsset(EnemyNames)).draw(Palette::Greenyellow);
		}
		for (size_t i = 0; i < ink_poses.size(); i++)
		{
			if (direction_ink[i] == 0)
			{
				//getBulletRegion(enemyBullets[i]).draw(ColorF(1.0, 0.0, 0.0));
				//InkIntersectRegion(ink_poses[i]).rotated(73).draw(ColorF(0.0, 1.0, 0.0));
				(getInkRegion(ink_poses[i]).rotated(0.75))(TextureAsset(U"sumitama")).draw();
			}
			else if (direction_ink[i] == 1)
			{
				//getBulletRegion(enemyBullets[i]).rotated(60).draw(ColorF(1.0, 0.0, 0.0));
				//InkIntersectRegion(ink_poses[i]).rotated(60).draw(ColorF(0.0, 1.0, 0.0));
				(getInkRegion(ink_poses[i]).rotated(0.4))(TextureAsset(U"sumitama")).draw();
			}
			else if (direction_ink[i] == 2)
			{
				//getBulletRegion(enemyBullets[i]).rotated(-60).draw(ColorF(1.0, 0.0, 0.0));
				//InkIntersectRegion(ink_poses[i]).draw(ColorF(0.0, 1.0, 0.0));
				(getInkRegion(ink_poses[i]).rotated(0.15))(TextureAsset(U"sumitama")).draw();
			}
		}
		for (size_t i = 0; i < BubblePos.size(); i++)
		{
			//getBubbleRegion(enemyBullets[i]).draw(ColorF(1.0, 0.0, 0.0));
			//BubbleIntersectRegion(BubblePos[i]).draw(ColorF(.0, 1.0, 0.0));
			getBubbleRegion(BubblePos[i])(TextureAsset(U"bubble")).draw();
		}
	}
private:
	String EnemyNames;
	Vec2 pos;
	Vec2 size;
	Effect effect;
	Vec2 error_enemy_body;
	Vec2 error_sumi = {0,0.5};

	int32 angle;
	P2World world;
	P2Body object;
	Vec2 InkSize = { 3.0,6.0 };
	Vec2 BulletSize = { 9.0,13.0 };
	double_t bubble_radius = 1.0;
	Stopwatch SumiTimer;
	double InkShotCoolTime = 2.60;
	// 敵ショットのクールタイムタイマー
	double InkShotTimer = 0.0;
	Array<Vec2> ink_poses;
	double InkSpeed = 18.0;
	Array<int> direction_ink;
	double BubbleShotCoolTime = 0.1;
	// 敵ショットのクールタイムタイマー
	double BubbleShotTimer = 0.0;
	Array<Vec2> BubblePos;
	double BubbleSpeed = 20.0;
	Array<int> direction_bubble;
	Stopwatch BubbleSpawnTimer;
	Stopwatch BubbleDurationTimer;
	bool collied;

};