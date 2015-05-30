#include "MapManager.hpp"
#include "../ResourceDefinitions.hpp"
#include <Application.hpp>
#include <ResourceManager.hpp>

MapManager::MapManager(void) :
	m_texture(nullptr)
{
}

MapManager::~MapManager(void)
{
}

void MapManager::init(void)
{
	octo::ResourceManager&	resources = octo::Application::getResourceManager();

	m_collisionManager.init(this);
	// TransitionManager init the biome
	m_transitionManager.init(this, &m_biome);
	m_cameraManager.init(this);
	m_decorManager.init(this, &m_biome);
	m_texture = &resources.getTexture(OCTO_PNG);
}

void MapManager::update(float pf_deltatime)
{
	/*static float accumulator = 0.f;
	static sf::Clock clock;
	float dt = 1.f / 120.f;

	// Always update TransitionManager first
	m_transitionManager.update(pf_deltatime);
	m_staticObjectManager.update(pf_deltatime);

	accumulator += clock.getElapsedTime().asSeconds();
	if (accumulator > 0.2f)
		accumulator = 0.2f;

	while (accumulator > dt)
	{
		m_collisionManager.update(dt);
		accumulator -= dt;
	}

	//float alpha = accumulator / dt;
	clock.restart();
*/
	m_cameraManager.update(pf_deltatime);
	m_transitionManager.update(pf_deltatime);
	m_decorManager.update(pf_deltatime);
	m_collisionManager.update(pf_deltatime);
}

void MapManager::draw(sf::RenderTarget& render, sf::RenderStates states) const
{
	render.draw(m_decorManager);
	render.draw(m_transitionManager);

	/*sf::RectangleShape rect;
	rect.setPosition(sf::Vector2f(m_collisionManager.getPlayer().getGlobalBounds().left, m_collisionManager.getPlayer().getGlobalBounds().top));
	rect.setSize(sf::Vector2f(m_collisionManager.getPlayer().getGlobalBounds().width, m_collisionManager.getPlayer().getGlobalBounds().height));
	render.draw(rect);
	*/
	states.texture = m_texture;
	render.draw(&m_collisionManager.getPlayer().m_vertices[0], 4, sf::Quads, states);
	m_collisionManager.debugDraw(render);
}
