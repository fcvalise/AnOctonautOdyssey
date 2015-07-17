#ifndef PARALLAXSCROLLING_HPP
# define PARALLAXSCROLLING_HPP

# include <SFML/Graphics.hpp>
# include <vector>
# include <initializer_list>

class ParallaxScrolling : public sf::Drawable
{
public:
	class ALayer : public sf::Drawable
	{
	public:
		ALayer(void) :
			ALayer(sf::Vector2f(0.5f, 0.5f))
		{}
		ALayer(sf::Vector2f const & speed) :
			m_speed(speed)
		{}
		virtual ~ALayer(void) = default;

		inline void setSpeed(sf::Vector2f const & speed) { m_speed = speed; }
		inline sf::Vector2f const & getSpeed(void) const { return m_speed; }

		virtual void update(float deltatime) = 0;
		virtual void draw(sf::RenderTarget & render, sf::RenderStates states) const = 0;

	private:
		sf::Vector2f	m_speed;
	};

	ParallaxScrolling(void);
	ParallaxScrolling(std::initializer_list<ALayer *> list);
	virtual ~ParallaxScrolling(void);

	/*! Add a layer */
	void addLayer(ALayer * layer);
	/*! Remove the layer from the vector and release the allocated memory */
	void removeLayer(std::size_t index);
	/*! Remove all layers from the vector and release the allocated memory */
	void removeAllLayers(void);

	virtual void update(float deltatime);
	virtual void draw(sf::RenderTarget & render, sf::RenderStates states) const;

private:
	//TODO: use unique_ptr
	//TODO:get map size from biome here
	std::vector<ALayer *>	m_layers;

};

#endif
