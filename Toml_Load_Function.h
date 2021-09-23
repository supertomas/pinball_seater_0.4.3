#pragma once
#include<Siv3D.hpp>
#include"Enemy.h"

struct SpinnerData
{
	Vec2 pos;
	Vec2 adjust;
	Vec2 size;
};

class TOMLConfig
{
private:

	FilePath m_tomlPath;
	TOMLReader m_toml;
	DirectoryWatcher m_watcher;

public:

	/// @brief TOML コンフィグクラスを初期化する
	/// @param path コンフィグで使用する TOML ファイルのパス 
	TOMLConfig(const FilePath& path)
	{
		m_tomlPath = FileSystem::FullPath(path);
		m_toml = TOMLReader(m_tomlPath);
		m_watcher = DirectoryWatcher(FileSystem::ParentPath(m_tomlPath));
	}

	/// @brief TOML コンフィグクラスが使えるかを返す。
	/// @return 使える場合 true, それ以外の場合は false
	explicit operator bool() const
	{
		return m_toml.isOpen();
	}

	/// @brief TOML ファイルが更新されたかを返す
	/// @return 更新されている場合 true, それ以外の場合は false
	bool hasUpdate() const
	{
		for (const auto& change : m_watcher.retrieveChanges())
		{
			if ((change.first == m_tomlPath)
				&& (change.second == FileAction::Modified || change.second == FileAction::Added))
			{
				return true;
			}
		}

		return false;
	}

	/// @brief TOML ファイルをリロードする
	/// @return リロードに成功した場合 true, それ以外の場合は false
	bool reload()
	{
		m_toml = TOMLReader(m_tomlPath);

		return m_toml.isOpen();
	}

	RectF loadStraightBumper() const
	{
		RectF data(0, 0, 0, 0);
		data.x = m_toml[U"straightbumper.pos.x"].get<double>();
		data.y = m_toml[U"straightbumper.pos.y"].get<double>();
		data.w = m_toml[U"straightbumper.size.w"].get<double>();
		data.h = m_toml[U"straightbumper.size.h"].get<double>();
		return data;
	}

	Array<RectF> LoadItemRect() const
	{
		Array<RectF> alldata;
		for (const auto& object : m_toml[U"ItemRect"].tableArrayView())
		{
			RectF data;
			data.pos = { object[U"pos.x"].get<double>(),object[U"pos.y"].get<double>() };
			data.size = { object[U"size.w"].get<double>(),object[U"size.h"].get<double>() };
			alldata << data;
		}
		return alldata;
	}


	Array<Array<Vec2>> LoadStraightFramePos() const
	{
		Array<Array<Vec2>> Allpoints;
		Array<Vec2> points;

		for (const auto& alldata : m_toml[U"Alldata"].tableArrayView())
		{
			for (const auto& object : alldata[U"data"].tableArrayView())
			{
				Vec2 pos;
				pos = Vec2(object[U"pos.x"].get<double>(), object[U"pos.y"].get<double>());
				points << pos;
			}
			Allpoints << points;
			points.clear();
		}
		return Allpoints;
	}

	Array<OffsetCircular> LoadRoundFrameData(int32& i) const
	{
		Array<OffsetCircular> alldata;

		for (const auto& object : m_toml[U"round_data"].tableArrayView())
		{
			Vec2 pos;
			double_t r;
			double_t angle;
			OffsetCircular data;

			data = OffsetCircular(Vec2(object[U"pos.x"].get<double_t>(), object[U"pos.y"].get<double_t>())
				, object[U"r"].get<double_t>(), i * object[U"angle"].get<double_t>() * 1_deg);
			alldata << data;
		}
		return alldata;
	}

	Array<Point> LoadgetRange() const
	{
		Array<Point> range_data;
		Point range;

		for (const auto& object : m_toml[U"range"].tableArrayView())
		{
			range = { object[U"pos.x"].get<int32>(),object[U"pos.y"].get<int32>() };
			range_data << range;
		}

		return range_data;
	}

	Array<Triangle> LoadTriangleData() const
	{
		Array<Triangle> try_data;
		Triangle triangle;

		for (const auto& object : m_toml[U"triangle"].tableArrayView())
		{
			triangle = Triangle(object[U"pos.x1"].get<double_t>(), object[U"pos.y1"].get<double_t>(),
				object[U"pos.x2"].get<double_t>(), object[U"pos.y2"].get<double_t>(),
				object[U"pos.x3"].get<double_t>(), object[U"pos.y3"].get<double_t>());
			try_data << triangle;
		}
		return try_data;
	}

	Array<Vec2> LoadRoundbumperData() const
	{
		Array<Vec2> Alldata;

		for (const auto& object : m_toml[U"roundbumper"].tableArrayView())
		{
			Vec2  data;
			data = Vec2(object[U"pos.x"].get<double_t>(), object[U"pos.y"].get<double_t>());
			Alldata << data;
		}
		return Alldata;
	}
	SpinnerData LoadSpinner() const
	{
		SpinnerData spinner;
		spinner.pos.x = m_toml[U"spinnerdata.pos.x"].get<double>();
		spinner.pos.y = m_toml[U"spinnerdata.pos.y"].get<double>();
		spinner.adjust = { m_toml[U"spinnerdata.adjust.x"].get<double>(),m_toml[U"spinnerdata.adjust.y"].get<double>() };
		spinner.size = { m_toml[U"spinnerdata.size.x"].get<double>(),m_toml[U"spinnerdata.size.y"].get<double>(), };
		return spinner;
	}

	Array<EnemyData> LoadEnemyData() const
	{
		Array<EnemyData> enemy_data;
		for (const auto& object : m_toml[U"enemydata"].tableArrayView())
		{
			EnemyData enemy;
			enemy.rect = RectF(object[U"pos.x"].get<double>(), object[U"pos.y"].get<double>(),
				object[U"size.w"].get<double>(), object[U"size.h"].get<double>());
			enemy.error = Vec2(object[U"error.x"].get<double>(), object[U"error.y"].get<double>());
			enemy.angle = int32(object[U"angle"].get<int32>());
			enemy_data << enemy;
		}
		return enemy_data;
	}


};