#pragma once
#define _ACH_ID( id, name ) { id, #id, name, "", 0, 0 }
#include <steamtypes.h>
#include <isteamuser.h>
#include <isteamuserstats.h>
#include "Observer.h"
#include "Singleton.h"

namespace biggin
{
	class Subject;
}

struct UserStatsReceived_t;
struct UserStatsStored_t;
struct UserAchievementStored_t;

//https://partner.steamgames.com/doc/features/achievements/ach_guide
struct Achievement_t
{
	int m_eAchievementID;
	const char* m_pchAchievementID;
	char m_rgchName[128];
	char m_rgchDescription[256];
	bool m_bAchieved;
	int m_iIconImage;
};

class CSteamAchievements final : public biggin::Observer, public biggin::Singleton<CSteamAchievements>
{
private:
	int64 m_iAppID; // Our current AppID
	Achievement_t* m_pAchievements; // Achievements data
	int m_iNumAchievements; // The number of Achievements
	bool m_bInitialized; // Have we called Request stats and received the callback?

public:
	CSteamAchievements();
	~CSteamAchievements() override;

	bool RequestStats();
	bool SetAchievement(const char* ID);

	void AddObserver(biggin::Subject* subject);
	void OnNotify(biggin::Component* entity, const std::string& event) override;

	//https://partner.steamgames.com/doc/sdk/api#callbacks
	STEAM_CALLBACK(CSteamAchievements, OnUserStatsReceived, UserStatsReceived_t,
		m_CallbackUserStatsReceived);
	STEAM_CALLBACK(CSteamAchievements, OnUserStatsStored, UserStatsStored_t,
		m_CallbackUserStatsStored);
	STEAM_CALLBACK(CSteamAchievements, OnAchievementStored,
		UserAchievementStored_t, m_CallbackAchievementStored);
};

