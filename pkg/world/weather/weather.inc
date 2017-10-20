Const SFX_THUNDER := 0x29;

function SetWeatherForPlayer (player)
	//check if they're in a dungeon.  If so, turn the weather off
	if (player.x >= 5120)
		MoveObjectToLocation( player, player.x, player.y, player.z, player.realm, MOVEOBJECT_FORCELOCATION );
		return;
	endif

	var weather := GetGlobalProperty ("#weather");
	if (!weather)
		return;
	endif
	
	//clear the current weather first
	SendPacket (player, weather);
endfunction

