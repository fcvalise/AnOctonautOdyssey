#ifndef CHARACTEROCTO_HPP
# define CHARACTEROCTO_HPP
# include <CharacterSprite.hpp>
# include <CharacterAnimation.hpp>
# include <DefaultGraphicsListeners.hpp>
# include "AGameObject.hpp"
# include "RectangleShape.hpp"
# include "CircleShape.hpp"
# include "NanoRobot.hpp"
# include "SmokeSystem.hpp"

# include <SFML/Graphics/Drawable.hpp>
# include <array>

class ElevatorStream;
class RepairNanoRobot;

class CharacterOcto : public AGameObject<GameObjectType::Player>,
	public octo::DefaultKeyboardListener,
	public sf::Drawable
{
	enum Events
	{
		Idle,
		Right,
		Left,
		Jump,
		DoubleJump,
		Fall,
		Dance,
		Umbrella,
		Death,
		Drink,
	};
	public:
	friend class OctoEvent;

	class OctoEvent : public AGameObject<GameObjectType::PlayerEvent>
	{
	public:
		CharacterOcto *	m_octo;
	};

	CharacterOcto();
	virtual ~CharacterOcto();

	void					setup(void);
	void					update(sf::Time frameTime);
	void					draw(sf::RenderTarget& render, sf::RenderStates states = sf::RenderStates())const;

	bool					onPressed(sf::Event::KeyEvent const& event);
	bool					onReleased(sf::Event::KeyEvent const& event);
	sf::Vector2f const &	getPosition() const;
	sf::Vector2f			getBubblePosition() const;
	void					onCollision(GameObjectType type, sf::Vector2f const& collisionDirection);
	void					setTopElevator(float top);
	void					setPosition(sf::Vector2f const & position);
	void					giveNanoRobot(NanoRobot * robot);
	void					giveRepairNanoRobot(RepairNanoRobot * robot);
	void					repairElevator(ElevatorStream & elevator);

	private:
	bool	dieFall();
	bool	endDeath();
	void	timeEvent(sf::Time frameTime);
	void	setupAnimation();
	void	setupMachine();
	void	collisionTileUpdate();
	void	onSky(Events event);
	void	collisionElevatorUpdate();
	void	commitControlsToPhysics(float frametime);
	void	commitPhysicsToGraphics();
	void	caseLeft();
	void	caseRight();
	void	caseSpace();
	void	caseUp();
	void	dance();

	private:
	octo::CharacterSprite		m_sprite;
	float						m_spriteScale;
	octo::CharacterAnimation	m_idleAnimation;
	octo::CharacterAnimation	m_walkAnimation;
	octo::CharacterAnimation	m_jumpAnimation;
	octo::CharacterAnimation	m_fallAnimation;
	octo::CharacterAnimation	m_danceAnimation;
	octo::CharacterAnimation	m_umbrellaAnimation;
	octo::CharacterAnimation	m_deathAnimation;
	octo::CharacterAnimation	m_drinkAnimation;
	RectangleShape *			m_box;
	CircleShape *				m_eventBox;
	OctoEvent					m_octoEvent;
	std::vector<std::unique_ptr<NanoRobot>>		m_nanoRobots;
	RepairNanoRobot *			m_repairNanoRobot;

	sf::Time					m_timeEventFall;
	sf::Time					m_timeEventIdle;
	sf::Time					m_timeEventDeath;
	sf::Time					m_timeEventInk;
	float						m_pixelSecondJump;
	float						m_pixelSecondUmbrella;
	float						m_pixelSecondWalk;
	float						m_pixelSecondAfterJump;
	float						m_pixelSecondAfterFullJump;
	float						m_pixelSecondMultiplier;
	float						m_jumpVelocity;
	float						m_afterJumpVelocity;
	float						m_previousTop;
	float						m_topElevator;
	float						m_deltaPositionY;
	std::size_t					m_numberOfJump;
	bool						m_originMove;
	bool						m_onGround;
	bool						m_onElevator;
	bool						m_onTopElevator;
	bool						m_afterJump;
	bool						m_keyLeft;
	bool						m_keyRight;
	bool						m_keySpace;
	bool						m_keyUp;
	bool						m_collisionTile;
	bool						m_collisionElevator;
	bool						m_collisionElevatorEvent;
	SmokeSystem					m_ink;
};

#endif
