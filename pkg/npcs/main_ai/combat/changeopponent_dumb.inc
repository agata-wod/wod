
///////////////////
//  determines when and how to change opponents
///////////////////

function ChangeOpponent (byref old_opponent, byref new_opponent)
	if (GetObjProperty (me, "#lastopponentchange"))
		var lastopponentchange := GetObjProperty (me, "#lastopponentchange");
		if (lastopponentchange + 8 > ReadGameClock())
			return 0;
		endif
	endif
	
	if (!CheckLoSAt (me, old_opponent.x, old_opponent.y, old_opponent.z))
		SetObjProperty (me, "#lastopponentchange", ReadGameClock());
		return 1;
	endif

	var new_opponent_distance := Distance (me, new_opponent);
	var old_opponent_distance := Distance (me, old_opponent);
	
	if (new_opponent_distance < old_opponent_distance)
		SetObjProperty (me, "#lastopponentchange", ReadGameClock());
		return 1;
	elseif (RandomInt (8))
		SetObjProperty (me, "#lastopponentchange", ReadGameClock());
		return 1;
	endif
endfunction
