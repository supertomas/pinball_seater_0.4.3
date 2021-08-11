#pragma once
#include<Siv3D.hpp>

RectF LoadStraightBumper(const FilePath& tomlPath)
{
    RectF data;
    const TOMLReader toml(tomlPath);
    data.pos = { 0,0 };
    data.size = { 0,0 };
    if (!toml)
    {
        return data;
    }

    data.pos = { toml[U"straightbumper.pos.x"].get<double_t>(),toml[U"straightbumper.pos.y"].get<double_t>() };
    data.size = { toml[U"straightbumper.size.w"].get<double_t>(),toml[U"straightbumper.size.h"].get<double_t>() };

    return data;
}

Array<RectF> LoadItemRect(const FilePath& tomlPath)
{
    Array<RectF> alldata;
    const TOMLReader toml(tomlPath);

    if (!toml)
    {
        return alldata;
    }
    for (const auto& object : toml[U"ItemRect"].tableArrayView())
    {
        RectF data;
        data.pos = { object[U"pos.x"].get<double_t>(),object[U"pos.y"].get<double_t>() };
        data.size = { object[U"size.w"].get<double_t>(),object[U"size.h"].get<double_t>() };
        alldata << data;
    }
    return alldata;
}

Array<Array<Vec2>> LoadStraightFramePos(const FilePath& tomlPath)
{

    Array<Array<Vec2>> Allpoints;
    Array<Vec2> points;
    const TOMLReader toml(tomlPath);

    if (!toml)
    {
        return Allpoints;
    }
    for (const auto& alldata : toml[U"Alldata"].tableArrayView())
    {
        for (const auto& object : alldata[U"data"].tableArrayView())
        {
            Vec2 pos;
            pos = Vec2(object[U"pos.x"].get<double_t>(), object[U"pos.y"].get<double_t>());
            points << pos;
        }
        Allpoints << points;
        points.clear();
    }
    return Allpoints;
}

Array<OffsetCircular> LoadRoundFrameData(const FilePath& tomlPath, int32& i)
{
    Array<OffsetCircular> alldata;
    const TOMLReader toml(tomlPath);

    if (!toml)
    {
        return alldata;
    }
    for (const auto& object : toml[U"round_data"].tableArrayView())
    {
        Vec2 pos;
        double_t r;
        int32 angle;
        OffsetCircular data;

        data = OffsetCircular(Vec2(object[U"pos.x"].get<double_t>(), object[U"pos.y"].get<double_t>())
            , object[U"r"].get<double_t>(), i * object[U"angle"].get<int32>() * 1_deg);
        alldata << data;
    }
    return alldata;
}

Array<Point> LoadgetRange(const FilePath& tomlPath)
{
    Array<Point> range_data;
    const TOMLReader toml(tomlPath);

    if (!toml)
    {
        return range_data;
    }
    for (const auto& object : toml[U"range"].tableArrayView())
    {
        Point range;

        range = { object[U"pos.x"].get<int32>(),object[U"pos.y"].get<int32>() };
        range_data << range;
    }
    return range_data;
}

Array<Triangle> LoadTriangleData(const FilePath& tomlPath)
{
    Array<Triangle> try_data;
    const TOMLReader toml(tomlPath);
    Triangle triangle;

    if (!toml)
    {
        return try_data;
    }

    for (const auto& object : toml[U"triangle"].tableArrayView())
    {
        triangle = Triangle(object[U"pos.x1"].get<double_t>(), object[U"pos.y1"].get<double_t>(),
            object[U"pos.x2"].get<double_t>(), object[U"pos.y2"].get<double_t>(),
            object[U"pos.x3"].get<double_t>(), object[U"pos.y3"].get<double_t>());
        try_data << triangle;
    }
    return try_data;

}

Array<Vec2> LoadRoundbumperData(const FilePath& tomlPath)
{
    Array<Vec2> Alldata;
    const TOMLReader toml(tomlPath);

    if (!toml)
    {
        return Alldata;
    }

    for (const auto& object : toml[U"roundbumper"].tableArrayView())
    {
        Vec2  data;
        data = Vec2(object[U"pos.x"].get<double_t>(), object[U"pos.y"].get<double_t>());
        Alldata << data;
    }
    return Alldata;
}

struct EnemyData
{
    RectF rect;
    Vec2 error;
    int32 angle;
};

Array<EnemyData> LoadEnemyData(const FilePath& tomlPath)
{
    Array<EnemyData> enemy_data;
    const TOMLReader toml(tomlPath);

    if (!toml)
    {
        return enemy_data;
    }

    for (const auto& object : toml[U"enemydata"].tableArrayView())
    {
        EnemyData enemy;
        enemy.rect = RectF(object[U"pos.x"].get<double_t>(), object[U"pos.y"].get<double_t>(),
            object[U"size.w"].get<double_t>(), object[U"size.h"].get<double_t>());
        enemy.error = Vec2(object[U"error.x"].get<double_t>(), object[U"error.y"].get<double_t>());
        enemy.angle = int32(object[U"angle"].get<int32>());
        enemy_data << enemy;
    }
    return enemy_data;
}