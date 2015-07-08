#ifndef PHYSICSMAPSCREEN_HPP
# define PHYSICSMAPSCREEN_HPP

# include <AbstractState.hpp>
# include <GraphicsListeners.hpp>
# include <Camera.hpp>
# include "PhysicsEngine.hpp"
# include "GroundManager.hpp"
# include "DefaultBiome.hpp"

class ConvexShape;
class RectangleShape;
class GroupShape;

class PhysicsMapScreen : public octo::AbstractState, public octo::DefaultKeyboardListener
{
public:
	PhysicsMapScreen(void);

	virtual void	start();
	virtual void	pause();
	virtual void	resume();
	virtual void	stop();
	virtual void	update(sf::Time frameTime);
	virtual void	draw(sf::RenderTarget& render)const;

private:
	PhysicsEngine &					m_engine;
	octo::Camera &					m_camera;
	ConvexShape *					m_shape;
	std::vector<RectangleShape *>	m_shapes;
	GroundManager					m_groundManager;
	DefaultBiome					m_biome;
	GroupShape *					m_groupShape;

	virtual bool onPressed(sf::Event::KeyEvent const & event);

};

#endif
