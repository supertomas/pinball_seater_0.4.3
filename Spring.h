#pragma once
#include<Siv3D.hpp>
class Spring
{
public:
    P2BodyID ballID;
    P2Body ball;

    Spring(P2World _world, double_t _init_pos, P2Body _ball, P2BodyID _ballID, double_t& _pos) :
        world(_world),
        init_pos(_init_pos),
        ball(_ball),
        ballID(_ballID),
        pos(_pos)
    {

    }
    void update()
    {
        next_pos = pos;
        for (auto [pair, collision] : world.getCollisions())
        {
            const P2BodyID a = pair.a;
            const P2BodyID b = pair.b;
            if (pair.a == ballID && pair.b == RectID || pair.b == ballID && pair.a == RectID)
            {
                ball.applyForce(Vec2(0, abs(let_pos - init_pos) * (-10.0)));
            }
        }

        if (RectBody.isEmpty())
        {
            RectBody = world.createStaticRect(Vec2(0, 0), RectF(14.4, pos, 1.2, 2), P2Material(0.1, 0.0));
            RectID = RectBody.id();
        }

        if (KeyDown.pressed())
        {
            action = false;
            pos += (init_pos - next_pos) * k_x + 0.1;
            RectBody = world.createStaticRect(Vec2(0, 0), RectF(14.4, pos, 1.2, 2), P2Material(0.1, 0.0));
            RectID = RectBody.id();
        }

        if (KeyDown.up())
        {
            let_pos = pos;
            action = true;
        }

        if (action && pos > init_pos + 0.1)
        {
            pos += (k_x * (init_pos - next_pos)) * 8.2;
            RectBody = world.createStaticRect(Vec2(0, 0), RectF(14.4, pos, 1.2, 2), P2Material(0.1, 0.0));
            RectID = RectBody.id();
        }

        else if (action && pos <= init_pos + 0.1)
        {
            pos = init_pos;
            let_pos = init_pos;
            action = false;
        }

    }

    void draw()
    {
        RectBody.draw();
    }

private:
    P2World world;
    P2Body RectBody;
    P2BodyID RectID;
    double_t init_pos;
    double_t k_x = 0.015;
    bool action;
    double_t pos;
    double_t next_pos;
    double_t let_pos = init_pos;

};