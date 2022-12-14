/* (c) Shereef Marzouk. See "licence DDRace.txt" and the readme.txt in the root of the distribution for more information. */
#include "gamecontext.h"
#include <engine/shared/config.h>
#include <engine/shared/protocol.h>
#include <game/server/gamemodes/DDRace.h>
#include <game/server/teams.h>
#include <game/version.h>

#include "entities/character.h"
#include "player.h"

bool CheckClientID(int ClientID);

void CGameContext::ConCredits(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *)pUserData;

	pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "chatresp",
		"DDNet is run by the DDNet staff (DDNet.tw/staff)");
	pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "chatresp",
		"Great maps and many ideas from the great community");
	pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "chatresp",
		"Help and code by eeeee, HMH, east, CookieMichal, Learath2,");
	pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "chatresp",
		"Savander, laxa, Tobii, BeaR, Wohoo, nuborn, timakro, Shiki,");
	pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "chatresp",
		"trml, Soreu, hi_leute_gll, Lady Saavik, Chairn, heinrich5991,");
	pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "chatresp",
		"swick, oy, necropotame, Ryozuki, Redix, d3fault, marcelherd,");
	pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "chatresp",
		"BannZay, ACTom, SiuFuWong, PathosEthosLogos, TsFreddie,");
	pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "chatresp",
		"Jupeyy, noby, ChillerDragon, ZombieToad, weez15, z6zzz,");
	pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "chatresp",
		"Piepow, QingGo, RafaelFF, sctt, jao, daverck, fokkonaut,");
	pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "chatresp",
		"Bojidar, FallenKN, ardadem, archimede67, sirius1242, Aerll,");
	pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "chatresp",
		"trafilaw, Zwelf, Patiga, Konsti, ElXreno, MikiGamer,");
	pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "chatresp",
		"Fireball, Banana090, axblk, yangfl, Kaffeine,");
	pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "chatresp",
		"Zodiac, c0d3d3v, GiuCcc, Ravie, Robyt3, simpygirl,");
	pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "chatresp",
		"sjrc6, Cellegen, srdante, Nouaa, Voxel & others.");
	pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "chatresp",
		"Based on DDRace by the DDRace developers,");
	pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "chatresp",
		"which is a mod of Teeworlds by the Teeworlds developers.");
}

void CGameContext::ConInfo(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "chatresp",
		"DDraceNetwork Mod. Version: " GAME_VERSION);
	if(GIT_SHORTREV_HASH)
	{
		char aBuf[64];
		str_format(aBuf, sizeof(aBuf), "Git revision hash: %s", GIT_SHORTREV_HASH);
		pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "chatresp", aBuf);
	}
	pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "chatresp",
		"Official site: DDNet.tw");
	pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "chatresp",
		"For more info: /cmdlist");
	pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "chatresp",
		"Or visit DDNet.tw");
}

void CGameContext::ConList(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	int ClientID = pResult->m_ClientID;
	if(!CheckClientID(ClientID))
		return;

	char zerochar = 0;
	if(pResult->NumArguments() > 0)
		pSelf->List(ClientID, pResult->GetString(0));
	else
		pSelf->List(ClientID, &zerochar);
}

void CGameContext::ConHelp(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *)pUserData;

	if(pResult->NumArguments() == 0)
	{
		pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "chatresp",
			"/cmdlist will show a list of all chat commands");
		pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "chatresp",
			"/help + any command will show you the help for this command");
		pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "chatresp",
			"Example /help settings will display the help about /settings");
	}
	else
	{
		const char *pArg = pResult->GetString(0);
		const IConsole::CCommandInfo *pCmdInfo =
			pSelf->Console()->GetCommandInfo(pArg, CFGFLAG_SERVER, false);
		if(pCmdInfo)
		{
			if(pCmdInfo->m_pParams)
			{
				char aBuf[256];
				str_format(aBuf, sizeof(aBuf), "Usage: %s %s", pCmdInfo->m_pName, pCmdInfo->m_pParams);
				pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "chatresp", aBuf);
			}

			if(pCmdInfo->m_pHelp)
				pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "chatresp", pCmdInfo->m_pHelp);
		}
		else
			pSelf->Console()->Print(
				IConsole::OUTPUT_LEVEL_STANDARD,
				"chatresp",
				"Command is either unknown or you have given a blank command without any parameters.");
	}
}

