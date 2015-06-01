#include "MapInstance.hpp"
#include "OctoNoise.hpp"
#include <cmath>
#include <Interpolations.hpp>

MapInstance::MapInstance(void) :
	Map(),
	m_instance(nullptr)
{}

MapInstance::~MapInstance(void) { }

void MapInstance::initBiome(void)
{
	m_biome->mn_height = 128u;
	m_biome->mn_width = 512u;
	m_biome->mn_startOffsetX = 0u;
	m_biome->mf_transitionTimerMax = 0.5f;
	m_biome->mn_nbDecor = 15u;
	m_biome->mn_temperature = 1;
	m_biome->mn_wind = 300;
	m_instance = new StaticTileObject(140u, 40u, 5u);
	m_instance->load();
	mn_totalWidth = m_biome->mn_width + m_instance->getWidth();
	m_biome->mn_totalWidth = mn_totalWidth;

	// Decors values
	m_biome->m_tree.mn_nb = 10;
	m_biome->m_tree.mn_minSizeX = 20;
	m_biome->m_tree.mn_maxSizeX = 50;
	m_biome->m_tree.mn_minSizeY = 80;
	m_biome->m_tree.mn_maxSizeY = 100;
	m_biome->m_tree.mn_minAngle = 15;
	m_biome->m_tree.mn_maxAngle = 75;
	m_biome->m_tree.mn_minLive = 10;
	m_biome->m_tree.mn_maxLive = 15;

	m_biome->m_crystal.mn_nb = 10;
	m_biome->m_crystal.mn_minSizeX = 10;
	m_biome->m_crystal.mn_maxSizeX = 30;
	m_biome->m_crystal.mn_minSizeY = 80;
	m_biome->m_crystal.mn_maxSizeY = 150;
	m_biome->m_crystal.mn_minElement = 3;
	m_biome->m_crystal.mn_maxElement = 6;

	m_biome->m_rock.mn_nb = 10;
	m_biome->m_rock.mn_minSizeX = 10;
	m_biome->m_rock.mn_maxSizeX = 20;
	m_biome->m_rock.mn_minSizeY = 150;
	m_biome->m_rock.mn_maxSizeY = 400;
	m_biome->m_rock.mn_minElement = 4;
	m_biome->m_rock.mn_maxElement = 15;

	//m_biome->m_star.mn_nb = 10;
	m_biome->m_star.mn_minSizeX = 70;
	m_biome->m_star.mn_maxSizeX = 100;
	m_biome->m_star.mn_minSizeY = 70;
	m_biome->m_star.mn_maxSizeY = 100;
	m_biome->m_star.mn_minLive = 5;
	m_biome->m_star.mn_maxLive = 15;

	m_biome->m_sun.mn_nb = 2;
	m_biome->m_sun.mn_minSizeX = 70;
	m_biome->m_sun.mn_maxSizeX = 100;

	m_biome->m_cloud.mn_nb = 3;
	m_biome->m_cloud.mn_minSizeX = 150;
	m_biome->m_cloud.mn_maxSizeX = 400;
	m_biome->m_cloud.mn_minSizeY = 10;
	m_biome->m_cloud.mn_maxSizeY = 20;
	m_biome->m_cloud.mn_minElement = 4;
	m_biome->m_cloud.mn_maxElement = 15;
	m_biome->m_tree.mn_minLive = 10;
	m_biome->m_tree.mn_maxLive = 15;
}

