#include "Game/OutGamePlayerState.h"

AOutGamePlayerState::AOutGamePlayerState()
{
	PlayerInfo.PlayerName = GetPlayerName();
	PlayerInfo.PlayerTeam = E_TeamID::Neutral;
	PlayerInfo.PlayerTeamID = -1;
	PlayerInfo.SelectedCharacter = nullptr;
}
