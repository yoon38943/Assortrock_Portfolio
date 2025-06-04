#include "Game/OutGamePlayerState.h"

AOutGamePlayerState::AOutGamePlayerState()
{
	PlayerInfo.PlayerName = GetPlayerName();
	PlayerInfo.PlayerTeam = E_TeamID::Neutral;
	PlayerInfo.PlayerTeamID = NULL;
	PlayerInfo.SelectedCharacter = nullptr;
}
