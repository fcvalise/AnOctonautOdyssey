#include "NanoRobot.hpp"
#include "ResourceDefinitions.hpp"
#include "Tile.hpp"
#include "PhysicsEngine.hpp"
#include "CircleShape.hpp"
#include "Progress.hpp"
#include <Application.hpp>
#include <AudioManager.hpp>
#include <ResourceManager.hpp>
#include <sstream>

sf::Vector2f NanoRobot::m_laserConvergence;

NanoRobot::NanoRobot(sf::Vector2f const & position, std::string const & id, std::size_t nbFrames, int seed, sf::Vector2f const & offsetLaser, float multiplier) :
	m_swarm(1),
	m_uniformPopulation(1234u, &octo::Application::getResourceManager().getPalette(FROM_SEA1_OPA),
						1.2f, 2.f, 6.f, 10.f, 32.f, 50.f,
						sf::Time::Zero, sf::Time::Zero),
	m_spawnMode(FireflySwarm::SpawnMode::Normal),
	m_positionBehavior(new FireflySwarm::CirclePositionBehavior(seed, 50.f)),
	m_ray(new sf::Vertex[16]),
	m_texture(nullptr),
	m_offsetLaser(offsetLaser),
	m_timerRepairShip(sf::Time::Zero),
	m_timerRepair(sf::Time::Zero),
	m_timerRepairMax(sf::seconds(4.f)),
	m_repairIndex(0u),
	m_startLastAnimation(false),
	m_usePathLaser(false),
	m_multiplier(multiplier),
	m_box(PhysicsEngine::getShapeBuilder().createCircle(false)),
	m_textIndex(0u),
	m_infoSetup(false),
	m_state(Idle),
	m_timer(sf::Time::Zero),
	m_timerMax(sf::seconds(15.f)),
	m_isTravelling(false),
	m_engine(std::time(0)),
	m_soundDistri(0u, 2u),
	m_popUp(false),
	m_popUpTimerMax(sf::seconds(3.f))
{
	octo::ResourceManager & resources = octo::Application::getResourceManager();
	Progress & progress = Progress::getInstance();
	InputListener::addInputListener();

	m_texture = &resources.getTexture(STARGRADIENT_PNG);

	m_box->setRadius(150.f);
	m_box->setType(AShape::Type::e_trigger);
	m_box->setCollisionMask(static_cast<std::size_t>(GameObjectType::Player));
	m_box->setApplyGravity(false);

	m_swarm.setPositionBehavior(m_positionBehavior);
	m_swarm.setTexture(resources.getTexture(FIREFLY01_PNG));
	m_swarm.create(m_spawnMode, position, sf::Color::Magenta, 8.f, 32.f, 2.f);

	m_sprite.setSpriteSheet(resources.getSpriteSheet(id));
	m_sprite.setScale(0.6f, 0.6f);
	m_glowingEffect.setNanoScale(sf::Vector2f(0.6f, 0.6f));

	octo::SpriteAnimation::FrameList	frames;
	for (std::size_t i = 0u; i < nbFrames; i++)
		frames.emplace_back(sf::seconds(0.2f), i);

	m_animation.setFrames(frames);
	m_animation.setLoop(octo::LoopMode::Loop);
	m_sprite.setAnimation(m_animation);
	m_sprite.play();

	std::map<std::string, std::vector<std::wstring>>	npcTexts;
	std::wstringstream f(resources.getText(progress.getTextFile()).toWideString());
	std::wstring wkey;
	std::wstring line;
	wchar_t delim = '=';
	while (std::getline(f, wkey, delim))
	{
		std::getline(f, line);
		std::string key(wkey.begin(), wkey.end());
		npcTexts[key].push_back(line);
	}
	for (std::size_t i = 0u; i < npcTexts[id].size(); i++)
	{
		std::unique_ptr<BubbleText> bubble;
		bubble.reset(new BubbleText());
		bubble->setup(npcTexts[id][i], sf::Color::White);
		bubble->setType(ABubble::Type::Speak);
		bubble->setActive(true);
		m_texts.push_back(std::move(bubble));
	}

	m_infoBubble.setType(ABubble::Type::None);
	m_infoBubble.setActive(false);

	sf::Color color = sf::Color::Red;

	for (std::size_t i = 0u; i < 16u; i++)
		m_ray[i].color = color;

	m_ray[1].color.a = 0;
	m_ray[2].color.a = 0;
	m_ray[4].color.a = 0;
	m_ray[7].color.a = 0;

	m_ray[12].color = sf::Color(255, 165, 0);
	m_ray[13].color = sf::Color(255, 165, 0);
	m_ray[14].color = sf::Color(255, 165, 0);
	m_ray[15].color = sf::Color(255, 165, 0);

	sf::Vector2f texSize(m_texture->getSize());
	m_ray[8].texCoords = sf::Vector2f(0.f, 0.f);
	m_ray[9].texCoords = sf::Vector2f(texSize.x, 0.f);
	m_ray[10].texCoords = texSize;
	m_ray[11].texCoords = sf::Vector2f(0.f, texSize.y);

	m_ray[12].texCoords = sf::Vector2f(0.f, 0.f);
	m_ray[13].texCoords = sf::Vector2f(texSize.x, 0.f);
	m_ray[14].texCoords = texSize;
	m_ray[15].texCoords = sf::Vector2f(0.f, texSize.y);

	m_particles.setColor(color);
}

