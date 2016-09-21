#include "ASwimNpc.hpp"
#include "RectangleShape.hpp"
#include "SkyCycle.hpp"
#include "CircleShape.hpp"
#include "CharacterOcto.hpp"
#include <Interpolations.hpp>

RandomGenerator ASwimNpc::m_generator("random");

ASwimNpc::ASwimNpc(ResourceKey const & npcId, bool isMeetable, bool isShift) :
	ANpc(npcId, isMeetable),
	m_waterLevel(0.f),
	m_isMet(false),
	m_isShift(isShift),
	m_speed(m_generator.randomFloat(7.f, 13.f)),
	m_shift(m_generator.randomFloat(-50.f, 50.f), m_generator.randomFloat(-50.f, 50.f))
{
	setupBox(this, static_cast<std::size_t>(GameObjectType::SwimNpc), static_cast<std::size_t>(GameObjectType::PlayerEvent));
	getBox()->setApplyGravity(false);
}

void ASwimNpc::setupMachine(void)
{
	typedef octo::CharacterSprite::ACharacterState	State;
	typedef octo::FiniteStateMachine::StatePtr		StatePtr;

	octo::FiniteStateMachine	machine;
	StatePtr					idle;
	StatePtr					special1;

	idle = std::make_shared<State>("0", getIdleAnimation(), getSprite());
	special1 = std::make_shared<State>("3", getSpecial1Animation(), getSprite());

	machine.setStart(idle);
	machine.addTransition(Idle, idle, idle);
	machine.addTransition(Idle, special1, idle);

	machine.addTransition(Special1, special1, special1);
	machine.addTransition(Special1, idle, special1);

	setMachine(machine);
	setNextEvent(Idle);
}

void ASwimNpc::setPosition(sf::Vector2f const & position)
{
	if (!m_isMet)
		ANpc::setPosition(position);
}

void ASwimNpc::update(sf::Time frametime)
{
	octo::CharacterSprite & sprite = getSprite();

	computeBehavior(frametime);
	updateState();
	updatePhysics();

	sprite.update(frametime);
	sprite.setPosition(getBox()->getRenderPosition());

	updateText(frametime);
	resetVariables();
}

void ASwimNpc::computeBehavior(sf::Time frametime)
{
	octo::CharacterSprite & sprite = getSprite();
	RectangleShape * box = getBox();
	sf::Vector2f position = octo::linearInterpolation(m_octoPosition, box->getPosition(), 1.f - frametime.asSeconds());

	if (sprite.getCurrentEvent() == Special1)
	{
		float dist = std::sqrt(std::pow(position.x - m_octoPosition.x, 2u) + std::pow(position.y - m_octoPosition.y, 2u));
		if (position.y > m_waterLevel + 200.f && dist >= 100.f)
		{
			box->setVelocity((position - box->getPosition()) * (dist / m_speed));
			if (m_isShift)
			{
				float angle = octo::rad2Deg(std::atan2(box->getPosition().y - m_octoPosition.y, box->getPosition().x - m_octoPosition.x)) - 90.f;
				if (angle < 0.f)
					angle += 360.f;
				sprite.setRotation(angle);
			}
		}
	}
	else if (m_isMet)
	{
		if (position.y > m_waterLevel + 200.f)
			box->setVelocity((position - box->getPosition()) * 20.f);
		if (sprite.getRotation() > 180.f)
			sprite.setRotation(octo::linearInterpolation(360.f, sprite.getRotation(), 1.f - frametime.asSeconds()));
		else
			sprite.setRotation(octo::linearInterpolation(0.f, sprite.getRotation(), 1.f - frametime.asSeconds()));
	}
}

void ASwimNpc::updateState(void)
{
	octo::CharacterSprite & sprite = getSprite();

	if (sprite.getCurrentEvent() == Idle && getCollideEventOcto() && m_octoPosition.y > m_waterLevel)
		sprite.setNextEvent(Special1);
	else if (sprite.getCurrentEvent() == Special1 && !getCollideEventOcto())
		sprite.setNextEvent(Idle);
}

void ASwimNpc::updatePhysics(void)
{
	getBox()->update();
}

void ASwimNpc::collideOctoEvent(CharacterOcto * octo)
{
	ANpc::collideOctoEvent(octo);
	m_octoPosition = octo->getPosition() + m_shift;
	if (!m_isMet)
		m_isMet = true;
	if (m_waterLevel == 0.f)
		m_waterLevel = octo->getWaterLevel();
}

void ASwimNpc::draw(sf::RenderTarget & render, sf::RenderStates states) const
{
	ANpc::draw(render, states);
}
