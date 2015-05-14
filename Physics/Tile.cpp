#include "Tile.hpp"

Tile::Tile(void) :
	Polygon(4u, CollideType::e_tile),
	mf_noiseValue(0.f),
	mb_isEmpty(true),
	me_transition(e_transition_none),
	mp_upLeft(nullptr),
	m_startColor(sf::Color::White)
{
}

Tile::~Tile(void) {}

bool Tile::isEmpty(void)
{
	return mb_isEmpty;
}

void Tile::lightCopy(Tile const & p_tile)
{
	mb_isEmpty = p_tile.mb_isEmpty;
}

void Tile::setUpLeft(sf::Vertex * vertex)
{
	m_recompute = true;
	mp_upLeft = vertex;
}

void Tile::copy(Tile const & p_tile, float p_offsetX, float p_offsetY)
{
	mf_noiseValue = p_tile.mf_noiseValue;
	mb_isEmpty = p_tile.mb_isEmpty;
	me_transition = p_tile.me_transition;
	m_startColor = p_tile.m_startColor;
	for (int i = 0; i < 4; i++)
	{
		m_startTransition[i].x = p_tile.m_startTransition[i].x + p_offsetX;
		m_startTransition[i].y = p_tile.m_startTransition[i].y + p_offsetY;
	}
}

void Tile::computeEdges(void)
{
	if (!m_recompute)
		return;
	m_recompute = false;
	for (std::size_t i = 0u; i < 4u; i++)
	{
		m_edges[i].x = mp_upLeft[i].position.x - mp_upLeft[(i + 1u) % 4u].position.x;
		m_edges[i].y = mp_upLeft[i].position.y - mp_upLeft[(i + 1u) % 4u].position.y;
		m_normals[i].x = -m_edges[i].y;
		m_normals[i].y = m_edges[i].x;
	}
}

void Tile::computeCenter(void)
{
	if (!m_recomputeCenter)
		return;
	m_recomputeCenter = false;
	m_center.x = 0.f;
	m_center.y = 0.f;
	std::size_t nbVertice = 0u;
	for (std::size_t i = 0u; i < 4u; i++)
	{
		if (mp_upLeft[i].position == mp_upLeft[(i + 1u) % 4u].position)
			continue;
		m_center.x += mp_upLeft[i].position.x;
		m_center.y += mp_upLeft[i].position.y;
		nbVertice += 1u;
	}
	m_center.x /= nbVertice;
	m_center.y /= nbVertice;
}

sf::Vector2f const & Tile::getVertex(std::size_t index) const
{
	return mp_upLeft[index].position;
}
