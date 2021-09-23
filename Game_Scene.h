#pragma once
#include"worldObject.h"
#include"Enemy.h"
#include"Spring.h"
#include"Scene_management.h"

class Circle_Light
{
public:
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
        isActive = false;

        if (circle.intersects(getRegion()))
        {
            if (circle.y < before_pos)
            {
                isActive = true;
            }
        }
        before_pos = circle.y;
    }
    void draw() const
    {
        if (isActive)
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
    bool isActive = false;
};




struct NumberEffect : IEffect
{
    Vec2 m_start;
    int32 m_number;
    Font m_font;
    int32 score;

    NumberEffect(Vec2 start, const Font& font, int32 score)
        : m_start(start)
        , m_font(font)
        , score(score)
    {}

    bool update(double t) override
    {
        //ClearPrint();
        //Print(m_font);
        const HSV color(180 - m_number * 1.8, 1.0 - (t * 2.0));
        m_font(score, U"点").drawAt(m_start.movedBy(0, t * -120), color);
        return t < 0.5;
    }
};

struct UI
{
    int32 score;
    Font score_font;
    int32 life;
    Font life_font;
};


class Game : public MyApp::Scene
{

private:
    // フレームレートを 60 に固定
    // フレームレートに依存しない、物理シミュレーションの更新
    double timeDelta = 1.0 / 40.0;
    TOMLConfig config = TOMLConfig(U"example/config/config.toml");
    WorldObjects worldObjects;

    //スコア
    UI ui;

    Stopwatch timer;
    Stopwatch timer2;
    int32 time = 0;
    bool isActive = false;
    // ボール 〇 ばね
    double_t init_pos = -7.0, spring_pos = -7.0;
    P2Body ball;
    Circle circle;
    Array<Circle_Light> circle_light;
    P2BodyID ballID;
    std::unique_ptr<Spring> spring;
    Array<RectF> Itemrects;
    //カメラ
    Camera2D camera = Camera2D(Vec2(0, -8), 24.0);
    int32 ItemID = 0;
    Effect effect;
    Font effectFont;
    bool first = true;

public:
    Array<P2BodyID> m_collidedIDs;

    Game(const InitData& init)
        : IScene(init)
    {
        Scene::SetBackground(ColorF(0.2, 0.5, 1.0));
        UpdateWorldObjects(config, worldObjects);
        effectFont = Font(400, Typeface::Heavy);
        Graphics::SetTargetFrameRateHz(60);
        ball = worldObjects.m_world.createDynamicCircle(Vec2(15.0, -10), 0.55, P2Material(0.05, 0.0));
        ballID = ball.id();
        circle = Circle(ball.getPos(), 0.6);
        Itemrects.push_back(config.LoadItemRect()[0]);
        spring = std::make_unique<Spring>(worldObjects.m_world, init_pos, ball, ballID, spring_pos);
        TextureAsset::Register(U"crab", Emoji(U"🦀"));
        TextureAsset::Register(U"octopus", Emoji(U"🐙"));
        ui.score_font = Font(100);
        ui.score = 0;
        ui.life_font = Font(70);
        ui.life = 5;
        circle_light << Circle_Light({ -11,-18 }, 0.7, ColorF(0.4, 0.2, 0.9), circle);
        circle_light << Circle_Light({ 11,-18 }, 0.7, ColorF(0.4, 0.2, 0.9), circle);
    }


