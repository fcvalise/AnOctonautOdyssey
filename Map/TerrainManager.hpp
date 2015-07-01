#ifndef TERRAINMANAGER_HPP
# define TERRAINMANAGER_HPP

# include "Map.hpp"
# include "DecorManager.hpp"

class ADecor;
class ABiome;
class TileShape;

class TerrainManager : public sf::Drawable
{
public:
	TerrainManager(void);
	virtual ~TerrainManager(void) = default;

	inline sf::Vertex * getHeight(int x) { m_tiles->getHeight(x); return m_tilesPrev->getHeight(x); }
	// Only used by StaticObjectManager to compute initial position
	inline void computeDecor(void) { m_tiles->computeDecor(); }

	void init(Biome * biome);
	void update(float pf_deltatime);
	void draw(sf::RenderTarget& render, sf::RenderStates states) const;

private:
	std::unique_ptr<Map>				m_tiles;
	std::unique_ptr<Map>				m_tilesPrev;
	float								m_transitionTimer;
	float								m_transitionTimerMax;
	sf::Vector2f						m_offset;
	std::unique_ptr<sf::Vertex[]>		m_vertices;
	std::size_t							m_verticesCount;
	sf::Vector2i						m_oldOffset;
	octo::Array2D<TileShape *>			m_tileShapes;
	DecorManager						m_decorManager;
	std::vector<DecorManager::Iterator>	m_decors;

	void initDecors(ABiome * biome);
	inline void defineTransition(void) { defineTransitionRange(0, m_tiles->getColumns(), 0, m_tiles->getRows()); }
	void defineTransitionRange(int startX, int endX, int startY, int endY);
	void defineTransition(int x, int y);
	void defineTransitionBorderTileRange(int startX, int endX, int startY, int endY);
	void setTransitionAppear(int x, int y);
	void setTransitionDisappear(int x, int y);
	void setTransitionModify(int x, int y);

	void swapMap(void);
	void updateOffset(float pf_deltatime);
	void updateTransition(void);

};

#endif
