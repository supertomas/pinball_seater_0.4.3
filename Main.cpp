#include"Game_Scene.h"
#include"Title.h"
// シーンの名前

void Main()
{
    // 使用するフォントアセットを登録
    FontAsset::Register(U"Title", 120, U"example/font/AnnyantRoman/AnnyantRoman.ttf");
    FontAsset::Register(U"Menu", 30, Typeface::Regular);
    FontAsset::Register(U"Score", 36, Typeface::Bold);
    AudioAsset::Register(U"sound1", U"Sound/sample2.mp3");
    AudioAsset::Register(U"sound2", U"Sound/sample5.mp3");
    AudioAsset::Register(U"sound3", U"Sound/kani_impulse.mp3");
    AudioAsset::Register(U"sound4", U"Sound/tako_impulse.mp3");
    AudioAsset::Register(U"sound5", U"Sound/get.mp3");
    TextureAsset::Register(U"sumitama", U"Images/sumi_shot.png");
    TextureAsset::Register(U"sumi", U"Images/sumi.png");
    TextureAsset::Register(U"bubble", U"Images/bubble.png");
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
