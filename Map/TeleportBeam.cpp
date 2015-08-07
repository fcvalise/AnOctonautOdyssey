/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TeleportBeam.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: irabeson <irabeson@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2015/08/01 04:30:42 by irabeson          #+#    #+#             */
/*   Updated: 2015/08/07 11:27:44 by irabeson         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "TeleportBeam.hpp"
#include "ResourceDefinitions.hpp"

#include <Application.hpp>
#include <ResourceManager.hpp>

#include <random>

class TeleportBeam::BeamParticle : public octo::ParticleSystem<sf::Time, float>
{
	enum MyComponent
	{
		ElapsedTime = Component::User,
		UpSpeed
	};
public:
	BeamParticle() :
		m_cycleTime(sf::seconds(4)),
		m_speedUp(64.f),
		m_distanceFactor(0.2f),
		m_thickness(150.f),
		m_height(300.f),
		m_distri(0.f, 1.f)
	{
		octo::ResourceManager&	resources = octo::Application::getResourceManager();
		sf::Texture const&		texture = resources.getTexture(FIREFLY01_PNG);
		sf::Vector2f			texSize(texture.getSize());

		reset({sf::Vertex({0.f, 0.f}, {0.f, 0.f}),
			   sf::Vertex({16.f, 0.f}, {0.f, texSize.y}),
			   sf::Vertex({16.f, 16.f}, {texSize.x, texSize.y}),
			   sf::Vertex({0.f, 16.f}, {texSize.x, 0.f})},
			   sf::Quads, 1000u);
		setTexture(texture);
		for (auto i = 0; i < 80; ++i)
			createParticle();
	}

	void	setThickness(float thickness)
	{
		m_thickness = thickness;
	}

	void	setHeight(float height)
	{
		m_height = height;
	}

	void	updateParticle(sf::Time frameTime, Particle& particle)
	{
		sf::Vector2f	position = std::get<Component::Position>(particle);
		sf::Time		currentTime = std::get<MyComponent::ElapsedTime>(particle) + frameTime;
		sf::Color&		color = std::get<Component::Color>(particle);
		float			scaleFactor;
		float			cycle = currentTime / m_cycleTime;

		position.y -= frameTime.asSeconds() * std::get<MyComponent::UpSpeed>(particle);
		position.x = std::cos(cycle * octo::Pi2) * m_thickness * 0.5f;
		scaleFactor = 1.f + std::sin(cycle * octo::Pi2) * m_distanceFactor;
		color.a = (position.y / m_height) * 255;
		if (currentTime >= m_cycleTime)
		{
			currentTime -= m_cycleTime;
		}
		std::get<MyComponent::ElapsedTime>(particle) = currentTime;
		std::get<Component::Position>(particle) = position;
		std::get<Component::Scale>(particle) = sf::Vector2f(scaleFactor, scaleFactor);
	}

	void	createParticle()
	{
		emplace(sf::Color(255, 255, 255),
				sf::Vector2f(0, m_distri(m_random) * -200.f),
				sf::Vector2f(1.f, 1.f), 0.f,
				sf::seconds(m_distri(m_random) * m_cycleTime.asSeconds()),
				m_speedUp);
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
	float			m_distanceFactor;
	float			m_thickness;
	float			m_height;
	std::mt19937	m_random;
	Distri			m_distri;
};

TeleportBeam::TeleportBeam() :
	m_particles(new BeamParticle),
	m_color(sf::Color::Blue),
	m_thickness(150.f)
{
	m_builder = octo::VertexBuilder(m_vertices.data(), 6u);

	m_particles->setThickness(m_thickness);
}

void	TeleportBeam::setPoints(sf::Vector2f const& top, sf::Vector2f const& bottom)
{
	m_particles->setPosition(bottom);
	m_particles->setHeight(bottom.y - top.y);
	m_builder.clear();
	//m_builder.createLine(top, bottom, m_thickness, sf::Color::Red);
}

void	TeleportBeam::update(sf::Time frameTime)
{
	m_particles->update(frameTime);
}

void	TeleportBeam::draw(sf::RenderTarget& render)const
{
	sf::RenderStates	states;

	states.shader = &m_shaders;
	//render.draw(m_vertices.data(), m_builder.getUsed(), sf::Triangles, states);
	m_particles->draw(render, states);
}
