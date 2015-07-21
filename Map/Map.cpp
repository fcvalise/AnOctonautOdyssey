#include "Map.hpp"
#include "ABiome.hpp"
#include "MapInstance.hpp"
#include "FunctionsOffset.hpp"
#include <Application.hpp>
#include <GraphicsManager.hpp>
#include <Interpolations.hpp>
#include <cassert>

Map::Map(void) :
	m_depth(0.f),
	m_oldDepth(0.f),
	m_mapJoinWidth(20.f),
	m_mapJoinHalfWidth(10.f),
	m_width(0u),
	m_height(0u),
	m_offset(nullptr),
	m_mapSurface(nullptr)
{}

Map::~Map(void)
{
	for (auto it = m_tiles.begin(); it != m_tiles.end(); it++)
		delete *it;
}

void Map::init(ABiome & biome)
{
	// Init value from biome
	m_mapSize = biome.getMapSize();
	m_mapJoinWidth = static_cast<float>(m_mapSize.y) / 4.f;
	m_mapJoinHalfWidth = m_mapJoinWidth / 2.f;

	m_width = octo::Application::getGraphicsManager().getVideoMode().width / Tile::TileSize + 4u; // 4 tiles to add margin at left and right
	m_height  = octo::Application::getGraphicsManager().getVideoMode().height / Tile::TileSize + 6u; // 6 tiles to add margin at top and bottom

	m_tiles.resize(m_width, m_height, nullptr);
	for (std::size_t x = 0; x < m_tiles.columns(); x++)
	{
		for (std::size_t y = 0; y < m_tiles.rows(); y++)
		{
			m_tiles.get(x, y) = new Tile();
			m_tiles.get(x, y)->setStartTransition(0u, sf::Vector2f(x * Tile::TileSize, y * Tile::TileSize));
			m_tiles.get(x, y)->setStartTransition(1u, sf::Vector2f(x * Tile::TileSize + Tile::TileSize, y * Tile::TileSize));
			m_tiles.get(x, y)->setStartTransition(2u, sf::Vector2f(x * Tile::TileSize + Tile::TileSize, y * Tile::TileSize + Tile::TileSize));
			m_tiles.get(x, y)->setStartTransition(3u, sf::Vector2f(x * Tile::TileSize, y * Tile::TileSize + Tile::TileSize));
		}
	}

	auto const & instances = biome.getInstances();
	for (auto & instance : instances)
	{
		//std::unique_ptr<MapInstance> ptr(new MapInstance(5, 5, 4));
		//m_instances.push_back(std::unique_ptr<MapInstance>(new MapInstance(6u, 6u, 5u)));
		//instance->load();
	}

	m_noise.setSeed(42);

	// Initialize mapSurface pointer
	setMapSurfaceGenerator([](Noise & noise, float x, float y)
	{
		return noise.fBm(x, y, 3, 3.f, 0.3f);
	});

	// Initialize tileColor pointer
	setTileColorGenerator([](Noise & noise, float x, float y, float z)
	{
		static const sf::Color end = sf::Color(254, 231, 170);
		static const sf::Color start = sf::Color(230, 168, 0);

		float transition = (noise.noise(x / 10.f, y / 10.f, z / 10.f) + 1.f) / 2.f;
		return octo::linearInterpolation(start, end, transition);
	});
}

