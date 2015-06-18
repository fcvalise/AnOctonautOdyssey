#include "Rock.hpp"
#include "DecorBuilder.hpp"
#include "ABiome.hpp"

//TODO: Dont forget to delete
#include <random>

Rock::Rock() :
	m_partCount(0u),
	m_animation(1.f)
{
}

void Rock::createOctogon(sf::Vector2f const & size, sf::Vector2f const & origin, sf::Color const & color, float const & sizeLeft, float const & sizeRight, float const & sizeRec, sf::Vector2f const & rockOrigin, DecorBuilder& builder)
{
	sf::Vector2f upLeft(-size.x, -size.y);
	sf::Vector2f upRight(size.x, -size.y);
	sf::Vector2f left(-size.x - sizeLeft, -size.y + sizeLeft);
	sf::Vector2f right(size.x + sizeRight, -size.y + sizeRight);
	sf::Vector2f midLeft(-size.x, -size.y + sizeLeft);
	sf::Vector2f midRight(size.x, -size.y + sizeRight);
	sf::Vector2f downLeft(-size.x - sizeLeft, 0.f);
	sf::Vector2f downRight(size.x + sizeRight, 0.f);
	sf::Vector2f downMidLeft(-size.x, 0.f);
	sf::Vector2f downMidRight(size.x, 0.f);
	sf::Vector2f recUp(upLeft.x + sizeRec, upLeft.y);
	sf::Vector2f recLeft(upLeft.x, upLeft.y + 4.0f);
	sf::Vector2f recRight(recUp.x, recUp.y + 4.0f);

	// Avoid under limit point when grows
	midLeft.y = midLeft.y > 0.f ? 0.f : midLeft.y;
	left.y = left.y > 0.f ? 0.f : left.y;
	midRight.y = midRight.y > 0.f ? 0.f : midRight.y;
	right.y = right.y > 0.f ? 0.f : right.y;

	builder.createTriangle(right + origin, midRight + origin, upRight + origin, color);

	builder.createQuad(upLeft + origin, upRight + origin, midRight + origin, midLeft + origin, color);
	builder.createQuad(midLeft + origin, midRight + origin, downMidRight + origin, downMidLeft + origin, color);
	builder.createQuad(left + origin, midLeft + origin, downMidLeft + origin, downLeft + origin, color);
	builder.createQuad(right + origin, midRight + origin, downMidRight + origin, downRight + origin, color);

	builder.createTriangle(left + origin, midLeft + origin, upLeft + origin, color + sf::Color(100, 100, 100));
	builder.createQuad(upLeft + origin, recUp + origin, recRight + origin, recLeft + origin, color + sf::Color(100, 100, 100));

	// Compute last left point
	if (origin.x - rockOrigin.x + downLeft.x < m_left.x && origin.x - rockOrigin.x < 0.f)
		m_left.x = origin.x - rockOrigin.x + downLeft.x;
	// Compute last right point
	if (origin.x - rockOrigin.x + downRight.x > m_right.x && origin.x - rockOrigin.x > 0.f)
		m_right.x = origin.x - rockOrigin.x + downRight.x;
}

void Rock::createRock(std::vector<OctogonValue> const & values, sf::Vector2f originRock, sf::Color const & color, DecorBuilder& builder)
{
	for (unsigned int i = 0; i < m_partCount; i++)
		createOctogon(sf::Vector2f(values[i].size.x, values[i].size.y * m_animation), values[i].origin + originRock, color,
						values[i].sizeLeft, values[i].sizeRight, values[i].sizeRec * m_animation, originRock, builder);
	builder.createTriangle(m_left + originRock, m_right + originRock, sf::Vector2f(0.0f, (m_right.x - m_left.x) / 2.f) + originRock, color);
}

void Rock::setup(ABiome& biome)
{
	m_size = biome.getRockSize();
	m_color = biome.getRockColor();
	m_partCount = biome.getRockPartCount();
	m_values.resize(m_partCount);

	unsigned int i = 0;
	float totalX = 0;
	float cornerSize = m_size.x / (m_partCount * 2.f);
	sf::Vector2f size;

	// Compute left random values
	size.x = cornerSize;
	size.y = m_size.y;
	sf::Vector2f origin = sf::Vector2f(0.f, 0.f);
	while (i < m_partCount / 2)
	{
		size.x = randomFloat(cornerSize * 0.5f, cornerSize);
		totalX += size.x;
		size.y -= totalX;
		origin.x += randomFloat(-totalX, 0.f);
		if (size.x * 2 < size.y)
		{
			m_values[i].size = size;
			m_values[i].origin = origin;
			m_values[i].sizeLeft = randomFloat(size.x, size.x * 2);
			m_values[i].sizeRight = randomFloat(size.x, size.x * 2);
			m_values[i].sizeRec = randomFloat(10.f, size.x * 2);
		}
		else
			break;
		i++;
	}

	// Compute right random values
	totalX = 0;
	size.x = cornerSize;
	size.y = m_size.y;
	origin = sf::Vector2f(0.f + size.x, 0.f);
	while (i < m_partCount)
	{
		size.x = randomFloat(cornerSize * 0.5f, cornerSize);
		totalX += size.x;
		size.y -= totalX;
		origin.x += randomFloat(0.0f, totalX);
		if (size.x * 2 < size.y)
		{
			m_values[i].size = size;
			m_values[i].origin = origin;
			m_values[i].sizeLeft = randomFloat(size.x, size.x * 2);
			m_values[i].sizeRight = randomFloat(size.x, size.x * 2);
			m_values[i].sizeRec = randomFloat(10.f, size.x * 2);
		}
		i++;
	}
}

void Rock::update(sf::Time, DecorBuilder& builder, ABiome&)
{
	sf::Vector2f const & position = getPosition();
	//TODO: Test this with terrain
	createRock(m_values, sf::Vector2f(position.x, position.y + m_size.x / 2.f), m_color, builder);
}

// TODO: To delete and add in Biome
float Rock::randomFloat(float min, float max)
{
	if (max - min == 0)
		return max;
	std::uniform_int_distribution<float> distribution(min, max);
	//TODO: REplace by timestamp unix
	std::random_device rd;
	std::mt19937 engine(rd());

	return distribution(engine);
}

