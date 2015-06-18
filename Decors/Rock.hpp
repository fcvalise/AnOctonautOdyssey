#ifndef ROCK_HPP
# define ROCK_HPP

# include "ADecor.hpp"
# include <SFML/Graphics/Color.hpp>

class Rock : public ADecor
{
struct OctogonValue
{
	sf::Vector2f			size;
	sf::Vector2f			origin;
	float					sizeLeft;
	float					sizeRight;
	float					sizeRec;
};

public:
	Rock(void);
	virtual ~Rock(void) = default;

	virtual void	setup(ABiome& biome);
	virtual void	update(sf::Time frameTime,
						   DecorBuilder& builder,
						   ABiome& biome);

private:
	sf::Vector2f				m_size;
	sf::Color					m_color;
	unsigned int				m_partCount;
	std::vector<OctogonValue>	m_values;
	float						m_animation;

	sf::Vector2f				m_left;
	sf::Vector2f				m_right;

	void createOctogon(sf::Vector2f const & size,
						sf::Vector2f const & origin,
						sf::Color const & color,
						float const & sizeLeft,
						float const & sizeRight,
						float const & sizeRec,
						sf::Vector2f const & position,
						DecorBuilder& builder);

	void createRock(std::vector<OctogonValue> const & values,
					sf::Vector2f originRock,
					sf::Color const & color,
					DecorBuilder& builder);

	float randomFloat(float min, float max);
};

#endif