void Map::computeMapRange(int startX, int endX, int startY, int endY)
{
	float noiseDepth = m_depth / static_cast<float>(m_mapSize.y);
	int height; // The height of the generated map
	int offsetX; // The tile position adjust to avoid negativ offset (because map is circular)
	int offsetY;
	int curOffsetX = static_cast<int>(m_curOffset.x / Tile::TileSize);
	int curOffsetY = static_cast<int>(m_curOffset.y / Tile::TileSize);
	int offsetPosX; // The real position of the tile (in the world)
	MapInstance * curInstance;

	assert(m_mapSurface);
	assert(m_tileColor);

	float startTransitionX = (m_mapSurface((static_cast<float>(m_mapSize.x) - m_mapJoinHalfWidth) / static_cast<float>(m_mapSize.x), noiseDepth) + 1.f) * static_cast<float>(m_mapSize.y) / 2.f;
	float endTransitionX = (m_mapSurface(m_mapJoinHalfWidth / static_cast<float>(m_mapSize.x), noiseDepth) + 1.f) * static_cast<float>(m_mapSize.y) / 2.f;

	for (int x = startX; x < endX; x++)
	{
		curInstance = nullptr;
		offsetX = x + curOffsetX;
		offsetPosX = offsetX;
		while (offsetX < 0)
			offsetX += m_mapSize.x;
		while (offsetX >= static_cast<int>(m_mapSize.x))
			offsetX -= m_mapSize.x;

		for (auto & instance : m_instances)
		{
			if (offsetX >= static_cast<int>(instance->getCornerPositions().left) && offsetX < static_cast<int>(instance->getCornerPositions().width))
			{
				curInstance = instance.get();
				break;
			}
		}
		// Check if we are at the transition between 0 and m_mapSize.x
		if (offsetX < static_cast<int>(m_mapJoinHalfWidth) || offsetX >= (static_cast<int>(m_mapSize.x) - static_cast<int>(m_mapJoinHalfWidth)))
		{
			float transition = offsetX < static_cast<int>(m_mapJoinHalfWidth) ? static_cast<float>(offsetX) + m_mapJoinHalfWidth : m_mapJoinHalfWidth - static_cast<float>(m_mapSize.x) + static_cast<float>(offsetX);
			height =  octo::cosinusInterpolation(startTransitionX, endTransitionX, transition / m_mapJoinWidth);
		}
		else
		{
			// mapSurface return a value between -1 & 1
			// we normalize it betwen 0 & max_height
			height = static_cast<int>((m_mapSurface(static_cast<float>(offsetX) / static_cast<float>(m_mapSize.x), noiseDepth) + 1.f) * static_cast<float>(m_mapSize.y) / 2.f);
		}
		for (int y = startY; y < endY; y++)
		{
			offsetY = y + curOffsetY;
			// Init square
			m_tiles.get(x, y)->setStartTransition(0u, sf::Vector2f((offsetPosX) * Tile::TileSize, (offsetY) * Tile::TileSize));
			m_tiles.get(x, y)->setStartTransition(1u, sf::Vector2f((offsetPosX + 1) * Tile::TileSize, (offsetY) * Tile::TileSize));
			m_tiles.get(x, y)->setStartTransition(2u, sf::Vector2f((offsetPosX + 1) * Tile::TileSize, (offsetY + 1) * Tile::TileSize));
			m_tiles.get(x, y)->setStartTransition(3u, sf::Vector2f((offsetPosX) * Tile::TileSize, (offsetY + 1) * Tile::TileSize));

			if (curInstance && offsetY >= static_cast<int>(curInstance->getCornerPositions().top) && offsetY < static_cast<int>(curInstance->getCornerPositions().height))
				m_tiles.get(x, y)->setIsEmpty(curInstance->get(offsetX - curInstance->getCornerPositions().left, offsetY - curInstance->getCornerPositions().top).isEmpty());
			else if (offsetY < height)
			{
				m_tiles.get(x, y)->setIsEmpty(true);
				continue;
			}
			else
				m_tiles.get(x, y)->setIsEmpty(false);
			m_tiles.get(x, y)->setStartColor(m_tileColor(static_cast<float>(offsetPosX), static_cast<float>(offsetY), noiseDepth));
		}
	}
}

void Map::computeDecor(void)
{
	float noiseDepth = m_depth / static_cast<float>(m_mapSize.y);
	int height;
	int curOffsetX = static_cast<int>(m_curOffset.x / Tile::TileSize);

	assert(m_mapSurface);

	float startTransitionX = (m_mapSurface((static_cast<float>(m_mapSize.x) - m_mapJoinHalfWidth) / static_cast<float>(m_mapSize.x), noiseDepth) + 1.f) * static_cast<float>(m_mapSize.y) / 2.f;
	float endTransitionX = (m_mapSurface(m_mapJoinHalfWidth / static_cast<float>(m_mapSize.x), noiseDepth) + 1.f) * static_cast<float>(m_mapSize.y) / 2.f;

	for (auto it = m_decorPositions.begin(); it != m_decorPositions.end(); it++)
	{
		// Check if we are at the transition between 0 and m_mapSize.x
		if (it->first < static_cast<int>(m_mapJoinHalfWidth) || it->first >= (static_cast<int>(m_mapSize.x) - static_cast<int>(m_mapJoinHalfWidth)))
		{
			float transition = it->first < static_cast<int>(m_mapJoinHalfWidth) ? static_cast<float>(it->first) + m_mapJoinHalfWidth : m_mapJoinHalfWidth - static_cast<float>(m_mapSize.x) + static_cast<float>(it->first);
			height =  octo::cosinusInterpolation(startTransitionX, endTransitionX, transition / m_mapJoinWidth);
		}
		else
		{
			// mapSurface return a value between -1 & 1
			// we normalize it betwen 0 & max_height
			height = static_cast<int>((m_mapSurface(static_cast<float>(it->first) / static_cast<float>(m_mapSize.x), noiseDepth) + 1.f) * static_cast<float>(m_mapSize.y) / 2.f);
		}
		it->second.x = Tile::TileSize * static_cast<float>(getCircleOffset(curOffsetX, it->first, static_cast<int>(m_tiles.columns()), static_cast<int>(m_mapSize.x)));
		it->second.y = Tile::TileSize * static_cast<float>(height);
	}
}