    void update() override
    {


        m_collidedIDs.clear();
        if (!config)
        {
            return;
        }

        if (first && ball.getPos().y > -10.0 && ball.getPos().x < 0.0)
        {
            first = false;
            worldObjects.spinner.spinnerJoint = P2PivotJoint{};
            worldObjects.spinner.spinnerBody = P2Body{};
            worldObjects.spinner.spinnerBody = worldObjects.m_world.createDynamicRect(Vec2(config.LoadSpinner().pos), SizeF(config.LoadSpinner().size), P2Material(0.1, 0.0));
            worldObjects.spinner.spinnerJoint = worldObjects.m_world.createPivotJoint(worldObjects.frames[0], worldObjects.spinner.spinnerBody,
                Vec2(config.LoadSpinner().pos.x + config.LoadSpinner().adjust.x, config.LoadSpinner().pos.y + config.LoadSpinner().adjust.y)).setMaxMotorTorque(0.05).setMotorSpeed(0).setMotorEnabled(true);

        }

        // camera.update();
        {
            // 左フリッパーの操作
            worldObjects.flipper.leftFlipper.applyTorque(KeyLeft.pressed() ? -200 : 40);

            // 右フリッパーの操作
            worldObjects.flipper.rightFlipper.applyTorque(KeyRight.pressed() ? 200 : -40);
            for (auto [pair, collision] : worldObjects.m_world.getCollisions())
            {
                if (pair.a == ballID)
                {
                    m_collidedIDs << pair.b;
                }
                else if (pair.b == ballID)
                {
                    m_collidedIDs << pair.a;
                }
                for (size_t i = 0; i < worldObjects.enemy_data.enemies_ids.size(); i++)
                {
                    if (pair.a == worldObjects.enemy_data.enemies_ids[i] || pair.b == worldObjects.enemy_data.enemies_ids[i])
                    {
                        ui.score += 10;
                        const Vec2 effectpos = ball.getPos().movedBy(0, 100);
                        effect.add<NumberEffect>(effectpos, effectFont, ui.score);
                    }
                }
                for (size_t i = 0; i < worldObjects.bumper_data.bumper_id.size(); i++)
                {
                    if (pair.a == worldObjects.bumper_data.bumper_id[i] || pair.b == worldObjects.bumper_data.bumper_id[i])
                    {
                        ui.score += 5;
                        const Vec2 effectpos = ball.getPos().movedBy(0, 100);
                        effect.add<NumberEffect>(effectpos, effectFont, ui.score);
                    }
                }
            }


            // 物理演算ワールドの更新
            worldObjects.m_world.update(timeDelta, 24, 12);
            //物理演算の更新の下にcameraをsetしなければならない
            circle.setPos(ball.getPos());
            camera.setCenter(ball.getPos());
            for (auto& i : circle_light)
            {
                i.update(circle);
            }

            if (ball.getPos().y > 10)
            {
                ball = worldObjects.m_world.createDynamicCircle(Vec2(15.0, -10), 0.55, P2Material(0.05, 0.0));
                ballID = ball.id();
                spring->ballID = ballID;
                spring->ball = ball;
                ui.life--;
                first = true;
            }

            spring->update();
            for (auto it = Itemrects.begin(); it != Itemrects.end();)
            {
                if (it->intersects(circle))
                {
                    it = Itemrects.erase(it);
                    worldObjects.getItem = true;
                    timer.restart();
                }
                else
                {
                    it++;
                }
            }

            if (timer.s() > 10)
            {
                worldObjects.getItem = false;
                timer.reset();
                time = Random(10, 25);
                timer2.restart();
                worldObjects.bumper_data.Itembumpers = P2Body{};
            }

            if (worldObjects.bumper_data.Itembumpers.isEmpty() && worldObjects.getItem)
            {
                worldObjects.bumper_data.Itembumpers = worldObjects.m_world.createStaticRect(Vec2(config.loadStraightBumper().pos), RectF(0, 0, { config.loadStraightBumper().size }), P2Material(2.65, 2.65));
                ItemID = worldObjects.bumper_data.bumpers.size() - 1;
            }

            if (timer2.isRunning() && timer2.s() > time)
            {
                int32 rand = Random(0, 2);
                Itemrects.push_back(config.LoadItemRect()[rand]);
                timer2.reset();
            }


          /*  for (auto& bumper : object_bumpers)
            {
                bumper.update();
            }*/


            if (ui.life == 0)
            {
                changeScene(State::GameOver);
            }
            getData().nowScore = ui.score;
            if (ui.score > getData().highScore)
            {
                getData().highScore = ui.score;
            }
            effect.update();
        }
        if (config.hasUpdate())
        {
            config.reload();
            // 完全にリフレッシュ
            {
                worldObjects.clear();
                WorldObjects tmp;
                UpdateWorldObjects(config, tmp);
                worldObjects = tmp;
                ball = worldObjects.m_world.createDynamicCircle(Vec2(ball.getPos()), 0.55, P2Material(0.05, 0.0));
                spring = std::make_unique<Spring>(worldObjects.m_world, init_pos, ball, ballID, spring_pos);
                circle = Circle(ball.getPos(), 0.6);
            }
        }
        ClearPrint();
        Print(worldObjects.getItem);
        Print(Itemrects);
        Print(Itemrects.size());

        Print(timer2.isRunning());
        Print(U"time =" + time);
        Print(U"timer2 = " + timer2.ms());
    }

    void draw() const override
    {
        ui.score_font(U"{}"_fmt(ui.score)).drawAt({ 700,50 }, Palette::Black);
        ui.life_font(U"{}"_fmt(ui.life)).drawAt({ 50,50 }, Palette::Red);

        const auto transformer = camera.createTransformer();
        for (const auto& frame : worldObjects.frames)
        {
            frame.draw(Palette::Red);
        }

        //アイテム描画
        if (worldObjects.getItem)
        {
            worldObjects.bumper_data.Itembumpers.draw(GetColor(worldObjects.bumper_data.Itembumpers, m_collidedIDs,Bumper_Color::Gray));
        }


        for (const auto& frame : worldObjects.frames)
        {
            frame.draw(Palette::Yellowgreen);
        }
        for (auto& Itemrect : Itemrects)
        {
            Itemrect.draw(Palette::Gray);
        }


        // バンパーの描画
        for (const auto& bumper : worldObjects.bumper_data.bumpers)
        {
            bumper.draw(GetColor(bumper, m_collidedIDs,Bumper_Color::Orange));
        }


        circle.draw();
        for (auto& enemy : worldObjects.enemy_data.enemies)
        {
            enemy.draw();
        }

        for (auto& i : circle_light)
        {
            i.draw();
        }
        ball.draw(Palette::Black);
        spring->draw();
        worldObjects.spinner.spinnerBody.draw(GetColor(worldObjects.spinner.spinnerBody, m_collidedIDs,Bumper_Color::Gray));
        worldObjects.spinner.spinnerJoint.draw(Palette::Red);

        // フリッパーの描画
        worldObjects.flipper.leftFlipper.draw(Palette::Orange);
        worldObjects.flipper.rightFlipper.draw(Palette::Orange);

        //// ジョイントの可視化
        worldObjects.flipper.leftJoint.draw(Palette::Red);
        worldObjects.flipper.rightJoint.draw(Palette::Red);

    }
};

class GameOver : public MyApp::Scene
{
public:
    GameOver(const InitData& init)
        : IScene(init)
    {

    }
    void update() override
    {
        if (KeyEnter.down())
        {
            changeScene(State::Game);
        }
    }
    void draw() const override
    {
        FontAsset(U"Score")(U"今回のスコア", getData().nowScore, U"点").drawAt(Scene::Center().x, 100);
        FontAsset(U"Score")(U"ハイスコア", getData().highScore, U"点").drawAt(Scene::Center().x, 250);
        FontAsset(U"Score")(U"エンターキーを押してね").drawAt(Scene::Center().x, 400);
    }
};


