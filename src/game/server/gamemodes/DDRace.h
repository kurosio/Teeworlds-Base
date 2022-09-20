/* (c) Shereef Marzouk. See "licence DDRace.txt" and the readme.txt in the root of the distribution for more information. */
#ifndef GAME_SERVER_GAMEMODES_DDRACE_H
#define GAME_SERVER_GAMEMODES_DDRACE_H

#include <game/server/gamecontroller.h>
#include <game/server/teams.h>

#include <map>
#include <vector>

struct CScoreInitResult;
class CGameControllerDDRace : public IGameController
{
public:
	CGameControllerDDRace(class CGameContext *pGameServer);
	~CGameControllerDDRace();

	void OnCharacterSpawn(class CCharacter *pChr) override;

	void OnPlayerConnect(class CPlayer *pPlayer) override;
	void OnPlayerDisconnect(class CPlayer *pPlayer, const char *pReason) override;

	void Tick() override;

	void DoTeamChange(class CPlayer *pPlayer, int Team, bool DoChatMsg = true) override;

	int64_t GetMaskForPlayerWorldEvent(int Asker, int ExceptID = -1) override;

	void InitTeleporter();

	int GetPlayerTeam(int ClientID) const;

	CGameTeams m_Teams;

	std::map<int, std::vector<vec2>> m_TeleOuts;
	std::map<int, std::vector<vec2>> m_TeleCheckOuts;
};
#endif // GAME_SERVER_GAMEMODES_DDRACE_H