NanoRobot::~NanoRobot(void)
{
	InputListener::removeInputListener();
	if (m_sound != nullptr)
		m_sound->stop();
}

void NanoRobot::setup(AGameObjectBase * gameObject)
{
	m_box->setGameObject(gameObject);
	m_box->setCollisionType(static_cast<std::size_t>(gameObject->getObjectType()));
}

void NanoRobot::setLaserColor(sf::Color const & color)
{
	for (std::size_t i = 0u; i < 12u; i++)
	{
		m_ray[i].color = color;
		m_ray[i].color.a = 100;
	}

	m_ray[1].color.a = 0;
	m_ray[2].color.a = 0;
	m_ray[4].color.a = 0;
	m_ray[7].color.a = 0;
}

void NanoRobot::setRepairShipPosition(sf::Vector2f const & position)
{
	m_repairShipPosition = m_target + position;
}

void NanoRobot::makeLaser(sf::Vertex* vertices, sf::Vector2f const& p0, sf::Vector2f const& p1, float thickness)
{
	static float const size = 8.f;
	sf::Vector2f	p(-(p1.y - p0.y), p1.x - p0.x);
	float			halfThickness = thickness * .5f;

	octo::normalize(p);
	p *= halfThickness;

	vertices[0].position = p0;
	vertices[1].position = p0 + p;
	vertices[2].position = p1 + p;
	vertices[3].position = p1;

	vertices[4].position = p0 - p;
	vertices[5].position = p0;
	vertices[6].position = p1;
	vertices[7].position = p1 - p;

	vertices[8].position = p0 + sf::Vector2f(-size, -size);
	vertices[9].position = p0 + sf::Vector2f(size, -size);
	vertices[10].position = p0 + sf::Vector2f(size, size);
	vertices[11].position = p0 + sf::Vector2f(-size, size);

	vertices[12].position = p1 + sf::Vector2f(-size, -size);
	vertices[13].position = p1 + sf::Vector2f(size, -size);
	vertices[14].position = p1 + sf::Vector2f(size, size);
	vertices[15].position = p1 + sf::Vector2f(-size, size);
}

void NanoRobot::playSoundRepair(void)
{
	octo::AudioManager &		audio = octo::Application::getAudioManager();
	octo::ResourceManager &		resources = octo::Application::getResourceManager();

	if ((m_state == Repair || m_state == RepairShip) && m_sound == nullptr)
		m_sound = audio.playSound(resources.getSound(REPAIR_WITH_LAZER_OGG), 0.6f);
	if (m_state != Repair && m_state != RepairShip && m_sound != nullptr)
	{
		m_sound->stop();
		m_sound = nullptr;
	}
}

void NanoRobot::addMapOffset(float x, float y)
{
	sf::Vector2f position = getPosition() + sf::Vector2f(x, y);
	if (m_startLastAnimation)
		return;
	if (std::abs(position.x - m_swarm.getFirefly(0u).position.x) > 400.f)
		m_isTravelling = true;
	else
		m_isTravelling = false;
	m_swarm.setTarget(position);
}

