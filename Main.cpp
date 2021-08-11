#include"Game_Scene.h"
#include"Title.h"
// シーンの名前

void Main()
{
    // 使用するフォントアセットを登録
    FontAsset::Register(U"Title", 120, U"example/font/AnnyantRoman/AnnyantRoman.ttf");
    FontAsset::Register(U"Menu", 30, Typeface::Regular);
    FontAsset::Register(U"Score", 36, Typeface::Bold);

    // 背景色を設定
  //  Scene::SetBackground(ColorF(0.2, 0.8, 0.4));

    // シーンと遷移時の色を設定
    MyApp manager;
    manager
        .add<Title>(State::Title)
        .add<Game>(State::Game)
        .add<GameOver>(State::GameOver)
        .setFadeColor(ColorF(1.0));

    while (System::Update())
    {
        if (!manager.update())
        {
            break;
        }
    }
}