void CGameContext::ConSettings(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *)pUserData;

	if(pResult->NumArguments() == 0)
	{
		pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "chatresp",
			"to check a server setting say /settings and setting's name, setting names are:");
		pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "chatresp",
			"teams, cheats, collision, hooking, endlesshooking, me, ");
		pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "chatresp",
			"hitting, oldlaser, timeout, votes, pause and scores");
	}
	else
	{
		const char *pArg = pResult->GetString(0);
		char aBuf[256];
		float ColTemp;
		float HookTemp;
		pSelf->m_Tuning.Get("player_collision", &ColTemp);
		pSelf->m_Tuning.Get("player_hooking", &HookTemp);
		if(str_comp(pArg, "teams") == 0)
		{
			str_format(aBuf, sizeof(aBuf), "%s %s",
				g_Config.m_SvTeam == SV_TEAM_ALLOWED ?
					"Teams are available on this server" :
					(g_Config.m_SvTeam == SV_TEAM_FORBIDDEN || g_Config.m_SvTeam == SV_TEAM_FORCED_SOLO) ?
					"Teams are not available on this server" :
					"You have to be in a team to play on this server", /*g_Config.m_SvTeamStrict ? "and if you die in a team all of you die" : */
				"and all of your team will die if the team is locked");
			pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "chatresp", aBuf);
		}
		else if(str_comp(pArg, "cheats") == 0)
		{
			pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "chatresp",
				g_Config.m_SvTestingCommands ?
					"Cheats are enabled on this server" :
					"Cheats are disabled on this server");
		}
		else if(str_comp(pArg, "collision") == 0)
		{
			pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "chatresp",
				ColTemp ?
					"Players can collide on this server" :
					"Players can't collide on this server");
		}
		else if(str_comp(pArg, "hooking") == 0)
		{
			pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "chatresp",
				HookTemp ?
					"Players can hook each other on this server" :
					"Players can't hook each other on this server");
		}
		else if(str_comp(pArg, "endlesshooking") == 0)
		{
			pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "chatresp",
				g_Config.m_SvEndlessDrag ?
					"Players hook time is unlimited" :
					"Players hook time is limited");
		}
		else if(str_comp(pArg, "oldlaser") == 0)
		{
			pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "chatresp",
				g_Config.m_SvOldLaser ?
					"Lasers can hit you if you shot them and they pull you towards the bounce origin (Like DDRace Beta)" :
					"Lasers can't hit you if you shot them, and they pull others towards the shooter");
		}
		else if(str_comp(pArg, "me") == 0)
		{
			pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "chatresp",
				g_Config.m_SvSlashMe ?
					"Players can use /me commands the famous IRC Command" :
					"Players can't use the /me command");
		}
		else if(str_comp(pArg, "timeout") == 0)
		{
			str_format(aBuf, sizeof(aBuf), "The Server Timeout is currently set to %d seconds", g_Config.m_ConnTimeout);
			pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "chatresp", aBuf);
		}
		else if(str_comp(pArg, "votes") == 0)
		{
			pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "chatresp",
				g_Config.m_SvVoteKick ?
					"Players can use Callvote menu tab to kick offenders" :
					"Players can't use the Callvote menu tab to kick offenders");
			if(g_Config.m_SvVoteKick)
			{
				str_format(aBuf, sizeof(aBuf),
					"Players are banned for %d minute(s) if they get voted off", g_Config.m_SvVoteKickBantime);

				pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "chatresp",
					g_Config.m_SvVoteKickBantime ?
						aBuf :
						"Players are just kicked and not banned if they get voted off");
			}
		}
		else if(str_comp(pArg, "pause") == 0)
		{
			pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "chatresp",
				g_Config.m_SvPauseable ?
					"/spec will pause you and your tee will vanish" :
					"/spec will pause you but your tee will not vanish");
		}
		else if(str_comp(pArg, "scores") == 0)
		{
			pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "chatresp",
				g_Config.m_SvHideScore ?
					"Scores are private on this server" :
					"Scores are public on this server");
		}
		else
		{
			pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "chatresp",
				"no matching settings found, type /settings to view them");
		}
	}
}

