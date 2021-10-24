#pragma once
#include"worldObject.h"
#include"Enemy.h"
#include"Spring.h"
#include"Scene_management.h"
#include"Bubble.h"

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
    const Font font = Font(40, U"example/font/LogoTypeGothic/LogoTypeGothic.otf");

    Size size;


    BubbleTexture bubbleTexture;
    //スコア
    UI ui;

    Stopwatch ItemGetTimer;
    Stopwatch ItemDurationTimer;
    Stopwatch SumiTimer;
    int32 time = 0;
    bool isActive = false;
    // ボール 〇 ばね
    double_t init_pos = -7.0, spring_pos = -7.0;
    P2Body Physics_ball;
    Circle circle_ball;
    P2BodyID ballID;
    std::unique_ptr<Spring> spring;
    Array<RectF> Itemrects;
    //カメラ
    Camera2D camera = Camera2D(Vec2(0, -8), 24.0);

    int32 ItemID = 0;
    Effect effect;
    Font effectFont;
    bool first = true;
    bool Intersect_Sumi = false;
public:
    Array<P2BodyID> m_collidedIDs;

    Game(const InitData& init)
        : IScene(init)
    {
        size = Size(840, 680);
        bubbleTexture = BubbleTexture(size, 10, 40, 2, 6);
        Scene::SetBackground(ColorF(0.2, 0.5, 1.0));
        circle_ball = Circle(Physics_ball.getPos(), 0.6);
        Physics_ball = worldObjects.m_world.createDynamicCircle(Vec2(15.0, -10), 0.55, P2Material(0.05, 0.0));
        ballID = Physics_ball.id();
        UpdateWorldObjects(config, worldObjects,circle_ball);
        effectFont = Font(400, Typeface::Heavy);
        Graphics::SetTargetFrameRateHz(60);
        Itemrects.push_back(config.LoadItemRect()[0]);
        spring = std::make_unique<Spring>(worldObjects.m_world, init_pos, Physics_ball, ballID, spring_pos);
        TextureAsset::Register(U"crab", Emoji(U"🦀"));
        TextureAsset::Register(U"octopus", Emoji(U"🐙"));
        ui.score_font = Font(100);
        ui.score = 0;
        ui.life_font = Font(70);
        ui.life = 5;

    }


    void update() override
    {
       /* ClearPrint();
        Print(worldObjects.bumper_data.bumper_id.size());*/
        // 泡の更新
        {
            bubbleTexture.update(Scene::DeltaTime());
            bubbleTexture.render(ColorF(0.1, 0.3, 0.6));
        }
        m_collidedIDs.clear();
        if (!config)
        {
            return;
        }

        if (first && Physics_ball.getPos().y > -10.0 && Physics_ball.getPos().x < 0.0)
        {
            first = false;
            worldObjects.spinner.spinnerJoint = P2PivotJoint{};
            worldObjects.spinner.spinnerBody = P2Body{};
            worldObjects.spinner.spinnerBody = worldObjects.m_world.createDynamicRect(Vec2(config.LoadSpinner().pos), SizeF(config.LoadSpinner().size), P2Material(0.1, 0.0));
            worldObjects.spinner.spinnerJoint = worldObjects.m_world.createPivotJoint(worldObjects.frames[0], worldObjects.spinner.spinnerBody,
                Vec2(config.LoadSpinner().pos.x + config.LoadSpinner().adjust.x, config.LoadSpinner().pos.y + config.LoadSpinner().adjust.y)).setMaxMotorTorque(0.05).setMotorSpeed(0).setMotorEnabled(true);
        }

 
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
                        const Vec2 effectpos = Physics_ball.getPos().movedBy(0, 100);
                        effect.add<NumberEffect>(effectpos, effectFont, ui.score);
                        if (worldObjects.enemy_data.enemies_ids[i] == worldObjects.enemy_data.P2_enemies[0].id())
                        {
                            AudioAsset(U"sound3").play();
                        }
                        else if (worldObjects.enemy_data.enemies_ids[i] == worldObjects.enemy_data.P2_enemies[1].id())
                        {
                            AudioAsset(U"sound4").play();
                        }
                }
            }

            for (size_t i = 0; i < worldObjects.bumper_data.bumper_id.size(); i++)
            {
                if (pair.a == worldObjects.bumper_data.bumper_id[i] || pair.b == worldObjects.bumper_data.bumper_id[i])
                {
                    ui.score += 5;
                    AudioAsset(U"sound1").play();
                    const Vec2 effectpos = Physics_ball.getPos().movedBy(0, 100);
                    effect.add<NumberEffect>(effectpos, effectFont, ui.score);
                }
            }
            if (pair.a == worldObjects.bumper_data.Itembumpers.id() || pair.b == worldObjects.bumper_data.Itembumpers.id())
            {
                AudioAsset(U"sound1").play();
            }
            for (size_t i = 0; i < worldObjects.bumper_data.Trianglebumpers.size(); i++)
            {
                if (pair.a == worldObjects.bumper_data.Trianglebumpers[i].id() || pair.b == worldObjects.bumper_data.Trianglebumpers[i].id())
                {
                    AudioAsset(U"sound1").play();
                }
            }
        }

       
        
        // 物理演算ワールドの更新
        worldObjects.m_world.update(timeDelta, 24, 12);
        //物理演算の更新の下にcameraをsetしなければならない
        circle_ball.setPos(Physics_ball.getPos());
        camera.setCenter(Physics_ball.getPos());
        for (auto& i :worldObjects.circle_lights)
        {
            i.update(circle_ball);
        }


       
            for (size_t i = 0; i < worldObjects.enemy_data.enemies.size(); i++)
            {
                worldObjects.enemy_data.enemies[i].update(worldObjects.circle_lights[i].m_shine_circle, circle_ball,&Intersect_Sumi,&ui.score);

            }
            spring->update();

            //アイテムを取った時の処理
            for (auto it = Itemrects.begin(); it != Itemrects.end();)
            {
                if (it->intersects(circle_ball))
                {
                    it = Itemrects.erase(it);
                    worldObjects.getItem = true;
                    AudioAsset(U"sound5").play();
                    ItemGetTimer.restart();
                }
                else
                {
                    it++;
                }
            }

            if (ItemGetTimer.s() > 10)
            {
                worldObjects.getItem = false;
                ItemGetTimer.reset();
                time = Random(10, 25);
                ItemDurationTimer.restart();
                worldObjects.bumper_data.Itembumpers = P2Body{};
            }

            if (worldObjects.bumper_data.Itembumpers.isEmpty() && worldObjects.getItem)
            {
                worldObjects.bumper_data.Itembumpers = worldObjects.m_world.createStaticRect(Vec2(config.loadStraightBumper().pos), RectF(0, 0, { config.loadStraightBumper().size }), P2Material(2.65, 2.65));
                ItemID = worldObjects.bumper_data.Trianglebumpers.size() - 1;
            }

            if (ItemDurationTimer.isRunning() && ItemDurationTimer.s() > time)
            {
                int32 rand = Random(0, 2);
                Itemrects.push_back(config.LoadItemRect()[rand]);
                ItemDurationTimer.reset();
            }

            
        effect.update();
    
        //ボールが下に落ちた時の処理
        if (Physics_ball.getPos().y > 10)
        {
            Physics_ball = worldObjects.m_world.createDynamicCircle(Vec2(15.0, -10), 0.55, P2Material(0.05, 0.0));
            ballID = Physics_ball.id();
            spring->ballID = ballID;
            spring->ball = Physics_ball;
            ui.life--;
            first = true;
        }
        //GameOver時の処理
        if (ui.life == 0)
        {
            changeScene(State::GameOver);
        }
        getData().nowScore = ui.score;
        if (ui.score > getData().highScore)
        {
            getData().highScore = ui.score;
        }
        //toml更新、オブジェクトのサイズを実行中に変えた場合の処理
        if (config.hasUpdate())
        {
            config.reload();
            // 完全にリフレッシュ
            {
                worldObjects.clear();
                WorldObjects tmp;
                UpdateWorldObjects(config, tmp, circle_ball);
                worldObjects = tmp;
                Physics_ball = worldObjects.m_world.createDynamicCircle(Vec2(Physics_ball.getPos()), 0.55, P2Material(0.05, 0.0));
                spring = std::make_unique<Spring>(worldObjects.m_world, init_pos, Physics_ball, ballID, spring_pos);
                circle_ball = Circle(Physics_ball.getPos(), 0.6);
            }
        }
    }

    void draw() const override
    {
        //const ScopedRenderStates2D rasterizer{ RasterizerState::WireframeCullNone };
        Circle(Scene::Center(), 800).draw(ColorF(1.0), ColorF(0.1, 0.3, 0.6));
        bubbleTexture.getTexture().draw();
        {
            const auto transformer = camera.createTransformer();

            for (const auto& frame : worldObjects.frames)
            {
                frame.draw(Palette::Red);
            }

            //アイテム描画
            if (worldObjects.getItem)
            {
                worldObjects.bumper_data.Itembumpers.draw(GetColor(worldObjects.bumper_data.Itembumpers, m_collidedIDs, Bumper_Color::Gray));
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
            for (const auto& bumper : worldObjects.bumper_data.Trianglebumpers)
            {
                bumper.draw(GetColor(bumper, m_collidedIDs, Bumper_Color::Orange));
            }
            for (const auto& bumper : worldObjects.bumper_data.Round_bumpers)
            {
                bumper.draw(GetColor(bumper, m_collidedIDs, Bumper_Color::Orange));
            }

            circle_ball.draw();
            for (auto& enemy : worldObjects.enemy_data.enemies)
            {
                enemy.draw();
            }

            for (auto& i : worldObjects.circle_lights)
            {
                i.draw();
            }
            Physics_ball.draw(Palette::Black);
            spring->draw();
            worldObjects.spinner.spinnerBody.draw(GetColor(worldObjects.spinner.spinnerBody, m_collidedIDs, Bumper_Color::Gray));
            worldObjects.spinner.spinnerJoint.draw(Palette::Red);

            // フリッパーの描画
            worldObjects.flipper.leftFlipper.draw(Palette::Orange);
            worldObjects.flipper.rightFlipper.draw(Palette::Orange);

            //// ジョイントの可視化
            worldObjects.flipper.leftJoint.draw(Palette::Red);
            worldObjects.flipper.rightJoint.draw(Palette::Red);
        }
        


        if (Intersect_Sumi)
        {
            Rect(170, 100, 500, 500)(TextureAsset(U"sumi")).draw();
        }
                 
        ui.score_font(U"{}"_fmt(ui.score)).drawAt({ 700,50 }, Palette::Black);
        ui.life_font(U"{}"_fmt(ui.life)).drawAt({ 50,50 }, Palette::Red);
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

