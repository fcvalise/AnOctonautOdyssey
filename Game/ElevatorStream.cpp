/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ElevatorStream.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: irabeson <irabeson@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2015/08/01 04:30:42 by irabeson          #+#    #+#             */
/*   Updated: 2015/08/12 19:28:54 by irabeson         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ElevatorStream.hpp"
#include "ResourceDefinitions.hpp"

#include <Application.hpp>
#include <ResourceManager.hpp>

#include <random>
#include <ctime>

class ElevatorStream::BeamParticle : public octo::ParticleSystem<sf::Time, float>
{
	enum MyComponent
	{
		ElapsedTime = Component::User,
		UpSpeed
	};
public:
	BeamParticle() :
		m_cycleTime(sf::seconds(4)),
		m_speedUp(512.f),
		m_width(200.f),
		m_height(300.f),
		m_emitInterval(sf::seconds(0.025f)),
		m_random(std::time(0)),
		m_distri(0.f, 1.f)
	{
		octo::ResourceManager&	resources = octo::Application::getResourceManager();
		sf::Texture const&		texture = resources.getTexture(FIREFLY01_PNG);
		sf::Vector2f const		texSize(texture.getSize());

		reset({sf::Vertex({0.f, 0.f}, {0.f, 0.f}),
			   sf::Vertex({16.f, 0.f}, {0.f, texSize.y}),
			   sf::Vertex({16.f, 16.f}, {texSize.x, texSize.y}),
			   sf::Vertex({0.f, 16.f}, {texSize.x, 0.f})},
			   sf::Quads, 1000u);
		setTexture(texture);
	}

	void	setWidth(float width)
	{
		m_width = width;
	}

	void	setHeight(float height)
	{
		m_height = height;
	}

	void	setColor(sf::Color const& color)
	{
		m_color = color;
	}

	void	updateParticle(sf::Time frameTime, Particle& particle)
	{
		sf::Vector2f	position = std::get<Component::Position>(particle);
		sf::Time		currentTime = std::get<MyComponent::ElapsedTime>(particle) + frameTime;
		sf::Color&		color = std::get<Component::Color>(particle);
		float			cycle = currentTime / m_cycleTime;
		float			heightPos = 0.f;

		position.y -= frameTime.asSeconds() * std::get<MyComponent::UpSpeed>(particle);
		heightPos = (position.y / m_height);
		position.x = std::cos(cycle * octo::Pi2) * m_width * 0.5f;
		color.a = heightPos * 255;
		if (currentTime >= m_cycleTime)
		{
			currentTime -= m_cycleTime;
		}
		std::get<MyComponent::ElapsedTime>(particle) = currentTime;
		std::get<Component::Position>(particle) = position;
		std::get<MyComponent::UpSpeed>(particle) *= 1.0001f;
	}

	void	update(sf::Time frameTime)
	{
		bool	emitParticle = false;

		m_emitTimer += frameTime;
		while (m_emitTimer > m_emitInterval)
		{
			emitParticle = true;
			m_emitTimer -= m_emitInterval;
		}
		if (emitParticle)
			createParticle();
		ParticleSystem::update(frameTime);
	}

	void	createParticle()
	{
		emplace(m_color,
				sf::Vector2f(0, 0),
				sf::Vector2f(1.f, 1.f), 0.f,
				sf::seconds(m_distri(m_random) * m_cycleTime.asSeconds()),
				std::max(0.2f, m_distri(m_random)) * m_speedUp);
	}
private:
	bool	isDeadParticle(Particle const& particle)
	{
		return (std::get<Component::Position>(particle).y < -m_height);
	}
private:
	typedef std::uniform_real_distribution<float>	Distri;

	sf::Time		m_cycleTime;
	float			m_speedUp;
	float			m_width;
	float			m_height;
	sf::Time		m_emitTimer;
	sf::Time		m_emitInterval;
	std::mt19937	m_random;
	Distri			m_distri;
	sf::Color		m_color;
};

ElevatorStream::ElevatorStream() :
	m_particles(new BeamParticle),
	m_waveCycleDuration(sf::seconds(0.5))
{
	octo::ResourceManager&	resources = octo::Application::getResourceManager();

	m_particles->setWidth(150.f);
	m_particles->setColor(sf::Color::White);
	m_shaders.loadFromMemory(resources.getText(ELEVATOR_VERT), sf::Shader::Vertex);
	m_shaders.setParameter("wave_amplitude", 5.f);
}

void	ElevatorStream::setPosX(float x)
{
	sf::Vector2f	pos = m_particles->getPosition();

	pos.x = x;
	m_particles->setPosition(pos);
}

void	ElevatorStream::setPosY(float y)
{
	sf::Vector2f	pos = m_particles->getPosition();

	pos.y = y;
	m_particles->setPosition(pos);
}

void	ElevatorStream::setHeight(float height)
{
	m_particles->setHeight(height);
}

void	ElevatorStream::setWidth(float width)
{
	m_particles->setWidth(width);
}

void	ElevatorStream::setColor(sf::Color const& color)
{
	m_particles->setColor(color);
}

void	ElevatorStream::update(sf::Time frameTime)
{
	m_particles->update(frameTime);
	m_waveCycle += frameTime;
	m_shaders.setParameter("wave_phase", m_waveCycle.asSeconds());
}

void	ElevatorStream::draw(sf::RenderTarget& render)const
{
	sf::RenderStates	states;

	states.shader = &m_shaders;
	m_particles->draw(render, states);
}