void CGameContext::ConRules(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	bool Printed = false;
	if(g_Config.m_SvDDRaceRules)
	{
		pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "chatresp",
			"Be nice.");
		Printed = true;
	}
#define GET_SERVER_RULE_LINE(n) g_Config.m_SvRulesLine##n
	char *apRuleLines[] = {
		GET_SERVER_RULE_LINE(1),
		GET_SERVER_RULE_LINE(2),
		GET_SERVER_RULE_LINE(3),
		GET_SERVER_RULE_LINE(4),
		GET_SERVER_RULE_LINE(5),
		GET_SERVER_RULE_LINE(6),
		GET_SERVER_RULE_LINE(7),
		GET_SERVER_RULE_LINE(8),
		GET_SERVER_RULE_LINE(9),
		GET_SERVER_RULE_LINE(10),
	};
	for(auto &pRuleLine : apRuleLines)
	{
		if(pRuleLine[0])
		{
			pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD,
				"chatresp", pRuleLine);
			Printed = true;
		}
	}
	if(!Printed)
	{
		pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "chatresp",
			"No Rules Defined, Kill em all!!");
	}
}

void ToggleSpecPause(IConsole::IResult *pResult, void *pUserData, int PauseType)
{
	if(!CheckClientID(pResult->m_ClientID))
		return;

	CGameContext *pSelf = (CGameContext *)pUserData;
	IServer *pServ = pSelf->Server();
	CPlayer *pPlayer = pSelf->m_apPlayers[pResult->m_ClientID];
	if(!pPlayer)
		return;

	int PauseState = pPlayer->IsPaused();
	if(PauseState > 0)
	{
		char aBuf[128];
		str_format(aBuf, sizeof(aBuf), "You are force-paused for %d seconds.", (PauseState - pServ->Tick()) / pServ->TickSpeed());
		pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "chatresp", aBuf);
	}
	else if(pResult->NumArguments() > 0)
	{
		if(-PauseState == PauseType && pPlayer->m_SpectatorID != pResult->m_ClientID && pServ->ClientIngame(pPlayer->m_SpectatorID) && !str_comp(pServ->ClientName(pPlayer->m_SpectatorID), pResult->GetString(0)))
		{
			pPlayer->Pause(CPlayer::PAUSE_NONE, false);
		}
		else
		{
			pPlayer->Pause(PauseType, false);
			pPlayer->SpectatePlayerName(pResult->GetString(0));
		}
	}
	else if(-PauseState != CPlayer::PAUSE_NONE && PauseType != CPlayer::PAUSE_NONE)
	{
		pPlayer->Pause(CPlayer::PAUSE_NONE, false);
	}
	else if(-PauseState != PauseType)
	{
		pPlayer->Pause(PauseType, false);
	}
}

