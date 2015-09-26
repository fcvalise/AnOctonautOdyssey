#include "SpaceShip.hpp"
#include "ResourceDefinitions.hpp"
#include "PhysicsEngine.hpp"
#include "CircleShape.hpp"
#include <Application.hpp>
#include <ResourceManager.hpp>

SpaceShip::SpaceShip(SpaceShipEvents event) :
	m_event(event),
	m_box(nullptr)
{
	octo::ResourceManager & resources = octo::Application::getResourceManager();

	if (event == Broken)
	{
		m_sprite.setSpriteSheet(resources.getSpriteSheet(BROKEN_SHIP_OSS));
		m_sprite.setScale(1.f, 1.f);

		typedef octo::SpriteAnimation::Frame	Frame;
		m_animation.setFrames({	Frame(sf::seconds(0.2f), 0u),
								Frame(sf::seconds(0.2f), 1u),
								Frame(sf::seconds(0.2f), 2u),
								Frame(sf::seconds(0.2f), 3u)});
		m_animation.setLoop(octo::LoopMode::Loop);
		m_smoke.setup(sf::Vector2f(10.f, 10.f));
		m_smoke.setVelocity(sf::Vector2f(0.f, -140.f));
		m_box = PhysicsEngine::getShapeBuilder().createCircle(false);
		m_box->setGameObject(this);
		m_box->setCollisionType(static_cast<std::size_t>(GameObjectType::SpaceShip));
		m_box->setCollisionMask(static_cast<std::size_t>(GameObjectType::PlayerEvent));
		m_box->setRadius(m_sprite.getGlobalSize().x / 2.f);
		m_box->setApplyGravity(false);
		m_box->setType(AShape::Type::e_trigger);
	}
	else if (event == Flying)
	{
		m_sprite.setSpriteSheet(resources.getSpriteSheet(SHIP_OSS));
		m_sprite.setScale(1.f, 1.f);

		typedef octo::SpriteAnimation::Frame	Frame;
		m_animation.setFrames({	Frame(sf::seconds(0.05f), 0u),
								Frame(sf::seconds(0.05f), 1u),
								Frame(sf::seconds(0.05f), 2u),
								Frame(sf::seconds(0.05f), 3u),
								Frame(sf::seconds(0.05f), 4u),
								Frame(sf::seconds(0.05f), 5u),
								Frame(sf::seconds(0.05f), 6u),
								Frame(sf::seconds(0.05f), 7u)});
		m_animation.setLoop(octo::LoopMode::Loop);
		m_smoke.setup(sf::Vector2f(10.f, 10.f));
		m_smoke.setVelocity(sf::Vector2f(-750.f, 0.f));
		m_smoke.setEmitTimeRange(0.05f, 0.1f);
	}
	m_sprite.setAnimation(m_animation);
	m_sprite.play();
}

void SpaceShip::setNextEvent(SpaceShipEvents event)
{
	m_event = event;
}

void SpaceShip::setPosition(sf::Vector2f const & position)
{
	m_sprite.setPosition(position.x, position.y - m_sprite.getLocalSize().y * 0.775f);
	if (m_event == Broken)
	{
		m_smoke.setPosition(position + sf::Vector2f(220.f, -200.f));
		if (m_box)
		{
			m_box->setPosition(m_sprite.getPosition() + m_sprite.getGlobalSize() / 2.f - sf::Vector2f(m_box->getRadius(), m_box->getRadius()));
			m_box->update();
		}
	}
}

sf::Vector2f const & SpaceShip::getPosition(void) const
{
	return m_sprite.getPosition();
}

sf::Vector2f SpaceShip::getSize(void) const
{
	return m_sprite.getGlobalSize();
}

void SpaceShip::move(sf::Vector2f const & translation)
{
	m_sprite.setPosition(m_sprite.getPosition() + translation);
	m_smoke.setPosition(m_sprite.getPosition() + sf::Vector2f(1000.f, 280.f));
}

void SpaceShip::update(sf::Time frameTime)
{
	m_sprite.update(frameTime);
	m_smoke.update(frameTime);
}

void SpaceShip::drawFront(sf::RenderTarget& render, sf::RenderStates) const
{
	render.draw(m_sprite);
	m_smoke.draw(render);
}
