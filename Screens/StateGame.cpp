/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   StateGame.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: irabeson <irabeson@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2015/04/15 19:29:33 by irabeson          #+#    #+#             */
/*   Updated: 2015/04/15 19:31:19 by irabeson         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "StateGame.hpp"
#include "../ResourceDefinitions.hpp"
#include <Application.hpp>
#include <Camera.hpp>
#include <ResourceManager.hpp>

void	StateGame::start()
{
	m_mapManager.init(Map::e_map_instance);

	octo::ResourceManager&	resources = octo::Application::getResourceManager();
	m_back.setSize(sf::Vector2f(1900, 1080));
	m_back.setTexture(&resources.getTexture(BACKGROUND_PNG));
}

void	StateGame::pause()
{
}

void	StateGame::resume()
{
}

void	StateGame::stop()
{
}

void	StateGame::update(sf::Time p_deltatime)
{
	m_mapManager.update(p_deltatime.asSeconds());
}

void	StateGame::draw(sf::RenderTarget& render) const
{
	render.clear(sf::Color::Black);
	render.draw(m_back);
	render.draw(m_mapManager);
}
