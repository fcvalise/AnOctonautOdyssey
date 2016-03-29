#include "Concert.hpp"
#include <Interpolations.hpp>
#include <SFML/Graphics.hpp>
#include <Application.hpp>
#include <Camera.hpp>

Concert::Concert(void) :
	SimpleObject(CONCERT_OSS, CONCERT_FRAG, 20.f),
	m_particlesCount(13),
	m_particles(new MusicSystem[m_particlesCount])
{
	typedef octo::SpriteAnimation::Frame	Frame;
	setupAnimation({Frame(sf::seconds(0.2f), 0u),
					Frame(sf::seconds(0.2f), 1u),
					Frame(sf::seconds(0.2f), 2u),
					Frame(sf::seconds(0.2f), 3u)}, octo::LoopMode::Loop);

	setupBox(this, static_cast<std::size_t>(GameObjectType::Concert), static_cast<std::size_t>(GameObjectType::Player));
	getSprite().setScale(sf::Vector2f(0.8f, 0.8f));
	sf::FloatRect const & rect = octo::Application::getCamera().getRectangle();
	getShader().setParameter("resolution", rect.width, rect.height);
	for (std::size_t i = 0; i < m_particlesCount; i++)
		m_particles[i].canEmit(true);
}

Concert::~Concert(void)
{}

void Concert::update(sf::Time frameTime)
{
	sf::Shader & shader = getShader();

	if (getStartBalle())
	{
		float time;
		if (getTimer() < getEffectDuration() / 2.f)
			time = octo::linearInterpolation(0.f, 1.f, getTimer() / (getEffectDuration() / 2.f));
		else
			time = octo::linearInterpolation(1.f, 0.f, (getTimer() - getEffectDuration() / 2.f) / (getEffectDuration() / 2.f));
		shader.setParameter("time", time);
	}
	SimpleObject::update(frameTime);
	for (std::size_t i = 0; i < m_particlesCount; i++)
		m_particles[i].update(frameTime);
}

void Concert::setPosition(sf::Vector2f const & position)
{
	SimpleObject::setPosition(position - sf::Vector2f(0.f, getSprite().getGlobalSize().x));
	m_particles[0].setEmitter(position + sf::Vector2f(100.f, -180.f));
	m_particles[1].setEmitter(position + sf::Vector2f(300.f, -180.f));
	m_particles[2].setEmitter(position + sf::Vector2f(200.f, -180.f));
	for (std::size_t i = 3; i < m_particlesCount; i++)
		m_particles[i].setEmitter(position + sf::Vector2f(-20.f + i * 28.f, -325.f));
}

void Concert::draw(sf::RenderTarget &, sf::RenderStates) const
{}

void Concert::drawFront(sf::RenderTarget & render, sf::RenderStates) const
{
	render.draw(getSprite());
	for (std::size_t i = 0; i < m_particlesCount; i++)
		m_particles[i].draw(render);
}
