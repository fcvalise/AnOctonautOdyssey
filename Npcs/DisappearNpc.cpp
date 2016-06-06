#include "DisappearNpc.hpp"
#include "RectangleShape.hpp"

RandomGenerator DisappearNpc::m_generator("random");

DisappearNpc::DisappearNpc(ResourceKey const & key) :
	ANpc(key, false),
	m_isVisible(true),
	m_transparency(0.f),
	m_randomAppearTimer(sf::seconds(m_generator.randomFloat(1.f, 5.f)))
{
	setSize(sf::Vector2f(25.f, 60.f));
	setOrigin(sf::Vector2f(90.f, m_generator.randomFloat(50.f, 400.f)));
	setScale(0.8f);
	setTextOffset(sf::Vector2f(-20.f, -10.f));
	setTimerMax(sf::seconds(8.0f));
	setup();
}

void DisappearNpc::setup(void)
{
	typedef octo::CharacterAnimation::Frame			Frame;

	getIdleAnimation().setFrames({
			Frame(sf::seconds(0.4f), {0u, sf::FloatRect(), sf::Vector2f()}),
			Frame(sf::seconds(0.4f), {1u, sf::FloatRect(), sf::Vector2f()}),
			Frame(sf::seconds(0.4f), {2u, sf::FloatRect(), sf::Vector2f()}),
			Frame(sf::seconds(0.4f), {3u, sf::FloatRect(), sf::Vector2f()}),
			Frame(sf::seconds(0.4f), {4u, sf::FloatRect(), sf::Vector2f()}),
			Frame(sf::seconds(0.4f), {5u, sf::FloatRect(), sf::Vector2f()}),
			Frame(sf::seconds(0.4f), {6u, sf::FloatRect(), sf::Vector2f()}),
			Frame(sf::seconds(0.4f), {7u, sf::FloatRect(), sf::Vector2f()}),
			Frame(sf::seconds(0.4f), {8u, sf::FloatRect(), sf::Vector2f()}),
			Frame(sf::seconds(0.4f), {9u, sf::FloatRect(), sf::Vector2f()}),
			Frame(sf::seconds(0.4f), {10u, sf::FloatRect(), sf::Vector2f()}),
			Frame(sf::seconds(0.4f), {11u, sf::FloatRect(), sf::Vector2f()}),
			});
	getIdleAnimation().setLoop(octo::LoopMode::Loop);

	getSpecial1Animation().setFrames({
			Frame(sf::seconds(0.4f), {0u, sf::FloatRect(), sf::Vector2f()}),
			Frame(sf::seconds(0.4f), {1u, sf::FloatRect(), sf::Vector2f()}),
			Frame(sf::seconds(0.4f), {2u, sf::FloatRect(), sf::Vector2f()}),
			Frame(sf::seconds(0.4f), {3u, sf::FloatRect(), sf::Vector2f()}),
			Frame(sf::seconds(0.4f), {4u, sf::FloatRect(), sf::Vector2f()}),
			Frame(sf::seconds(0.4f), {5u, sf::FloatRect(), sf::Vector2f()}),
			Frame(sf::seconds(0.4f), {6u, sf::FloatRect(), sf::Vector2f()}),
			Frame(sf::seconds(0.4f), {7u, sf::FloatRect(), sf::Vector2f()}),
			Frame(sf::seconds(0.4f), {8u, sf::FloatRect(), sf::Vector2f()}),
			Frame(sf::seconds(0.4f), {9u, sf::FloatRect(), sf::Vector2f()}),
			Frame(sf::seconds(0.4f), {10u, sf::FloatRect(), sf::Vector2f()}),
			Frame(sf::seconds(0.4f), {11u, sf::FloatRect(), sf::Vector2f()}),
			});
	getSpecial1Animation().setLoop(octo::LoopMode::NoLoop);

	setupMachine();
}

void DisappearNpc::setupMachine(void)
{
	typedef octo::CharacterSprite::ACharacterState	State;
	typedef octo::FiniteStateMachine::StatePtr		StatePtr;

	octo::FiniteStateMachine	machine;
	StatePtr					idleState;
	StatePtr					special1State;

	idleState = std::make_shared<State>("0", getIdleAnimation(), getSprite());
	special1State = std::make_shared<State>("1", getSpecial1Animation(), getSprite());

	machine.setStart(idleState);
	machine.addTransition(Idle, idleState, idleState);
	machine.addTransition(Idle, special1State, idleState);

	machine.addTransition(Special1, idleState, special1State);

	setMachine(machine);
	setNextEvent(Idle);
}

void DisappearNpc::makeDisappear(sf::Time frametime)
{
	m_randomAppearTimer -= frametime;
	m_randomDisappearTimer -= frametime;

	if (getCollideEventOcto())
		m_isVisible = false;
	if (m_randomDisappearTimer <= sf::Time::Zero)
	{
		m_isVisible = false;
		m_randomDisappearTimer = sf::seconds(m_generator.randomFloat(10.f, 30.f));
	}

	if (!m_isVisible)
	{
		if (m_transparency - frametime.asSeconds() * 255.f > 0.f)
			m_transparency -= frametime.asSeconds() * 255.f;
		else
		{
			m_randomAppearTimer = sf::seconds(m_generator.randomFloat(1.f, 3.f));
			m_isVisible = true;
			setOrigin(sf::Vector2f(m_generator.randomFloat(20.f, 120.f), m_generator.randomFloat(50.f, 400.f)));
		}
	}
	else if (m_randomAppearTimer <= sf::Time::Zero && m_transparency + frametime.asSeconds() * 255.f < 255.f)
		m_transparency += frametime.asSeconds() * 255.f;
}

void DisappearNpc::update(sf::Time frametime)
{
	octo::CharacterSprite & sprite = getSprite();

	updateState();
	updatePhysics();

	makeDisappear(frametime);

	sprite.update(frametime);
	sprite.setPosition(getBox()->getRenderPosition());
	sprite.setColor(sf::Color(255, 255, 255, m_transparency));

	updateText(frametime);
	resetVariables();
}

void DisappearNpc::updateState(void)
{
	octo::CharacterSprite & sprite = getSprite();

	if (sprite.getCurrentEvent() == Special1)
	{
		if (sprite.isTerminated())
		{
			sprite.setNextEvent(Idle);
			addTimer(-getTimer());
		}
	}
	else if (sprite.getCurrentEvent() == Idle)
	{
		if (getTimer() >= getTimerMax())
		{
			addTimer(-getTimerMax());
			sprite.setNextEvent(Special1);
		}
	}
}