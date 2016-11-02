#include "Rocket.hpp"
#include "PhysicsEngine.hpp"
#include "RectangleShape.hpp"
#include "CircleShape.hpp"
#include "CharacterOcto.hpp"
#include "PostEffectLayer.hpp"
#include "Progress.hpp"
#include <Application.hpp>
#include <AudioManager.hpp>
#include <ResourceManager.hpp>
#include <Interpolations.hpp>
#include <GraphicsManager.hpp>
#include <StateManager.hpp>

Rocket::Rocket(void) :
	ANpc(ROCKET_OSS),
	m_shader(PostEffectLayer::getInstance().getShader(ROCKET_TAKEOFF_FRAG)),
	m_enterRocketShape(PhysicsEngine::getShapeBuilder().createCircle(false)),
	m_state(Waiting),
	m_smokesCount(3),
	m_smokes(new SmokeSystem[m_smokesCount]),
	m_timerBeforeMax(sf::seconds(3.f)),
	m_timerFirstBlastMax(sf::seconds(5.f)),
	m_timerSecondBlastMax(sf::seconds(5.f)),
	m_timerOctoEnteringMax(sf::seconds(1.f)),
	m_sound(true),
	m_stopCameraMovement(false)
{
	setType(GameObjectType::Rocket);
	setSize(sf::Vector2f(267.f, 1426.f));
	setOrigin(sf::Vector2f(0.f, 0.f));
	setScale(1.f);
	setup();

	m_enterRocketShape->setGameObject(this);
	m_enterRocketShape->setRadius(40.f);
	m_enterRocketShape->setCollisionType(static_cast<std::size_t>(GameObjectType::RocketDoor));
	m_enterRocketShape->setCollisionMask(static_cast<std::size_t>(GameObjectType::Player));
	m_enterRocketShape->setApplyGravity(false);
	m_enterRocketShape->setType(AShape::Type::e_trigger);
	setupBox(this, static_cast<std::size_t>(GameObjectType::Npc), static_cast<std::size_t>(GameObjectType::PlayerEvent));
}

void Rocket::setup(void)
{
	typedef octo::CharacterAnimation::Frame			Frame;

	getIdleAnimation().setFrames({
			Frame(sf::seconds(0.4f), {0u, sf::FloatRect(), sf::Vector2f()}),
			});
	getIdleAnimation().setLoop(octo::LoopMode::Loop);

	setupMachine();

	for (std::size_t i = 0u; i < m_smokesCount; i++)
	{
		m_smokes[i].setVelocity(sf::Vector2f(0.f, 100.f));
		m_smokes[i].setup(sf::Vector2f(10.f, 10.f));
		m_smokes[i].setLifeTimeRange(0.2f, 0.4f);
	}
}

void Rocket::setupMachine(void)
{
	typedef octo::CharacterSprite::ACharacterState	State;
	typedef octo::FiniteStateMachine::StatePtr		StatePtr;

	octo::FiniteStateMachine	machine;
	StatePtr					idleState;

	idleState = std::make_shared<State>("0", getIdleAnimation(), getSprite());

	machine.setStart(idleState);
	machine.addTransition(Idle, idleState, idleState);

	setMachine(machine);
	setNextEvent(Idle);
}

void Rocket::setPosition(sf::Vector2f const & position)
{
	ANpc::setPosition(position);
	m_enterRocketShape->setPosition(position.x + 80.f, position.y - 830.f);
	m_enterRocketShape->update();
	m_smokes[0].setPosition(position + sf::Vector2f(40.f, 1350.f));
	m_smokes[1].setPosition(position + sf::Vector2f(152.f, 1350.f));
	m_smokes[2].setPosition(position + sf::Vector2f(264.f, 1350.f));
}

void Rocket::addMapOffset(float x, float y)
{
	ANpc::addMapOffset(x, y);
	m_enterRocketShape->setPosition(m_enterRocketShape->getPosition().x + x, m_enterRocketShape->getPosition().y + y);
	m_enterRocketShape->update();
	m_smokes[0].setPosition(m_smokes[0].getPosition() + sf::Vector2f(40.f, 1350.f));
	m_smokes[1].setPosition(m_smokes[1].getPosition() + sf::Vector2f(152.f, 1350.f));
	m_smokes[2].setPosition(m_smokes[2].getPosition() + sf::Vector2f(264.f, 1350.f));
}

void Rocket::playSound(void)
{
	if (m_sound)
	{
		octo::AudioManager& audio = octo::Application::getAudioManager();
		octo::ResourceManager& resources = octo::Application::getResourceManager();
		audio.playSound(resources.getSound(EVENT_ROCKET_AIR_BLAST_OGG), 1.f);
		m_sound = false;
	}
}

void Rocket::collideOctoEvent(CharacterOcto * octo)
{
	ANpc::collideOctoEvent(octo);
	switch (m_state)
	{
		case Waiting:
			octo->setOctoInRocketEnd();
			m_octoPosition = octo->getPhysicsPosition();
			m_state = OctoEntering;
			break;
		case OctoEntering:
			octo->setStartPosition(octo::linearInterpolation(m_octoPosition, m_enterRocketShape->getPosition(), m_timerOctoEntering / m_timerOctoEnteringMax));
			break;
		case StartSmoke:
			if (m_stopCameraMovement)
				octo->endInRocket();
			playSound();
			octo->enableCutscene(true);
			octo->setStartPosition(m_enterRocketShape->getPosition());
			break;
		default:
			break;
	}
}

