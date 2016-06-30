#include "Rocket.hpp"
#include "RectangleShape.hpp"
#include <Interpolations.hpp>

Rocket::Rocket(void) :
	ANpc(ROCKET_OSS),
	m_smokesCount(3),
	m_smokes(new SmokeSystem[m_smokesCount]),
	m_collide(false),
	m_timerBeforeMax(sf::seconds(3.f)),
	m_timerFirstBlastMax(sf::seconds(5.f)),
	m_timerSecondBlastMax(sf::seconds(5.f))
{
	setSize(sf::Vector2f(267.f, 1426.f));
	setOrigin(sf::Vector2f(0.f, 0.f));
	setScale(1.f);
	setup();

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

void Rocket::update(sf::Time frametime)
{
	octo::CharacterSprite & sprite = getSprite();

	updateState();
	updatePhysics();

	sprite.update(frametime);
	sf::Vector2f const & center = getBox()->getRenderPosition();
	sprite.setPosition(center);

	if (m_timerFirstBlast < m_timerFirstBlastMax * 0.8f)
	{
		m_smokes[0].setPosition(ANpc::getPosition() + sf::Vector2f(20.f, 1350.f));
		m_smokes[1].setPosition(ANpc::getPosition() + sf::Vector2f(132.f, 1350.f));
		m_smokes[2].setPosition(ANpc::getPosition() + sf::Vector2f(244.f, 1350.f));
	}
	else
	{
		m_smokes[0].setPosition(ANpc::getPosition() + sf::Vector2f(60.f, 1450.f));
		m_smokes[1].setPosition(ANpc::getPosition() + sf::Vector2f(132.f, 1450.f));
		m_smokes[2].setPosition(ANpc::getPosition() + sf::Vector2f(204.f, 1450.f));
	}
	for (std::size_t i = 0u; i < m_smokesCount; i++)
		m_smokes[i].update(frametime);

	updateTimer(frametime);
	resetVariables();
}

void Rocket::updateTimer(sf::Time frametime)
{
	if (m_collide)
	{
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
					m_timerSecondBlast = m_timerSecondBlastMax;
			}
		}
	}
}

void Rocket::updateState(void)
{
	if (getCollideEventOcto())
		m_collide = true;

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

	m_position = getPosition();
}

void Rocket::updatePhysics(void)
{
	if (m_collide && m_timerBefore >= m_timerBeforeMax)
	{
		RectangleShape * box = getBox();
		if (m_timerFirstBlast < m_timerFirstBlastMax)
			box->setPosition(octo::cosinusInterpolation(m_position, m_position + sf::Vector2f(0.f, -400.f), m_timerFirstBlast / m_timerFirstBlastMax));
		else if (m_timerSecondBlast < m_timerSecondBlastMax)
			box->setPosition(octo::cosinusInterpolation(m_position + sf::Vector2f(0.f, -400.f), m_position + sf::Vector2f(0.f, -3500.f), m_timerSecondBlast / m_timerSecondBlastMax));
		else
			box->setPosition(m_position + sf::Vector2f(0.f, -3500.f));
		box->update();
	}
}

void Rocket::draw(sf::RenderTarget & render, sf::RenderStates states) const
{
	ANpc::draw(render, states);
	for (std::size_t i = 0u; i < m_smokesCount; i++)
		m_smokes[i].draw(render);
}