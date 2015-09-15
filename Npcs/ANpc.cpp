#include "ANpc.hpp"
#include "RectangleShape.hpp"
#include "CircleShape.hpp"
#include "PhysicsEngine.hpp"
#include <Application.hpp>
#include <ResourceManager.hpp>
#include <sstream>

ANpc::ANpc(ResourceKey const & npcId) :
	m_box(PhysicsEngine::getShapeBuilder().createRectangle()),
	m_eventBox(PhysicsEngine::getShapeBuilder().createCircle()),
	m_currentText(-1),
	m_velocity(200.f),
	m_scale(1.f)
{
	octo::ResourceManager & resources = octo::Application::getResourceManager();

	m_sprite.setSpriteSheet(resources.getSpriteSheet(npcId));

	m_box->setGameObject(this);
	m_eventBox->setGameObject(this);
	m_eventBox->setRadius(200.f);
	m_eventBox->setApplyGravity(false);
	m_eventBox->setType(AShape::Type::e_trigger);
	setBoxCollision(static_cast<std::size_t>(GameObjectType::Npc), 0u);

	std::map<std::string, std::vector<std::string>>	npcTexts;
	std::istringstream f(resources.getText(DIALOGS_TXT).toAnsiString());
	std::string key;
	std::string line;
	while (std::getline(f, key, '='))
	{
		std::getline(f, line);
		npcTexts[key].push_back(line);
	}
	setTexts(npcTexts[npcId]);
}

ANpc::~ANpc(void)
{
}

void ANpc::setupIdleAnimation(std::initializer_list<FramePair> list, octo::LoopMode loopMode)
{
	setupAnimation(m_idleAnimation, list, loopMode);
}

void ANpc::setupWalkAnimation(std::initializer_list<FramePair> list, octo::LoopMode loopMode)
{
	setupAnimation(m_walkAnimation, list, loopMode);
}

void ANpc::setupSpecial1Animation(std::initializer_list<FramePair> list, octo::LoopMode loopMode)
{
	setupAnimation(m_special1Animation, list, loopMode);
}

void ANpc::setupSpecial2Animation(std::initializer_list<FramePair> list, octo::LoopMode loopMode)
{
	setupAnimation(m_special2Animation, list, loopMode);
}

void ANpc::setupAnimation(octo::CharacterAnimation & animation, std::initializer_list<FramePair> list, octo::LoopMode loopMode)
{
	typedef octo::CharacterAnimation::Frame		Frame;
	std::vector<Frame>							frames;

	for (auto & frame : list)
		frames.push_back(Frame(sf::seconds(frame.first), { frame.second, sf::FloatRect(), sf::Vector2f() }));
	animation.setFrames(frames);
	animation.setLoop(loopMode);
}

void ANpc::setupMachine(void)
{
	typedef octo::CharacterSprite::ACharacterState	State;
	typedef octo::FiniteStateMachine::StatePtr		StatePtr;

	octo::FiniteStateMachine	machine;
	StatePtr					idleState;
	StatePtr					walkLeftState;
	StatePtr					walkRightState;

	idleState = std::make_shared<State>("0", m_idleAnimation, m_sprite);
	walkLeftState = std::make_shared<State>("1", m_walkAnimation, m_sprite);
	walkRightState = std::make_shared<State>("2", m_walkAnimation, m_sprite);

	machine.setStart(idleState);
	machine.addTransition(Idle, idleState, idleState);
	machine.addTransition(Idle, walkLeftState, idleState);
	machine.addTransition(Idle, walkRightState, idleState);

	machine.addTransition(Left, idleState, walkLeftState);

	machine.addTransition(Right, idleState, walkRightState);

	setMachine(machine);
	m_sprite.setNextEvent(Idle);
}

bool ANpc::canWalk(void) const
{
	return true;
}

bool ANpc::canJump(void) const
{
	return false;
}

bool ANpc::canDoubleJump(void) const
{
	return false;
}

bool ANpc::canDance(void) const
{
	return false;
}

bool ANpc::canSpecial1(void) const
{
	return false;
}

bool ANpc::canSpecial2(void) const
{
	return false;
}

void ANpc::setArea(sf::FloatRect const & area)
{
	m_area = area;
}

void ANpc::setOrigin(sf::Vector2f const & origin)
{
	m_origin = origin;
	m_sprite.setOrigin(origin);
}

void ANpc::setScale(float scale)
{
	m_scale = scale;
	m_sprite.setScale(scale, scale);
}

void ANpc::setPosition(sf::Vector2f const & position)
{

	if (m_box->getSleep())
	{
		m_box->setPosition(position.x, position.y - m_box->getSize().y - getHeight());
		m_eventBox->setPosition(position.x - m_eventBox->getRadius(), position.y - m_eventBox->getRadius() - getHeight());
		m_box->update();
		m_eventBox->update();
	}
	else
	{
		m_eventBox->setPosition(position.x - m_eventBox->getRadius(), position.y - m_eventBox->getRadius());
		m_box->setPosition(position.x, position.y - m_box->getSize().y);
	}
}

void ANpc::setNextEvent(Events event)
{
	m_sprite.setNextEvent(event);
}

void ANpc::setMachine(octo::FiniteStateMachine const & machine)
{
	m_sprite.setMachine(machine);
	m_sprite.restart();
}

void ANpc::setVelocity(float velocity)
{
	m_velocity = velocity;
}

