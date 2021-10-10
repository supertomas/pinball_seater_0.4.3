#pragma once
# include <Siv3D.hpp> // OpenSiv3D v0.4.2

/* ここに BubbleTexture クラス */

class BubbleTexture
{
private:

    // 泡を描画する内部テクスチャ
    MSRenderTexture m_texture;

    // 泡を表現する Circle の配列
    Array<Circle> m_bubbles;

    // 泡の発生レートの最大値（個/秒）
    static constexpr double MaxSpawnRate = 300.0;

    // 泡のスピードの最小値（ピクセル/秒）
    static constexpr double MinSpeed = 10.0;

    // 泡の発生レート（個/秒）
    double m_spawnRate = 10.0;

    // 泡のスピード（ピクセル/秒）
    double m_speed = 40.0;

    // 泡の最小サイズ
    double m_minSize = 0.0;

    // 泡の最大サイズ
    double m_maxSize = 0.0;

    // 泡発生更新用のカウンタ
    double m_timeAccum = 0.0;

public:

    BubbleTexture() = default;

    // コンストラクタ
    BubbleTexture(const Size& size, double spawnRate, double speed, double minSize, double maxSize)
        : m_texture(size)
        , m_spawnRate(Clamp(spawnRate, 0.0, MaxSpawnRate))
        , m_speed(Max(speed, MinSpeed))
        , m_minSize(minSize)
        , m_maxSize(maxSize)
    {
        // prewarm
        const double prewarmTime = (size.y + m_maxSize * 2) / speed;
        const double prewarmStepTime = 1.0 / 60.0;

        for (int32 i = 0; i < (prewarmTime / prewarmStepTime); ++i)
        {
            update(prewarmStepTime);
        }
    }

    // 泡の更新
    void update(double deltaTime)
    {
        // 泡の移動
        {
            const double speedBase = ((m_minSize + m_maxSize) * 0.5);

            for (auto& bubble : m_bubbles)
            {
                bubble.y -= deltaTime * m_speed * (bubble.r / speedBase);
            }
            m_bubbles.remove_if([maxSize = m_maxSize](const Circle& c) { return c.y < -maxSize; });
        }

        // 泡の発生
        {
            m_timeAccum += deltaTime;

            const double spawnTime = 1.0 / m_spawnRate;

            while (m_timeAccum > spawnTime)
            {
                const RectF spawnArea(-m_maxSize, m_texture.height() + m_maxSize, m_texture.width() + (m_maxSize * 2), m_maxSize);

                m_bubbles.emplace_back(RandomVec2(spawnArea), Random(m_minSize, m_maxSize));

                m_timeAccum -= spawnTime;
            }
        }
    }

    // 泡を内部テクスチャに描画
    void render(const ColorF& backgroundColor)
    {
        m_texture.clear(backgroundColor);
        {
            ScopedRenderTarget2D rt(m_texture);
            ScopedRenderStates2D blend(BlendState::Additive);

            for (const auto& bubble : m_bubbles)
            {
                const double a = EaseInQuad(0.15 + bubble.y / m_texture.height() * 0.45);
                bubble.draw(ColorF(1.0, a));
            }
        }
        Graphics2D::Flush();
        m_texture.resolve();
    }

    // 内部テクスチャを取得
    const Texture& getTexture() const
    {
        return m_texture;
    }
};