void NanoRobot::transfertToOcto(bool inInit)
{
	PhysicsEngine::getInstance().unregisterShape(m_box);
	m_box = nullptr;
	m_positionBehavior->setRadius(250.f);
	m_swarm.getFirefly(0u).speed = 1.f;
	m_state = Speak;
	m_glowingEffect.onTransfer();
	if (!inInit)
		Progress::getInstance().addNanoRobot();
}

void NanoRobot::setTarget(sf::Vector2f const & target)
{
	m_target = target;
}

void NanoRobot::setSwarmTarget(sf::Vector2f const & position)
{
	m_swarm.setTarget(position);
}

void NanoRobot::setPosition(sf::Vector2f const & position)
{
	sf::Vector2f pos = position + sf::Vector2f(0.f, -200.f);
	if (m_startLastAnimation)
		return;
	if (std::abs(pos.x - m_swarm.getFirefly(0u).position.x) > 400.f)
		m_isTravelling = true;
	else
		m_isTravelling = false;
	m_swarm.setTarget(pos);
}

bool NanoRobot::isTravelling(void) const
{
	return m_isTravelling;
}

void NanoRobot::popUpInfo(void)
{
	if (m_popUp == false)
		m_popUp = true;
}

void NanoRobot::updatePopUpInfo(sf::Time frametime)
{
	if (m_popUp && m_popUpTimer <= m_popUpTimerMax)
	{
		m_infoBubble.setup(m_infoText, sf::Color::White, 20u, 600u);
		m_infoBubble.setType(ABubble::Type::Speak);
		m_infoBubble.setActive(true);
		m_popUpTimer += frametime;
	}
	else
	{
		m_popUp = false;
		m_popUpTimer = sf::Time::Zero;
		m_infoBubble.setType(ABubble::Type::None);
		m_infoBubble.setActive(false);
	}
}

bool NanoRobot::onInputPressed(InputListener::OctoKeys const & key)
{
	switch (key)
	{
		case OctoKeys::Infos:
		{
			if (m_infoSetup == false && m_infoText.size())
			{
				m_infoBubble.setup(m_infoText, sf::Color(255, 255, 255, 200), 20u, 600u);
				m_infoBubble.setType(ABubble::Type::Speak);
				m_infoBubble.setActive(true);
				m_infoSetup = true;
			}
		}
		default:
			break;
	}
	return true;
}

bool NanoRobot::onInputReleased(InputListener::OctoKeys const & key)
{
	switch (key)
	{
		case OctoKeys::Infos:
		{
			if (m_infoSetup == true)
			{
				m_infoBubble.setType(ABubble::Type::None);
				m_infoBubble.setActive(false);
				m_infoSetup = false;
			}
		}
		default:
			break;
	}
	return true;
}

void NanoRobot::setState(NanoRobot::State state)
{
	if (m_startLastAnimation)
		return;
	if (state == GoingToRepairShip)
		m_startLastAnimation = true;
	m_state = state;
}

void NanoRobot::setTextIndex(std::size_t index)
{
	assert(index < m_texts.size());
	m_textIndex = index;
}

void NanoRobot::setTargets(std::vector<sf::Vector2f> const & targets, float travelDuration)
{
	m_targets = targets;
	m_timerRepairMax = sf::seconds(travelDuration);
}

sf::Vector2f const & NanoRobot::getPosition(void) const
{
	return m_swarm.getFirefly(0u).position;
}

sf::Vector2f const & NanoRobot::getTargetPosition(void)
{
	if (std::abs(m_swarm.getTarget().x - m_swarm.getFirefly(0u).position.x) > 400.f)
		m_isTravelling = true;
	else
		m_isTravelling = false;
	return m_swarm.getTarget();
}

NanoRobot::State NanoRobot::getState(void) const
{
	return m_state;
}

bool NanoRobot::isReparingShip(void) const
{
	return m_startLastAnimation;
}

void NanoRobot::setLaserConvergence(sf::Vector2f const & position)
{
	m_laserConvergence = position;
}

