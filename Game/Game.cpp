#include "Game.hpp"
#include "DefaultBiome.hpp"
#include "GenerativeLayer.hpp"
#include "PhysicsEngine.hpp"
#include "AShape.hpp"
#include "RectangleShape.hpp"
#include "MapInstance.hpp"

#include <Application.hpp>
#include <GraphicsManager.hpp>
#include <Camera.hpp>
#include <LevelMap.hpp>
#include <ResourceManager.hpp>
#include <Interpolations.hpp>

Game::Game() :
	m_physicsEngine(PhysicsEngine::getInstance())
{
}

void	Game::setup()
{
	m_biomeManager.registerBiome<DefaultBiome>("test");
	octo::GraphicsManager & graphics = octo::Application::getGraphicsManager();
	graphics.addKeyboardListener(this);
	graphics.addKeyboardListener(&m_octo);
}

void	Game::loadLevel(std::string const& fileName)
{
	(void)fileName;
	// TODO
	m_biomeManager.changeBiome("test", 0x12345);

	m_skyCycle.setup(m_biomeManager.getCurrentBiome());
	m_skyManager.setup(m_biomeManager.getCurrentBiome(), m_skyCycle);
	m_groundManager.setup(m_biomeManager.getCurrentBiome(), m_skyCycle);
	m_parallaxScrolling.setup(m_biomeManager.getCurrentBiome());

	m_physicsEngine.setIterationCount(4u);
	m_physicsEngine.setTileCollision(true);
	m_physicsEngine.setContactListener(this);

	//TODO: put in a function or class NPCManager ?
	auto const & instances = m_biomeManager.getCurrentBiome().getInstances();
	for (auto & instance : instances)
	{
		octo::LevelMap const & levelMap = octo::Application::getResourceManager().getLevelMap(instance.second);
		for (std::size_t i = 0u; i < levelMap.getSpriteCount(); i++)
		{
			octo::LevelMap::SpriteTrigger const & spriteTrigger = levelMap.getSprite(i);
			RectangleShape * rect = m_physicsEngine.createRectangle();
			rect->setPosition(sf::Vector2f(spriteTrigger.trigger.getPosition().x + instance.first * Tile::TileSize - Map::OffsetX, (-levelMap.getMapSize().y + MapInstance::HeightOffset) * Tile::TileSize + spriteTrigger.trigger.getPosition().y - Map::OffsetY));
			rect->setSize(spriteTrigger.trigger.getSize());
			rect->setApplyGravity(false);
			rect->setType(AShape::Type::e_trigger);
		}
	}
}

void	Game::update(sf::Time frameTime)
{
	m_skyCycle.update(frameTime, m_biomeManager.getCurrentBiome());
	m_octo.update(frameTime);
	followPlayer();
	m_groundManager.update(frameTime.asSeconds());
	m_parallaxScrolling.update(frameTime.asSeconds());
	m_physicsEngine.update(frameTime.asSeconds());
	m_skyManager.update(frameTime);
}

void Game::onShapeCollision(AShape * shapeA, AShape * shapeB)
{
	if (shapeB->getGameObject() != nullptr
			&& gameObjectCast<CharacterOcto>(shapeB->getGameObject()) != nullptr
	 && shapeA->getGameObject() != nullptr
			&& gameObjectCast<AGameObject<GameObjectType::Tile>>(shapeA->getGameObject()) != nullptr){
		m_octo.onCollision(GameObjectType::Tile);
	}
	// don't forget to check if shapeA->getGameObject() != nullptr
	// Utiliser gameObjectCast pour réupérer le bon objet avec shapeA->getGameObject()
}

bool Game::onPressed(sf::Event::KeyEvent const & event)
{
	switch (event.code)
	{
		case sf::Keyboard::E:
			m_groundManager.setNextGenerationState(GroundManager::GenerationState::Next);
		break;
		case sf::Keyboard::R:
			m_groundManager.setNextGenerationState(GroundManager::GenerationState::Previous);
		break;
		default:
		break;
	}
	return true;
}

void	Game::draw(sf::RenderTarget& render, sf::RenderStates states)const
{
	render.clear();
	render.draw(m_skyManager.getDecorsBack(), states);
	render.draw(m_parallaxScrolling, states);
	render.draw(m_groundManager.getDecorsBack(), states);
	// Draw pnj
	render.draw(m_octo, states);
	render.draw(m_groundManager.getDecorsFront(), states);
	render.draw(m_groundManager, states);
	render.draw(m_groundManager.getDecorsGround(), states);
	render.draw(m_skyManager.getDecorsFront(), states);

	//m_physicsEngine.debugDraw(render);
}

void	Game::followPlayer()
{
	octo::Camera&	camera = octo::Application::getCamera();
	m_cameraPos = camera.getCenter();
	m_octoPos = m_octo.getPosition();
	sf::Vector2f cameraPos;
	cameraPos.x = octo::linearInterpolation(m_octoPos.x, m_cameraPos.x, 0.98);
	cameraPos.y = octo::linearInterpolation(m_octoPos.y, m_cameraPos.y, 0.9);

	camera.setCenter(cameraPos);
}