void MapInstance::computeMapRange(int p_startX, int p_endX, int p_startY, int p_endY)
{
	float vec[3];
	int height;
	int offsetInstance;
	int offset;
	float v;
	bool inInstance;
	int offsetX = static_cast<int>(m_curOffset.x / Tile::TileSize);
	int offsetY = static_cast<int>(m_curOffset.y / Tile::TileSize);
	int offsetPosX;

	for (int x = p_startX; x < p_endX; x++)
	{
		inInstance = false;
		offset = x + offsetX;
		offsetPosX = offset;
		while (offset < 0)
			offset += mn_totalWidth;
		while (offset >= static_cast<int>(mn_totalWidth))
			offset -= mn_totalWidth;
		if (offset >= static_cast<int>(m_biome->mn_width) && offset < static_cast<int>(mn_totalWidth))
			inInstance = true;

		if (inInstance)
			vec[0] = static_cast<float>(m_biome->mn_width);
		else
			vec[0] = static_cast<float>(offset);

		vec[1] = m_depth;
		// firstCurve return a value between -1 & 1
		// we normalize it betwen 0 & max_height
		v = (firstCurve(vec) + 1.f) * static_cast<float>(m_biome->mn_height) / 2.f;
		offsetInstance = height = static_cast<int>(v);
		for (int y = p_startY; y < p_endY; y++)
		{
			offsetY = y + static_cast<int>(m_curOffset.y / Tile::TileSize);
			// Init square
			m_tiles.get(x, y)->m_startTransition[0] = sf::Vector2f((offsetPosX) * Tile::TileSize, (offsetY) * Tile::TileSize);
			m_tiles.get(x, y)->m_startTransition[1] = sf::Vector2f((offsetPosX + 1) * Tile::TileSize, (offsetY) * Tile::TileSize);
			m_tiles.get(x, y)->m_startTransition[2] = sf::Vector2f((offsetPosX + 1) * Tile::TileSize, (offsetY + 1) * Tile::TileSize);
			m_tiles.get(x, y)->m_startTransition[3] = sf::Vector2f((offsetPosX) * Tile::TileSize, (offsetY + 1) * Tile::TileSize);
			float yy = offsetY - offsetInstance;
			if (inInstance && yy >= 0 && yy < m_instance->getHeight())
				m_tiles.get(x, y)->mb_isEmpty = m_instance->get(offset - m_biome->mn_width, yy).mb_isEmpty;
			else if (offsetY < height || offsetY < 0)
			{
				m_tiles.get(x, y)->mb_isEmpty = true;
				continue;
			}
			else
				m_tiles.get(x, y)->mb_isEmpty = false;
			vec[0] = static_cast<float>(x + offsetX);
			vec[1] = static_cast<float>(offsetY);
			vec[2] = m_depth;
			// secondCurve return a value between -1 & 1
			m_tiles.get(x, y)->mf_noiseValue = (secondCurve(vec) + 1.f) / 2.f;
			setColor(*m_tiles.get(x, y));
		}
	}
}

float MapInstance::firstCurve(float * vec)
{
	vec[0] /= 100.f;
	vec[1] /= 100.f;
	return OctoNoise::getCurrent().fbm(vec, 3, 2.0f, 0.4f);
}

float MapInstance::secondCurve(float * vec)
{
	vec[0] /= 10.f;
	vec[1] /= 10.f;
	vec[2] /= 10.f;
	return OctoNoise::getCurrent().noise3(vec);
	//return sin(vec[0] * 15.f + OctoNoise::getCurrent().noise3(vec) * sin(vec[1]) * 5.f);
}

void MapInstance::setColor(Tile & p_tile)
{
	sf::Color start = sf::Color(178.f, 0.f, 86.f);
	sf::Color end = sf::Color(178.f, 162.f, 32.f);
	sf::Color mid = sf::Color(0.f, 74.f, 213.f);

	start = octo::linearInterpolation(end, mid, p_tile.mf_noiseValue);

	if (p_tile.mf_noiseValue < 0.4f)
		p_tile.m_startColor = start;
	else if (p_tile.mf_noiseValue < 0.6f)
	{
		p_tile.m_startColor = octo::linearInterpolation(start, end, (p_tile.mf_noiseValue - 0.4f) * 5.f);
	}
	else
		p_tile.m_startColor = end;
}

void MapInstance::swapDepth(void)
{
	float tmp = m_depth;
	m_depth = m_oldDepth;
	m_oldDepth = tmp;
	m_instance->swapDepth();
}

void MapInstance::registerDepth(void)
{
	m_oldDepth = m_depth;
	m_instance->registerDepth();
}

void MapInstance::nextStep(void)
{
	m_depth += 3.f;
	m_instance->nextStep();
}

void MapInstance::previousStep(void)
{
	m_depth -= 3.f;
	m_instance->previousStep();
}