void NanoRobot::setUsePathLaser(bool usePathLaser)
{
	m_usePathLaser = usePathLaser;
}

sf::Vector2f const & NanoRobot::getLaserConvergence(void)
{
	return m_laserConvergence;
}

void NanoRobot::update(sf::Time frametime)
{
	m_swarm.update(frametime);
	m_sprite.update(frametime);

	sf::Vector2f const & pos = m_swarm.getFirefly(0u).position;
	m_sprite.setPosition(pos - m_sprite.getGlobalSize() / 2.f);

	if (m_box)
	{
		m_box->setPosition(pos.x - m_box->getRadius(), pos.y - m_box->getRadius());
		m_box->update();
	}

	m_particles.canEmit(false);
	if (m_state == Speak)
	{
		m_timer += frametime;
		if (m_timer > m_timerMax)
			m_state = FollowOcto;
	}
	else if (m_state == Repair)
	{
		makeLaser(m_ray.get(), getPosition() + m_offsetLaser, m_target, 4.f);
		m_particles.canEmit(true);
		m_particles.setPosition(m_target);
	}
	else if (m_state == GoingToRepairShip)
	{
		m_timerRepair += frametime;
		m_positionBehavior->setRadius(30.f);
		m_swarm.setTarget(m_repairShipPosition);
		if (m_timerRepair > m_timerRepairMax)
		{
			m_timerRepair = sf::Time::Zero;
			m_state = RepairShip;
		}
	}
	else if (m_state == RepairShip)
	{
		m_timerRepair += frametime;
		m_swarm.setTarget(m_repairShipPosition);
		if (m_timerRepair > m_timerRepairMax)
		{
			m_timerRepair = sf::Time::Zero;
			m_repairIndex++;
		}
		sf::Vector2f target = m_target;
		if (m_usePathLaser)
		{
			std::size_t index2 = (m_repairIndex + 1u) % m_targets.size();
			target = octo::linearInterpolation(m_target + m_targets[m_repairIndex % m_targets.size()], m_target + m_targets[index2], m_timerRepair / m_timerRepairMax);
		}
		makeLaser(m_ray.get(), getPosition() + m_offsetLaser, target, 4.f);
		m_particles.canEmit(true);
		m_particles.setPosition(target);
	}
	playSoundRepair();
	m_particles.update(frametime);

	m_texts[m_textIndex]->setPosition(m_sprite.getPosition() - sf::Vector2f(0.f, 0.f));
	m_texts[m_textIndex]->update(frametime);
	m_infoBubble.setPosition(m_sprite.getPosition() - sf::Vector2f(0.f, 0.f));
	m_infoBubble.update(frametime);
	updatePopUpInfo(frametime);
	m_glowingEffect.setPosition(pos);
	m_sprite.setScale(m_glowingEffect.getNanoScale());
	m_glowingEffect.update(frametime);

	updateRepairShip(frametime);
}

void NanoRobot::updateRepairShip(sf::Time frameTime)
{
	if (isReparingShip())
	{
		m_timerRepairShip += frameTime;
		float speed = m_timerRepairShip.asSeconds() + octo::Pi2 * m_multiplier / 6.f;
		setRepairShipPosition(sf::Vector2f(std::cos(speed), std::sin(speed)) * 150.f);
		setTarget(m_laserConvergence);
	}
}

void NanoRobot::draw(sf::RenderTarget& render, sf::RenderStates) const
{
	if (!m_isTravelling || m_state == FollowOcto || m_state == Speak || m_state == Repair || m_state == RepairShip || m_state == GoingToRepairShip)
	{
		render.draw(m_glowingEffect);
		render.draw(m_sprite);
	}
	if (m_state == Repair || m_state == RepairShip)
	{
		render.draw(m_ray.get(), 8, sf::Quads);
		render.draw(m_ray.get() + 8, 8u, sf::Quads, m_texture);
	}
	m_particles.draw(render);
	if (m_infoBubble.isActive())
		m_infoBubble.draw(render);
}

void NanoRobot::drawText(sf::RenderTarget& render, sf::RenderStates) const
{
	if (m_state == Idle)
		return;
	if (m_state == Speak)
		m_texts[m_textIndex]->draw(render);
}