void Rocket::update(sf::Time frametime)
{
	octo::CharacterSprite & sprite = getSprite();

	switch (m_state)
	{
		case Waiting:
			break;
		case OctoEntering:
			m_timerOctoEntering += frametime;
			if (m_timerOctoEntering >= m_timerOctoEnteringMax)
			{
				m_timerOctoEntering = sf::Time::Zero;
				m_timerOctoEnteringMax = sf::seconds(15.f);
				PostEffectLayer::getInstance().enableShader(ROCKET_TAKEOFF_FRAG, true);
				m_state = StartSmoke;
			}
			m_lastPosition = getPosition();
			m_lastPositionDoor = m_enterRocketShape->getPosition();
			break;
		case StartSmoke:
			m_timerOctoEntering += frametime;
			m_shader.setParameter("time", m_timerOctoEntering.asSeconds());
			m_shader.setParameter("intensity", octo::linearInterpolation(0.f, 0.3f, std::min(1.f, m_timerOctoEntering / m_timerOctoEnteringMax)));
			if (m_timerBefore < m_timerBeforeMax)
				m_timerBefore += frametime;
			else
			{
				m_timerBefore = m_timerBeforeMax;
				if (m_timerFirstBlast < m_timerFirstBlastMax)
					m_timerFirstBlast += frametime;
				else
				{
					m_timerFirstBlast = m_timerFirstBlastMax;
					if (m_timerSecondBlast < m_timerSecondBlastMax)
						m_timerSecondBlast += frametime;
					else
					{
						m_timerSecondBlast = m_timerSecondBlastMax;
						octo::Application::getStateManager().change("rocket_end");
					}
				}
			}

			for (std::size_t i = 0u; i < m_smokesCount; i++)
			{
				if (m_timerBefore < m_timerBeforeMax)
				{
					float coef = m_timerBefore / m_timerBeforeMax;

					m_smokes[i].setVelocity(sf::Vector2f(0.f, 100.f * coef));
					m_smokes[i].setLifeTimeRange(coef * 1.f, coef * 3.f);
					m_smokes[i].setScaleRange(coef * 1.f, coef * 3.f);
				}
				else
				{
					m_smokes[i].setVelocity(sf::Vector2f(0.f, 30.f * (1.f + m_timerFirstBlast.asSeconds() + m_timerSecondBlast.asSeconds())));
					if (m_timerFirstBlast == m_timerFirstBlastMax)
						m_smokes[i].setEmitTimeRange(0.01f, 0.05f);
					else if (m_timerSecondBlast > m_timerSecondBlastMax)
						m_smokes[i].setCanEmit(false);
				}
			}
			if (m_timerBefore >= m_timerBeforeMax)
			{
				RectangleShape * box = getBox();
				if (m_timerFirstBlast < m_timerFirstBlastMax)
				{
					box->setPosition(octo::cosinusInterpolation(m_lastPosition, m_lastPosition + sf::Vector2f(0.f, -400.f), m_timerFirstBlast / m_timerFirstBlastMax));
					m_enterRocketShape->setPosition(octo::cosinusInterpolation(m_lastPositionDoor, m_lastPositionDoor + sf::Vector2f(0.f, -400.f), m_timerFirstBlast / m_timerFirstBlastMax));
				}
				else if (m_timerSecondBlast < m_timerSecondBlastMax)
				{
					if (m_timerSecondBlast > m_timerSecondBlastMax / 2.f)
						m_stopCameraMovement = true;
					box->setPosition(octo::cosinusInterpolation(m_lastPosition + sf::Vector2f(0.f, -400.f), m_lastPosition + sf::Vector2f(0.f, -3500.f), m_timerSecondBlast / m_timerSecondBlastMax));
					m_enterRocketShape->setPosition(octo::cosinusInterpolation(m_lastPositionDoor + sf::Vector2f(0.f, -400.f), m_lastPositionDoor + sf::Vector2f(0.f, -3500.f), m_timerSecondBlast / m_timerSecondBlastMax));
				}
				else
				{
					box->setPosition(m_lastPosition + sf::Vector2f(0.f, -3500.f));
					m_enterRocketShape->setPosition(m_lastPositionDoor + sf::Vector2f(0.f, -3500.f));
				}
				box->update();
				m_enterRocketShape->update();
			}
			break;
		default:
			break;
	}

	sprite.update(frametime);
	sf::Vector2f const & center = getBox()->getRenderPosition();
	sprite.setPosition(center);

	if (m_timerFirstBlast < m_timerFirstBlastMax * 0.8f)
	{
		m_smokes[0].setPosition(ANpc::getPosition() + sf::Vector2f(40.f, 1350.f));
		m_smokes[1].setPosition(ANpc::getPosition() + sf::Vector2f(152.f, 1350.f));
		m_smokes[2].setPosition(ANpc::getPosition() + sf::Vector2f(264.f, 1350.f));
	}
	else
	{
		m_smokes[0].setPosition(ANpc::getPosition() + sf::Vector2f(80.f, 1450.f));
		m_smokes[1].setPosition(ANpc::getPosition() + sf::Vector2f(152.f, 1450.f));
		m_smokes[2].setPosition(ANpc::getPosition() + sf::Vector2f(224.f, 1450.f));
	}
	for (std::size_t i = 0u; i < m_smokesCount; i++)
		m_smokes[i].update(frametime);

	resetVariables();
}

void Rocket::drawFront(sf::RenderTarget & render, sf::RenderStates states) const
{
	ANpc::draw(render, states);
	for (std::size_t i = 0u; i < m_smokesCount; i++)
		m_smokes[i].draw(render);
}