void ToggleSpecPauseVoted(IConsole::IResult *pResult, void *pUserData, int PauseType)
{
	if(!CheckClientID(pResult->m_ClientID))
		return;

	CGameContext *pSelf = (CGameContext *)pUserData;
	CPlayer *pPlayer = pSelf->m_apPlayers[pResult->m_ClientID];
	if(!pPlayer)
		return;

	int PauseState = pPlayer->IsPaused();
	if(PauseState > 0)
	{
		IServer *pServ = pSelf->Server();
		char aBuf[128];
		str_format(aBuf, sizeof(aBuf), "You are force-paused for %d seconds.", (PauseState - pServ->Tick()) / pServ->TickSpeed());
		pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "chatresp", aBuf);
		return;
	}

	bool IsPlayerBeingVoted = pSelf->m_VoteCloseTime &&
				  (pSelf->IsKickVote() || pSelf->IsSpecVote()) &&
				  pResult->m_ClientID != pSelf->m_VoteVictim;
	if((!IsPlayerBeingVoted && -PauseState == PauseType) ||
		(IsPlayerBeingVoted && PauseState && pPlayer->m_SpectatorID == pSelf->m_VoteVictim))
	{
		pPlayer->Pause(CPlayer::PAUSE_NONE, false);
	}
	else
	{
		pPlayer->Pause(PauseType, false);
		if(IsPlayerBeingVoted)
			pPlayer->m_SpectatorID = pSelf->m_VoteVictim;
	}
}

void CGameContext::ConToggleSpec(IConsole::IResult *pResult, void *pUserData)
{
	ToggleSpecPause(pResult, pUserData, g_Config.m_SvPauseable ? CPlayer::PAUSE_SPEC : CPlayer::PAUSE_PAUSED);
}

void CGameContext::ConToggleSpecVoted(IConsole::IResult *pResult, void *pUserData)
{
	ToggleSpecPauseVoted(pResult, pUserData, g_Config.m_SvPauseable ? CPlayer::PAUSE_SPEC : CPlayer::PAUSE_PAUSED);
}

void CGameContext::ConTogglePause(IConsole::IResult *pResult, void *pUserData)
{
	ToggleSpecPause(pResult, pUserData, CPlayer::PAUSE_PAUSED);
}

void CGameContext::ConTogglePauseVoted(IConsole::IResult *pResult, void *pUserData)
{
	ToggleSpecPauseVoted(pResult, pUserData, CPlayer::PAUSE_PAUSED);
}

void CGameContext::ConDND(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	if(!CheckClientID(pResult->m_ClientID))
		return;

	CPlayer *pPlayer = pSelf->m_apPlayers[pResult->m_ClientID];
	if(!pPlayer)
		return;

	if(pPlayer->m_DND)
	{
		pPlayer->m_DND = false;
		pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "chatresp", "You will receive global chat and server messages");
	}
	else
	{
		pPlayer->m_DND = true;
		pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "chatresp", "You will not receive any further global chat and server messages");
	}
}

void CGameContext::ConTimeout(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	if(!CheckClientID(pResult->m_ClientID))
		return;

	CPlayer *pPlayer = pSelf->m_apPlayers[pResult->m_ClientID];
	if(!pPlayer)
		return;

	const char *pTimeout = pResult->NumArguments() > 0 ? pResult->GetString(0) : pPlayer->m_aTimeoutCode;

	if(!pSelf->Server()->IsSixup(pResult->m_ClientID))
	{
		for(int i = 0; i < pSelf->Server()->MaxClients(); i++)
		{
			if(i == pResult->m_ClientID)
				continue;
			if(!pSelf->m_apPlayers[i])
				continue;
			if(str_comp(pSelf->m_apPlayers[i]->m_aTimeoutCode, pTimeout))
				continue;
			if(pSelf->Server()->SetTimedOut(i, pResult->m_ClientID))
			{
				if(pSelf->m_apPlayers[i]->GetCharacter())
					pSelf->SendTuningParams(i, pSelf->m_apPlayers[i]->GetCharacter()->m_TuneZone);
				return;
			}
		}
	}
	else
	{
		pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "chatresp",
			"Your timeout code has been set. 0.7 clients can not reclaim their tees on timeout; however, a 0.6 client can claim your tee ");
	}

	pSelf->Server()->SetTimeoutProtected(pResult->m_ClientID);
	str_copy(pPlayer->m_aTimeoutCode, pResult->GetString(0), sizeof(pPlayer->m_aTimeoutCode));
}

