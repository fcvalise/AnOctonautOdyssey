#include "RandomGenerator.hpp"

RandomGenerator::RandomGenerator(void)
{
	std::vector<double> i{0, 400, 450, 500, 550, 600, 1000};
	std::vector<double> w{0, 1, 2, 3, 2, 1, 0};
	m_distributionPiecewise.param(std::piecewise_linear_distribution<>::param_type(i.begin(), i.end(), w.begin()));
}

void RandomGenerator::setSeed(std::string const & string)
{
	if (string == "random")
	{
		std::random_device rd;
		m_engine.seed(rd());
	}
	else
	{
		std::seed_seq seed(string.begin(), string.end());
		m_engine.seed(seed);
	}
}

float RandomGenerator::randomFloat(float min, float max)
{
	m_distributionFloat.param(std::uniform_real_distribution<float>::param_type(min, max));
	return m_distributionFloat(m_engine);
}

int RandomGenerator::randomInt(int min, int max)
{
	m_distributionInt.param(std::uniform_int_distribution<int>::param_type(min, max));
	return m_distributionInt(m_engine);
}

bool RandomGenerator::randomBool(float percent)
{
	m_distributionBool.param(std::bernoulli_distribution::param_type(percent));
	return m_distributionBool(m_engine);
}

// Get value between 0 and 1000
// Increasing probability between 0 and 500
// Decreasing probility between 500 and 1000
std::size_t RandomGenerator::randomPiecewise(std::size_t max)
{
	std::size_t value = m_distributionPiecewise(m_engine);
	value = value * max / 1000;
	return value;
}

