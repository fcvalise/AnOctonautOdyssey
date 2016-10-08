#include "MainMenu.hpp"
#include "PlayEndMenu.hpp"
#include "CheatCodeMenu.hpp"
#include "ControlMenu.hpp"
#include "CreditMenu.hpp"
#include "YesNoMenu.hpp"
#include "Progress.hpp"
#include "ResourceDefinitions.hpp"
#include <AudioManager.hpp>
#include <ResourceManager.hpp>
#include <Camera.hpp>
#include <Application.hpp>

//Sub Menu
class YesNoQuit : public YesNoMenu
{
	inline void setIndex(void) { setIndexCursor(0); }
	inline void actionYes(void)
	{
		Progress &				progress = Progress::getInstance();
		octo::StateManager &	states = octo::Application::getStateManager();

		if (!progress.isMenu())
		{
			states.setTransitionDuration(sf::seconds(0.5f), sf::seconds(0.5f));
			states.change("menu");
		}
		else
			octo::Application::stop();
	}
	inline void actionNo(void) { }
};

class YesNoReset : public YesNoMenu
{
	inline void setIndex(void) { setIndexCursor(0); }
	inline void actionYes(void)
	{
		Progress &				progress = Progress::getInstance();
		octo::StateManager &	states = octo::Application::getStateManager();
		progress.reset();
		progress.setFirstTime(true);
		states.change("zero");
	}

	inline void actionNo(void) { }
};

//MainMenu
MainMenu::MainMenu(void) :
	m_soundPlayed(false)
{
}

void MainMenu::createMenus(void)
{
	Progress &				progress = Progress::getInstance();

	if (progress.isMenu())
	{
		addMenu(AMenu::getText("menu_quit"), std::unique_ptr<YesNoQuit>(new YesNoQuit()));
		addMenu(AMenu::getText("menu_restart"), std::unique_ptr<YesNoReset>(new YesNoReset()));
		addMenu(AMenu::getText("menu_credits"), std::unique_ptr<CreditMenu>(new CreditMenu()));
		addMenu(AMenu::getText("menu_options"), std::unique_ptr<OptionMenu>(new OptionMenu()));
		addMenu(AMenu::getText("menu_new"), std::unique_ptr<EmptyMenu>(new EmptyMenu()));
		if (!progress.isGameFinished())
			addMenu(AMenu::getText("menu_play"), std::unique_ptr<EmptyMenu>(new EmptyMenu()));
		else
			addMenu(AMenu::getText("menu_play"), std::unique_ptr<PlayEndMenu>(new PlayEndMenu()));
		setCharacterSize(50);
		setBubbleType(ABubble::Type::MainMenu);
		setCursorAtEnd();
	}
	else
	{
		addMenu(AMenu::getText("menu_controls"), std::unique_ptr<ControlMenu>(new ControlMenu()));
		#ifndef NDEBUG
		addMenu(L"Easy", std::unique_ptr<CheatCodeMenu>(new CheatCodeMenu()));
		#endif
		addMenu(AMenu::getText("menu_options"), std::unique_ptr<OptionMenu>(new OptionMenu()));
		addMenu(AMenu::getText("menu_return"), std::unique_ptr<YesNoQuit>(new YesNoQuit()));
		setCharacterSize(30);
		setBubbleType(ABubble::Type::Think);
	}
}

void MainMenu::onSelection(void)
{
	Progress const & progress = Progress::getInstance();

	if (progress.isMenu() && getIndexCursor() == 5u && !progress.isGameFinished())
	{
		octo::StateManager &	states = octo::Application::getStateManager();

		setState(AMenu::State::Hide);
		AMenu * backMenu = getBackMenu();
		if (backMenu)
			backMenu->setState(AMenu::State::Active);
		if (progress.isFirstTime())
			states.change("zero");
		else
		{
			octo::AudioManager &		audio = octo::Application::getAudioManager();
			octo::ResourceManager &		resources = octo::Application::getResourceManager();
			if (!m_soundPlayed)
			{
				m_soundPlayed = true;
				audio.playSound(resources.getSound(OCTO_GREETING_OGG), 0.7f);
			}
			states.setTransitionDuration(sf::seconds(0.5f), sf::seconds(0.5f));
			states.change("transitionLevel");
		}
	}
	else if (progress.isMenu() && getIndexCursor() == 4u && progress.isMenu())
	{
		octo::StateManager &	states = octo::Application::getStateManager();
		states.change("menu");
		states.setTransitionDuration(sf::seconds(0.5f), sf::seconds(0.5f));
	}
	else
		AMenuSelection::onSelection();
}

void MainMenu::setup(void)
{
	AMenuSelection::setup();
	m_filter.setSize(octo::Application::getCamera().getSize() * 1.2f);
	m_filter.setFillColor(sf::Color(0, 0, 0, 50));
}

void MainMenu::update(sf::Time frameTime, sf::Vector2f const & octoBubblePosition)
{
	AMenuSelection::update(frameTime, octoBubblePosition);
	sf::FloatRect const & camera = octo::Application::getCamera().getRectangle();
	m_filter.setPosition(sf::Vector2f(camera.left, camera.top));
}

void MainMenu::draw(sf::RenderTarget & render, sf::RenderStates states) const
{
	render.draw(m_filter, states);
	AMenuSelection::draw(render, states);
}
