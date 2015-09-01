#include "PhysicsMapScreen.hpp"
#include "ConvexShape.hpp"
#include "RectangleShape.hpp"
#include "CircleShape.hpp"
#include "GroupShape.hpp"
#include "GenerativeLayer.hpp"
#include <Application.hpp>
#include <GraphicsManager.hpp>
#include <Options.hpp>

PhysicsMapScreen::PhysicsMapScreen(void) :
	m_engine(PhysicsEngine::getInstance()),
	m_camera(octo::Application::getCamera()),
	m_shape(nullptr),
	m_groupShape(nullptr),
	m_nbCollision(0u)
{}

void	PhysicsMapScreen::start()
{
	octo::GraphicsManager & graphics = octo::Application::getGraphicsManager();
	graphics.addKeyboardListener(this);

	m_groundManager.setup(m_biome, m_cycle);

	m_engine.setIterationCount(octo::Application::getOptions().getValue<std::size_t>("iteration_count"));
	m_engine.setTileCollision(true);
	m_engine.setContactListener(this);

	m_shape = m_engine.createConvex();
	m_shape->setPosition(300.f, 600.f);
	m_shape->setApplyGravity(false);
	m_shape->setVertexCount(4u);
	m_shape->setOrigin(sf::Vector2f(10.f, 30.f));
	m_shape->setPosition(sf::Vector2f(460.f, 350.f));
	m_shape->setVertex(0u, sf::Vector2f(0.f, 0.f));
	m_shape->setVertex(1u, sf::Vector2f(40.f, 0.f));
	m_shape->setVertex(2u, sf::Vector2f(40.f, 60.f));
	m_shape->setVertex(3u, sf::Vector2f(0.f, 60.f));
	m_shape->setCollisionMask(2u);
	m_shape->setCollisionType(1u);

	for (std::size_t i = 0u; i < 10; i++)
	{
		RectangleShape * rect = m_engine.createRectangle();
		if (i == 1)
			rect->setApplyGravity(false);
		else
			rect->setApplyGravity(true);
		rect->setPosition(sf::Vector2f(560.f + i * 50.f, 350.f));
		rect->setSize(sf::Vector2f(40.f, 60.f));
		m_shapes.push_back(rect);
	}

	m_groupShape = m_engine.createGroupShape();
	m_groupShape->setSize(100.f, 50.f);
	m_groupShape->setApplyGravity(false);
	m_groupShape->setPosition(100.f, 500.f);
	m_groupShape->setOrigin(sf::Vector2f(30.f, 15.f));
	m_groupShape->setCollisionMask(1u);
	m_groupShape->setCollisionType(2u);
	RectangleShape * r = m_groupShape->addRectangleShape();
	r->setSize(20.f, 20.f);
	r->setPosition(-20.f, -20.f);
	r->setOrigin(sf::Vector2f(10.f, 10.f));
	r->setRotation(3.14f / 3.f);
	r = m_groupShape->addRectangleShape();
	r->setSize(20.f, 40.f);
	r->setPosition(30.f, -60.f);
	r->setOrigin(sf::Vector2f(10.f, 20.f));
	r->setRotation(3.14f / 5.f);
	r = m_groupShape->addRectangleShape();
	r->setSize(20.f, 40.f);
	r->setPosition(60.f, 60.f);
	r->setOrigin(sf::Vector2f(10.f, 20.f));
	r->setRotation(3.14f / 2.31f);

	CircleShape * c = m_groupShape->addCircleShape();
	c->setRadius(10.f);
	c->setPosition(120.f, 60.f);

	CircleShape * circle = m_engine.createCircle();
	circle->setRadius(260.f);
	circle->setPosition(100.f, 300.f);

	m_engine.unregisterShape(m_shapes[5]);
}

void	PhysicsMapScreen::pause()
{
}

void	PhysicsMapScreen::resume()
{
}

void	PhysicsMapScreen::stop()
{
}

void	PhysicsMapScreen::update(sf::Time deltatime)
{
	float speed = 200.f;
	float cameraSpeed = 500.f * deltatime.asSeconds();

	m_nbCollision = 0u;
	m_groundManager.update(deltatime.asSeconds());

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Z))
		m_groupShape->addVelocity(0.f, -speed);
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
		m_groupShape->addVelocity(0.f, speed);
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
		m_groupShape->addVelocity(speed, 0.f);
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Q))
		m_groupShape->addVelocity(-speed, 0.f);

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
		m_camera.move(-cameraSpeed, 0.f);
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
		m_camera.move(cameraSpeed, 0.f);
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
		m_camera.move(0.f, -cameraSpeed);
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
		m_camera.move(0.f, cameraSpeed);

	m_engine.update(deltatime.asSeconds());
}

void PhysicsMapScreen::onShapeCollision(AShape * shapeA, AShape * shapeB, sf::Vector2f const & collisionDirection)
{
	(void)shapeA;
	(void)shapeB;
	(void)collisionDirection;
	m_nbCollision++;
}

void PhysicsMapScreen::onTileShapeCollision(TileShape * tileShape, AShape * shape, sf::Vector2f const & collisionDirection)
{
	(void)tileShape;
	(void)shape;
	(void)collisionDirection;
	m_nbCollision++;
}

bool PhysicsMapScreen::onPressed(sf::Event::KeyEvent const & event)
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

void	PhysicsMapScreen::draw(sf::RenderTarget& render)const
{
	sf::RenderStates states;
	render.clear(sf::Color::Black);
	render.draw(m_groundManager);
	m_engine.debugDraw(render);
}