void CGameContext::ConPractice(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	if(!CheckClientID(pResult->m_ClientID))
		return;

	CPlayer *pPlayer = pSelf->m_apPlayers[pResult->m_ClientID];
	if(!pPlayer)
		return;

	if(pSelf->ProcessSpamProtection(pResult->m_ClientID, false))
		return;

	if(!g_Config.m_SvPractice)
	{
		pSelf->Console()->Print(
			IConsole::OUTPUT_LEVEL_STANDARD,
			"chatresp",
			"Practice mode is disabled");
		return;
	}

	CGameTeams &Teams = ((CGameControllerDDRace *)pSelf->m_pController)->m_Teams;

	int Team = Teams.m_Core.Team(pResult->m_ClientID);

	if(Team < TEAM_FLOCK || (Team == TEAM_FLOCK && g_Config.m_SvTeam != SV_TEAM_FORCED_SOLO) || Team >= TEAM_SUPER)
	{
		pSelf->Console()->Print(
			IConsole::OUTPUT_LEVEL_STANDARD,
			"chatresp",
			"Join a team to enable practice mode, which means you can use /r, but can't earn a rank.");
		return;
	}

	if(Teams.IsPractice(Team))
	{
		pSelf->Console()->Print(
			IConsole::OUTPUT_LEVEL_STANDARD,
			"chatresp",
			"Team is already in practice mode");
		return;
	}

	bool VotedForPractice = pResult->NumArguments() == 0 || pResult->GetInteger(0);

	if(VotedForPractice == pPlayer->m_VotedForPractice)
		return;

	pPlayer->m_VotedForPractice = VotedForPractice;

	int NumCurrentVotes = 0;
	int TeamSize = 0;

	for(int i = 0; i < MAX_CLIENTS; i++)
	{
		if(Teams.m_Core.Team(i) == Team)
		{
			CPlayer *pPlayer2 = pSelf->m_apPlayers[i];
			if(pPlayer2 && pPlayer2->m_VotedForPractice)
				NumCurrentVotes++;
			TeamSize++;
		}
	}

	int NumRequiredVotes = TeamSize / 2 + 1;

	char aBuf[512];
	str_format(aBuf, sizeof(aBuf), "'%s' voted to %s /practice mode for your team, which means you can use /r, but you can't earn a rank. Type /practice to vote (%d/%d required votes)", pSelf->Server()->ClientName(pResult->m_ClientID), VotedForPractice ? "enable" : "disable", NumCurrentVotes, NumRequiredVotes);
	pSelf->SendChatTeam(Team, aBuf);

	if(NumCurrentVotes >= NumRequiredVotes)
	{
		Teams.SetPractice(Team, true);
		pSelf->SendChatTeam(Team, "Practice mode enabled for your team, happy practicing!");
	}
}

void CGameContext::ConMe(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	if(!CheckClientID(pResult->m_ClientID))
		return;

	char aBuf[256 + 24];

	str_format(aBuf, 256 + 24, "'%s' %s",
		pSelf->Server()->ClientName(pResult->m_ClientID),
		pResult->GetString(0));
	if(g_Config.m_SvSlashMe)
		pSelf->SendChat(-2, CGameContext::CHAT_ALL, aBuf, pResult->m_ClientID);
	else
		pSelf->Console()->Print(
			IConsole::OUTPUT_LEVEL_STANDARD,
			"chatresp",
			"/me is disabled on this server");
}

void CGameContext::ConConverse(IConsole::IResult *pResult, void *pUserData)
{
	// This will never be called
}

void CGameContext::ConWhisper(IConsole::IResult *pResult, void *pUserData)
{
	// This will never be called
}