void ANpc::setBoxCollision(std::size_t type, std::size_t mask)
{
	m_box->setCollisionType(type);
	m_box->setCollisionMask(mask);
	m_eventBox->setCollisionType(type);
	m_eventBox->setCollisionMask(mask);
}

void ANpc::setCurrentText(std::size_t index)
{
	m_currentText = index;
}

void ANpc::setTextOffset(sf::Vector2f const & offset)
{
	m_textOffset = offset;
}

void ANpc::setSize(sf::Vector2f const & size)
{
	m_box->setSize(size);
}

void ANpc::setTexts(std::vector<std::string> const & texts)
{
	for (std::size_t i = 0u; i < texts.size(); i++)
	{
		std::unique_ptr<BubbleText> bubble;
		bubble.reset(new BubbleText());
		bubble->setup(texts[i], sf::Color::White);
		bubble->setType(ABubble::Type::Speak);
		m_texts.push_back(std::move(bubble));
		setCurrentText(i);
	}
}

void ANpc::setCurrentText(int index)
{
	assert(index < static_cast<int>(m_texts.size()));
	m_currentText = index;
}

float ANpc::getScale(void) const
{
	return m_scale;
}

float ANpc::getVelocity(void) const
{
	return m_velocity;
}

sf::Vector2f const & ANpc::getOrigin(void) const
{
	return m_origin;
}

sf::FloatRect const & ANpc::getArea(void) const
{
	return m_area;
}

RectangleShape * ANpc::getBox(void)
{
	return m_box;
}

CircleShape * ANpc::getEventBox(void)
{
	return m_eventBox;
}

octo::CharacterSprite & ANpc::getSprite(void)
{
	return m_sprite;
}

octo::CharacterAnimation & ANpc::getIdleAnimation(void)
{
	return m_idleAnimation;
}

octo::CharacterAnimation & ANpc::getWalkAnimation(void)
{
	return m_walkAnimation;
}

octo::CharacterAnimation & ANpc::getSpecial1Animation(void)
{
	return m_special1Animation;
}

octo::CharacterAnimation & ANpc::getSpecial2Animation(void)
{
	return m_special2Animation;
}

void ANpc::addMapOffset(float x, float y)
{
	m_box->setPosition(m_box->getPosition().x + x, m_box->getPosition().y + y);
	m_box->update(); // We must update ourselves because the box is out of the screen, and the engine didn't update shape out of the screen
	m_eventBox->setPosition(m_eventBox->getPosition().x + x, m_eventBox->getPosition().y + y);
	m_eventBox->update(); // We must update ourselves because the box is out of the screen, and the engine didn't update shape out of the screen
	m_area.left += x;
	m_area.top += y;
}

void ANpc::activatePhysics(bool activate)
{
	m_box->setSleep(!activate);
}

float ANpc::getHeight(void) const
{
	return 100.f;
}

sf::Vector2f const & ANpc::getPosition(void) const
{
	return m_box->getPosition();
}

void ANpc::update(sf::Time frametime)
{
	updateState();
	updatePhysics();

	m_sprite.update(frametime);
	sf::FloatRect const & bounds = m_box->getGlobalBounds();
	m_sprite.setPosition(bounds.left, bounds.top);

	updateText(frametime);
}

void ANpc::updateState(void)
{
	sf::FloatRect const & bounds = m_box->getGlobalBounds();
	if ((bounds.left + bounds.width) <= (m_area.left + m_area.width) && m_sprite.getCurrentEvent() == Idle && canWalk())
	{
		m_sprite.setNextEvent(Right);
		m_sprite.setOrigin(m_origin.x, m_origin.y);
		m_sprite.setScale(m_scale, m_scale);
	}
	else if (bounds.left >= m_area.left && m_sprite.getCurrentEvent() == Idle && canWalk())
	{
		m_sprite.setNextEvent(Left);
		sf::Vector2f const & size = m_sprite.getLocalSize();
		m_sprite.setOrigin(size.x - m_origin.x, m_origin.y);
		m_sprite.setScale(-m_scale, m_scale);
	}
	else if ((bounds.left <= m_area.left || (bounds.left + bounds.width) >= (m_area.left + m_area.width))
			&& m_sprite.getCurrentEvent() != Idle)
	{
		m_sprite.setNextEvent(Idle);
	}
}

void ANpc::updatePhysics(void)
{
	sf::Vector2f velocity;
	if (m_sprite.getCurrentEvent() == Left)
	{
		velocity.x = (-1.f * m_velocity);
	}
	else if (m_sprite.getCurrentEvent() == Right)
	{
		velocity.x = m_velocity;
	}
	m_box->setVelocity(velocity);
	m_eventBox->setPosition(m_sprite.getPosition().x - m_eventBox->getRadius(), m_sprite.getPosition().y - m_eventBox->getRadius());
}

void ANpc::updateText(sf::Time frametime)
{
	if (m_currentText >= 0)
	{
		m_texts[m_currentText]->setPosition(m_sprite.getPosition() + m_textOffset);
		m_texts[m_currentText]->update(frametime);
		m_texts[m_currentText]->setActive(true);
	}
}

void ANpc::draw(sf::RenderTarget & render, sf::RenderStates states) const
{
	m_sprite.draw(render, states);
}

void ANpc::drawText(sf::RenderTarget & render, sf::RenderStates) const
{
	if (m_currentText >= 0)
		m_texts[m_currentText]->draw(render);
}
