#include "BigginPCH.h"
#include "StatsAndAchievements.h"
#include <isteamuser.h>
#include <isteamuserstats.h>
#include <isteamutils.h>
#include "ScoreComponent.h"
#include "Subject.h"
#include "steam_api.h"
#include "StringUtils.hpp"

// Defining our achievements
enum EAchievements
{
	ACH_WIN_ONE_GAME = 0,
	ACH_WIN_100_GAMES = 1,
	ACH_TRAVEL_FAR_ACCUM = 2,
	ACH_TRAVEL_FAR_SINGLE = 3,
	NEW_ACHIEVEMENT_0_4 = 4,
	_NumAchievements
};


CSteamAchievements::CSteamAchievements() :
    m_iAppID(0),
    m_bInitialized(false),
    m_CallbackUserStatsReceived(this, &CSteamAchievements::OnUserStatsReceived),
    m_CallbackUserStatsStored(this, &CSteamAchievements::OnUserStatsStored),
    m_CallbackAchievementStored(this, &CSteamAchievements::OnAchievementStored)
{

	// Achievement array which will hold data about the achievements and their state
	Achievement_t achievements[] =
	{
		_ACH_ID(ACH_WIN_ONE_GAME, "Winner"),
		_ACH_ID(ACH_WIN_100_GAMES, "Champion"),
		_ACH_ID(ACH_TRAVEL_FAR_ACCUM, "Interstellar"),
		_ACH_ID(ACH_TRAVEL_FAR_SINGLE, "Orbiter"),
		_ACH_ID(NEW_ACHIEVEMENT_0_4, "NEW_ACHIEVEMENT_0_4"), //Secret a achievement ;)
	};

    m_iAppID = SteamUtils()->GetAppID();
    m_pAchievements = new Achievement_t [_NumAchievements] (*achievements);
    m_iNumAchievements = _NumAchievements;

    RequestStats();
}

CSteamAchievements::~CSteamAchievements()
{
	delete m_pAchievements;
	m_pAchievements = nullptr;
}

bool CSteamAchievements::RequestStats()
{
	// Is Steam loaded? If not we can't get stats.
	if (nullptr == SteamUserStats() || nullptr == SteamUser())
	{
		return false;
	}
	// Is the user logged on?  If not we can't get stats.
	if (!SteamUser()->BLoggedOn())
	{
		return false;
	}
	// Request user stats.
	return SteamUserStats()->RequestCurrentStats();
}

bool CSteamAchievements::SetAchievement(const char* ID)
{
	// Have we received a call back from Steam yet?
	if (m_bInitialized)
	{
		SteamUserStats()->SetAchievement(ID);
		return SteamUserStats()->StoreStats();
	}
	// If not then we can't set achievements yet
	return false;
}

void CSteamAchievements::OnNotify(biggin::Component* entity, const std::string& event)
{
	if (event != "ScoreChanged") //early out as there's only one achievement at the moment
		return;

	const int& score = static_cast<const biggin::ScoreComponent*>(entity)->GetScore();
	if (score >= 500)
		SetAchievement("ACH_WIN_ONE_GAME");
}

void CSteamAchievements::OnUserStatsReceived(UserStatsReceived_t* pCallback)
{
	// we may get callbacks for other games' stats arriving, ignore them
	if (m_iAppID == static_cast<int64>(pCallback->m_nGameID))
	{
		if (k_EResultOK == pCallback->m_eResult)
		{
			OutputDebugString(L"Received stats and achievements from Steam\n");
			m_bInitialized = true;

			// load achievements
			for (int iAch = 0; iAch < m_iNumAchievements; ++iAch)
			{
				Achievement_t& ach = m_pAchievements[iAch];

				SteamUserStats()->GetAchievement(ach.m_pchAchievementID, &ach.m_bAchieved);
				_snprintf(ach.m_rgchName, sizeof(ach.m_rgchName), "%s",
					SteamUserStats()->GetAchievementDisplayAttribute(ach.m_pchAchievementID,
						"name"));
				_snprintf(ach.m_rgchDescription, sizeof(ach.m_rgchDescription), "%s",
					SteamUserStats()->GetAchievementDisplayAttribute(ach.m_pchAchievementID,
						"desc"));
			}
		}
		else
		{
			char buffer[128];
			_snprintf(buffer, 128, "RequestStats - failed, %d\n", pCallback->m_eResult);
			std::wstring result = converter.from_bytes(buffer);
			OutputDebugString(result.c_str());
		}
	}
}

void CSteamAchievements::OnUserStatsStored(UserStatsStored_t* pCallback)
{
	// we may get callbacks for other games' stats arriving, ignore them
	if (m_iAppID == static_cast<int64>(pCallback->m_nGameID))
	{
		if (k_EResultOK == pCallback->m_eResult)
		{
			OutputDebugString(L"Stored stats for Steam\n");
		}
		else
		{
			char buffer[128];
			_snprintf(buffer, 128, "StatsStored - failed, %d\n", pCallback->m_eResult);
			std::wstring result = converter.from_bytes(buffer);
			OutputDebugString(result.c_str());
		}
	}
}

void CSteamAchievements::OnAchievementStored(UserAchievementStored_t* pCallback)
{
	// we may get callbacks for other games' stats arriving, ignore them
	if (m_iAppID == static_cast<int64>(pCallback->m_nGameID))
	{
		OutputDebugString(L"Stored Achievement for Steam\n");
	}
}
