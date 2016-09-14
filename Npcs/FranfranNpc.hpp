#ifndef FRANFRANNPC_HPP
# define FRANFRANNPC_HPP

# include "ASpecialNpc.hpp"
# include "SmokeSystem.hpp"

class FranfranNpc : public ASpecialNpc
{
public:
	FranfranNpc(void);
	virtual ~FranfranNpc(void) = default;

	virtual void setup(void);
	virtual void update(sf::Time frametime);
	virtual void draw(sf::RenderTarget & render, sf::RenderStates states) const;

private:
	SmokeSystem	m_smoke;
	SmokeSystem m_puff;
	sf::Time	m_puffTimer;
	sf::Time	m_puffTimerMax;
};

#endif
