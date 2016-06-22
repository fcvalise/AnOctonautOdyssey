#ifndef LABORATORYENDSCREEN_HPP
# define LABORATORYENDSCREEN_HPP

# include <AbstractState.hpp>
# include <SFML/Graphics/Texture.hpp>
# include <SFML/Graphics/Sprite.hpp>
# include "InputListener.hpp"

class LaboratoryEndScreen : public octo::AbstractState,
						public InputListener
{
public:
	LaboratoryEndScreen(void);
	virtual void	start();
	virtual void	pause();
	virtual void	resume();
	virtual void	stop();
	virtual void	update(sf::Time frameTime);
	virtual void	draw(sf::RenderTarget& render)const;

	virtual bool	onInputPressed(InputListener::OctoKeys const & key);
	virtual bool	onInputReleased(InputListener::OctoKeys const & key);

private:
	sf::Texture			m_startTexture;
	sf::Sprite			m_startSprite;

};

#endif
