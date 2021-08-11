#pragma once
#pragma once
#include"Scene_management.h"

// タイトルシーン
class Title : public MyApp::Scene
{
private:

    Rect m_startButton = Rect(Arg::center = Scene::Center().movedBy(0, 0), 300, 60);
    Transition m_startTransition = Transition(0.4s, 0.2s);

    Rect m_exitButton = Rect(Arg::center = Scene::Center().movedBy(0, 100), 300, 60);
    Transition m_exitTransition = Transition(0.4s, 0.2s);

public:

    Title(const InitData& init)
        : IScene(init) {
        Scene::SetBackground(ColorF(0.2, 0.8, 0.4));

    }

    void update() override
    {
        m_startTransition.update(m_startButton.mouseOver());
        m_exitTransition.update(m_exitButton.mouseOver());

        if (m_startButton.mouseOver() || m_exitButton.mouseOver())
        {
            Cursor::RequestStyle(CursorStyle::Hand);
        }

        if (m_startButton.leftClicked())
        {
            changeScene(State::Game);
        }

        if (m_exitButton.leftClicked())
        {
            System::Exit();
        }
    }

    void draw() const override
    {
        const String titleText = U"ピンボール";
        const String titleText2 = U"Seaター";
        const Vec2 center(Scene::Center().x, 60);
        const Vec2 center2(Scene::Center().x, 180);

        FontAsset(U"Title")(titleText).drawAt(center.movedBy(4, 6), ColorF(0.2, 0.6));
        FontAsset(U"Title")(titleText).drawAt(center);
        FontAsset(U"Title")(titleText2).drawAt(center2.movedBy(4, 6), ColorF(0.2, 0.6));
        FontAsset(U"Title")(titleText2).drawAt(center2);

        m_startButton.draw(ColorF(1.0, m_startTransition.value())).drawFrame(2);
        m_exitButton.draw(ColorF(1.0, m_exitTransition.value())).drawFrame(2);

        FontAsset(U"Menu")(U"はじめる").drawAt(m_startButton.center(), ColorF(0.25));
        FontAsset(U"Menu")(U"おわる").drawAt(m_exitButton.center(), ColorF(0.25));

        Rect(0, 500, Scene::Width(), Scene::Height() - 500)
            .draw(Arg::top = ColorF(0.0, 0.0), Arg::bottom = ColorF(0.0, 0.5));

        const int32 highScore = getData().highScore;
        FontAsset(U"Score")(U"High score: {}"_fmt(highScore)).drawAt(Vec2(620, 550));
    }
};