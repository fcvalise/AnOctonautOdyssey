#ifndef STEAM_API_HPP
# define STEAM_API_HPP

#include "steam_api.h"
#include "Progress.hpp"

# define _ACH_ID( id, name ) { id, #id, name, "", 0, 0 }
struct Achievement_t
{
	int			m_eAchievementID;
	const char	*m_pchAchievementID;
	char		m_rgchName[128];
	char		m_rgchDescription[256];
	bool		m_bAchieved;
	int			m_iIconImage;
};

class SteamAPI
{
	// Defining our achievements
	enum EAchievements
	{
		ACH_RANDOM_PORTAL,
		ACH_NPC,
		ACH_JUMP,
		ACH_DEATH,
		ACH_NANOROBOT,
		ACH_SPIRIT,
		ACH_BLUE_END,
		ACH_RED_END,
		ACH_SPACESHIP,
		COUNT_ACH
	};

	// Achievement array which will hold data about the achievements and their state
	Achievement_t g_Achievements[9] =
	{
		_ACH_ID( ACH_RANDOM_PORTAL, "ACH_RANDOM_PORTAL" ),
		_ACH_ID( ACH_NPC, "ACH_NPC" ),
		_ACH_ID( ACH_JUMP, "ACH_JUMP" ),
		_ACH_ID( ACH_DEATH, "ACH_DEATH" ),
		_ACH_ID( ACH_NANOROBOT, "ACH_NANOROBOT" ),
		_ACH_ID( ACH_SPIRIT, "ACH_SPIRIT" ),
		_ACH_ID( ACH_BLUE_END, "ACH_BLUE_END" ),
		_ACH_ID( ACH_RED_END, "ACH_RED_END" )
		_ACH_ID( ACH_SPACESHIP, "ACH_SPACESHIP" )
	};

public:
	SteamAPI(void);
	virtual ~SteamAPI(void);

	void update(Progress::data & data);
	void reset(void);
	void unlockAchievements(Progress::data & data);
	void updateStats(Progress::data & data);
	bool setAchievement(EAchievements achievement);

	bool requestStats();

private:
	bool	m_initRet;
	int64	m_iAppID; // Our current AppID
	int		m_iNumAchievements; // The number of Achievements
	bool	m_bInitialized; // Have we called Request stats and received the callback?

public:
	STEAM_CALLBACK( SteamAPI, OnUserStatsReceived, UserStatsReceived_t, 
		m_CallbackUserStatsReceived );
	STEAM_CALLBACK( SteamAPI, OnUserStatsStored, UserStatsStored_t, 
		m_CallbackUserStatsStored );
	STEAM_CALLBACK( SteamAPI, OnAchievementStored, 
		UserAchievementStored_t, m_CallbackAchievementStored );
};

#endif
