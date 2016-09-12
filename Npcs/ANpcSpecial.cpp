#include "ANpcSpecial.hpp"

ANpcSpecial::ANpcSpecial(ResourceKey const & npcId, bool isMeetable) :
	ANpc(npcId, isMeetable),
	m_canDoSpecial(true)
{
	setSize(sf::Vector2f(75.f, 325.f));
	setOrigin(sf::Vector2f(60.f, -150.f));
	setScale(0.8f);
	setTextOffset(sf::Vector2f(10.f, 150.f));
	setTimerMax(sf::seconds(8.0f));
	setupBox(this, static_cast<std::size_t>(GameObjectType::SpecialNpc), static_cast<std::size_t>(GameObjectType::Player));
}

void ANpcSpecial::setupMachine(void)
{
	typedef octo::CharacterSprite::ACharacterState	State;
	typedef octo::FiniteStateMachine::StatePtr		StatePtr;

	octo::FiniteStateMachine	machine;
	StatePtr					idleState;
	StatePtr					specialState;

	idleState = std::make_shared<State>("0", getIdleAnimation(), getSprite());
	specialState = std::make_shared<State>("1", getSpecial1Animation(), getSprite());

	machine.setStart(specialState);
	machine.addTransition(Idle, idleState, idleState);
	machine.addTransition(Idle, specialState, idleState);

	machine.addTransition(Special1, specialState, specialState);
	machine.addTransition(Special1, idleState, specialState);

	setMachine(machine);
	setNextEvent(Idle);
}

void ANpcSpecial::update(sf::Time frameTime)
{
	if (!getCollideEventOcto())
		m_canDoSpecial = true;
	ANpc::update(frameTime);
}

void ANpcSpecial::updateState(void)
{
	octo::CharacterSprite & sprite = getSprite();

	if (sprite.getCurrentEvent() == Idle && getCollideEventOcto() && m_canDoSpecial)
	{
		m_canDoSpecial = false;
		sprite.setNextEvent(Special1);
	}
	else if (sprite.getCurrentEvent() == Special1 && sprite.isTerminated())
	{
		sprite.setNextEvent(Idle);
	}
}
