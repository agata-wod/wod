
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

	if (!IsGood (new_opponent) and RandomInt (10))
		if (Distance (me, old_opponent) <= 8)
			return 0;
		endif
	endif

	if (!IsGood (old_opponent) and RandomInt (8))
		if (Distance (me, new_opponent) <= 8)
			SetObjProperty (me, "#lastopponentchange", ReadGameClock());
			return 1;
		endif
	endif

	var new_opponent_distance := Distance (me, new_opponent);
	var old_opponent_distance := Distance (me, old_opponent);

	if (new_opponent_distance <= old_opponent_distance)
		if (old_opponent_distance > 7)
			SetObjProperty (me, "#lastopponentchange", ReadGameClock());
			return 1;
		elseif (old_opponent_distance < 3)
			if (GetVital (old_opponent, "Life") < 2000 and RandomInt (10))
				return 0;
			endif
		elseif (new_opponent_distance < 3)
			if (GetVital (new_opponent, "Life") and RandomInt (10))
				SetObjProperty (me, "#lastopponentchange", ReadGameClock());
				return 1;
			endif
		elseif (old_opponent_distance == 1 and new_opponent_distance == 1)
			if (RandomInt (10))
				return 0;
			endif
			SetObjProperty (me, "#lastopponentchange", ReadGameClock());
			return 1;
		elseif (RandomInt (8))
			SetObjProperty (me, "#lastopponentchange", ReadGameClock());
			return 1;
		endif
		return 0;
	endif

	//if we got here, the new opponent is further away than the old one
	if (new_opponent_distance > 20)
		return 0;
	elseif (new_opponent_distance > old_opponent_distance + 6)
		if (RandomInt (8))
			return 0;
		endif
		SetObjProperty (me, "#lastopponentchange", ReadGameClock());
		return 1;
	else
		if (RandomInt (6))
			return 0;
		endif
		SetObjProperty (me, "#lastopponentchange", ReadGameClock());
		return 1;
	endif
	SetObjProperty (me, "#lastopponentchange", ReadGameClock());
	return 1;
endfunction