void CGameContext::ConSetEyeEmote(IConsole::IResult *pResult,
	void *pUserData)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	if(!CheckClientID(pResult->m_ClientID))
		return;

	CPlayer *pPlayer = pSelf->m_apPlayers[pResult->m_ClientID];
	if(!pPlayer)
		return;
	if(pResult->NumArguments() == 0)
	{
		pSelf->Console()->Print(
			IConsole::OUTPUT_LEVEL_STANDARD,
			"chatresp",
			(pPlayer->m_EyeEmoteEnabled) ?
				"You can now use the preset eye emotes." :
				"You don't have any eye emotes, remember to bind some. (until you die)");
		return;
	}
	else if(str_comp_nocase(pResult->GetString(0), "on") == 0)
		pPlayer->m_EyeEmoteEnabled = true;
	else if(str_comp_nocase(pResult->GetString(0), "off") == 0)
		pPlayer->m_EyeEmoteEnabled = false;
	else if(str_comp_nocase(pResult->GetString(0), "toggle") == 0)
		pPlayer->m_EyeEmoteEnabled = !pPlayer->m_EyeEmoteEnabled;
	pSelf->Console()->Print(
		IConsole::OUTPUT_LEVEL_STANDARD,
		"chatresp",
		(pPlayer->m_EyeEmoteEnabled) ?
			"You can now use the preset eye emotes." :
			"You don't have any eye emotes, remember to bind some. (until you die)");
}

void CGameContext::ConEyeEmote(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	if(g_Config.m_SvEmotionalTees == -1)
	{
		pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "chatresp",
			"Emotes are disabled.");
		return;
	}

	if(!CheckClientID(pResult->m_ClientID))
		return;

	CPlayer *pPlayer = pSelf->m_apPlayers[pResult->m_ClientID];
	if(!pPlayer)
		return;

	if(pResult->NumArguments() == 0)
	{
		pSelf->Console()->Print(
			IConsole::OUTPUT_LEVEL_STANDARD,
			"chatresp",
			"Emote commands are: /emote surprise /emote blink /emote close /emote angry /emote happy /emote pain /emote normal");
		pSelf->Console()->Print(
			IConsole::OUTPUT_LEVEL_STANDARD,
			"chatresp",
			"Example: /emote surprise 10 for 10 seconds or /emote surprise (default 1 second)");
	}
	else
	{
		if(!pPlayer->CanOverrideDefaultEmote())
			return;

		int EmoteType = 0;
		if(!str_comp(pResult->GetString(0), "angry"))
			EmoteType = EMOTE_ANGRY;
		else if(!str_comp(pResult->GetString(0), "blink"))
			EmoteType = EMOTE_BLINK;
		else if(!str_comp(pResult->GetString(0), "close"))
			EmoteType = EMOTE_BLINK;
		else if(!str_comp(pResult->GetString(0), "happy"))
			EmoteType = EMOTE_HAPPY;
		else if(!str_comp(pResult->GetString(0), "pain"))
			EmoteType = EMOTE_PAIN;
		else if(!str_comp(pResult->GetString(0), "surprise"))
			EmoteType = EMOTE_SURPRISE;
		else if(!str_comp(pResult->GetString(0), "normal"))
			EmoteType = EMOTE_NORMAL;
		else
		{
			pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD,
				"chatresp", "Unknown emote... Say /emote");
			return;
		}

		int Duration = 1;
		if(pResult->NumArguments() > 1)
			Duration = clamp(pResult->GetInteger(1), 1, 86400);

		pPlayer->OverrideDefaultEmote(EmoteType, pSelf->Server()->Tick() + Duration * pSelf->Server()->TickSpeed());
	}
}

void CGameContext::ConNinjaJetpack(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	if(!CheckClientID(pResult->m_ClientID))
		return;

	CPlayer *pPlayer = pSelf->m_apPlayers[pResult->m_ClientID];
	if(!pPlayer)
		return;
	if(pResult->NumArguments())
		pPlayer->m_NinjaJetpack = pResult->GetInteger(0);
	else
		pPlayer->m_NinjaJetpack = !pPlayer->m_NinjaJetpack;
}

