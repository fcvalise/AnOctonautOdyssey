#include "JumpNanoRobot.hpp"
#include "ResourceDefinitions.hpp"

JumpNanoRobot::JumpNanoRobot(void) :
	NanoRobot(sf::Vector2f(9700, -1400.f), NANO_JUMP_OSS, 4, 654, sf::Vector2f(0.f, -24.f))
{
	setup(this);

	std::vector<sf::Vector2f> targets;
	targets.push_back(sf::Vector2f(284.f, 339.f));
	targets.push_back(sf::Vector2f(328.f, 369.f));
	targets.push_back(sf::Vector2f(331.f, 381.f));
	targets.push_back(sf::Vector2f(320.f, 396.f));
	targets.push_back(sf::Vector2f(308.f, 398.f));
	targets.push_back(sf::Vector2f(264.f, 367.f));
	targets.push_back(sf::Vector2f(263.f, 362.f));
	targets.push_back(sf::Vector2f(278.f, 340.f));
	setTargets(targets, 0.6f);
}