void Map::registerDecor(int x)
{
	m_decorPositions.emplace_back(std::pair<int, sf::Vector2f>(x, sf::Vector2f()));
}

void Map::swapDepth(void)
{
	float tmp = m_depth;
	m_depth = m_oldDepth;
	m_oldDepth = tmp;
	for (auto & instance : m_instances)
		instance->swapDepth();
}

void Map::registerDepth(void)
{
	m_oldDepth = m_depth;
	for (auto & instance : m_instances)
		instance->registerDepth();
}

void Map::nextStep(void)
{
	m_depth += 3.f;
	for (auto & instance : m_instances)
		instance->nextStep();
}

void Map::previousStep(void)
{
	m_depth -= 3.f;
	for (auto & instance : m_instances)
		instance->previousStep();
}

void Map::setMapSurfaceGenerator(MapSurfaceGenerator mapSurface)
{
	m_mapSurface = std::bind(mapSurface, std::ref(m_noise), std::placeholders::_1, std::placeholders::_2);
}

void Map::setTileColorGenerator(TileColorGenerator tileColor)
{
	m_tileColor = std::bind(tileColor, std::ref(m_noise), std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
}

void Map::addOffsetX(int offsetX)
{
	Tile *	m_tmp[m_tiles.rows()];

	if (offsetX > 0)
	{
		for (std::size_t y = 0; y < m_tiles.rows(); y++)
			m_tmp[y] = m_tiles(0, y);
		for (std::size_t x = 0; x < m_tiles.columns() - 1; x++)
		{
			for (std::size_t y = 0; y < m_tiles.rows(); y++)
				m_tiles(x, y) = m_tiles(x + 1, y);
		}
		for (std::size_t y = 0; y < m_tiles.rows(); y++)
			m_tiles(m_tiles.columns() - 1, y) = m_tmp[y];
		addOffsetX(offsetX - 1);
	}
	else if (offsetX < 0)
	{
		for (std::size_t y = 0; y < m_tiles.rows(); y++)
			m_tmp[y] = m_tiles(m_tiles.columns() - 1, y);
		for (std::size_t x = m_tiles.columns() - 1; x > 0; x--)
		{
			for (std::size_t y = 0; y < m_tiles.rows(); y++)
				m_tiles(x, y) = m_tiles(x - 1, y);
		}
		for (std::size_t y = 0; y < m_tiles.rows(); y++)
			m_tiles(0, y) = m_tmp[y];
		addOffsetX(offsetX + 1);
	}
}

void Map::addOffsetY(int offsetY)
{
	Tile *	m_tmp[m_tiles.columns()];

	if (offsetY > 0)
	{
		for (std::size_t x = 0; x < m_tiles.columns(); x++)
			m_tmp[x] = m_tiles(x, 0);
		for (std::size_t y = 0; y < m_tiles.rows() - 1; y++)
		{
			for (std::size_t x = 0; x < m_tiles.columns(); x++)
				m_tiles(x, y) = m_tiles(x, y + 1);
		}
		for (std::size_t x = 0; x < m_tiles.columns(); x++)
			m_tiles(x, m_tiles.rows() - 1) = m_tmp[x];
		addOffsetY(offsetY - 1);
	}
	else if (offsetY < 0)
	{
		for (std::size_t x = 0; x < m_tiles.columns(); x++)
			m_tmp[x] = m_tiles(x, m_tiles.rows() - 1);
		for (std::size_t y = m_tiles.rows() - 1; y > 0; y--)
		{
			for (std::size_t x = 0; x < m_tiles.columns(); x++)
				m_tiles(x, y) = m_tiles(x, y - 1);
		}
		for (std::size_t x = 0; x < m_tiles.columns(); x++)
			m_tiles(x, 0) = m_tmp[x];
		addOffsetY(offsetY + 1);
	}
}