void CGameContext::ConShowOthers(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	if(!CheckClientID(pResult->m_ClientID))
		return;

	CPlayer *pPlayer = pSelf->m_apPlayers[pResult->m_ClientID];
	if(!pPlayer)
		return;
	if(g_Config.m_SvShowOthers)
	{
		if(pResult->NumArguments())
			pPlayer->m_ShowOthers = pResult->GetInteger(0);
		else
			pPlayer->m_ShowOthers = !pPlayer->m_ShowOthers;
	}
	else
		pSelf->Console()->Print(
			IConsole::OUTPUT_LEVEL_STANDARD,
			"chatresp",
			"Showing players from other teams is disabled");
}

void CGameContext::ConShowAll(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	if(!CheckClientID(pResult->m_ClientID))
		return;

	CPlayer *pPlayer = pSelf->m_apPlayers[pResult->m_ClientID];
	if(!pPlayer)
		return;

	if(pResult->NumArguments())
	{
		if(pPlayer->m_ShowAll == (bool)pResult->GetInteger(0))
			return;

		pPlayer->m_ShowAll = pResult->GetInteger(0);
	}
	else
	{
		pPlayer->m_ShowAll = !pPlayer->m_ShowAll;
	}

	if(pPlayer->m_ShowAll)
		pSelf->SendChatTarget(pResult->m_ClientID, "You will now see all tees on this server, no matter the distance");
	else
		pSelf->SendChatTarget(pResult->m_ClientID, "You will no longer see all tees on this server");
}

void CGameContext::ConSpecTeam(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	if(!CheckClientID(pResult->m_ClientID))
		return;

	CPlayer *pPlayer = pSelf->m_apPlayers[pResult->m_ClientID];
	if(!pPlayer)
		return;

	if(pResult->NumArguments())
		pPlayer->m_SpecTeam = pResult->GetInteger(0);
	else
		pPlayer->m_SpecTeam = !pPlayer->m_SpecTeam;
}

bool CheckClientID(int ClientID)
{
	return ClientID >= 0 && ClientID < MAX_CLIENTS;
}

void CGameContext::ConTele(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	if(!CheckClientID(pResult->m_ClientID))
		return;
	CPlayer *pPlayer = pSelf->m_apPlayers[pResult->m_ClientID];
	if(!pPlayer)
		return;
	CCharacter *pChr = pPlayer->GetCharacter();
	if(!pChr)
		return;

	CGameTeams &Teams = ((CGameControllerDDRace *)pSelf->m_pController)->m_Teams;
	int Team = Teams.m_Core.Team(pResult->m_ClientID);
	if(!Teams.IsPractice(Team))
	{
		pSelf->SendChatTarget(pPlayer->GetCID(), "You're not in a team with /practice turned on. Note that you can't earn a rank with practice enabled.");
		return;
	}

	vec2 Pos = pPlayer->m_ViewPos;

	if(pResult->NumArguments() > 0)
	{
		int ClientID;
		for(ClientID = 0; ClientID < MAX_CLIENTS; ClientID++)
		{
			if(str_comp(pResult->GetString(0), pSelf->Server()->ClientName(ClientID)) == 0)
				break;
		}
		if(ClientID == MAX_CLIENTS)
		{
			pSelf->SendChatTarget(pPlayer->GetCID(), "No player with this name found.");
			return;
		}
		CPlayer *pPlayerTo = pSelf->m_apPlayers[ClientID];
		if(!pPlayerTo)
			return;
		CCharacter *pChrTo = pPlayerTo->GetCharacter();
		if(!pChrTo)
			return;
		Pos = pChrTo->m_Pos;
	}

	pSelf->Teleport(pChr, Pos);
	pChr->UnFreeze();
	pChr->Core()->m_Vel = vec2(0, 0);
}