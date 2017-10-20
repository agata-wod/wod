///////////////////
//  looks around the area for other NPCs that are 'friendly' with the NPC being attacked, and asks
//  them to attack
///////////////////

function GetHelpFromAllies (byref opponent)
	var ev := struct;
	ev.+type := EVID_ALL_ATTACK_CMD;
	ev.+source := opponent;
	if (opponent.npctemplate and !GetObjProperty (opponent, "attackable"))
		return 1;
	endif

	var found_friend := 0;
	foreach mobile in ListMobilesNearLocation ( me.x, me.y, me.z, HALT_THRESHOLD, me.realm )
		if (mobile.npctemplate["praetorian"] and mobile != me)
			if (distance (mobile, opponent) < 16 )
				found_friend := 1;
				if (!mobile.warmode)
					SendEvent (mobile, ev);
				endif
			endif
		endif
	endforeach
	foreach mobile in ListMobilesNearLocation ( opponent.x, opponent.y, opponent.z, HALT_THRESHOLD, opponent.realm )
		if (mobile.npctemplate["praetorian"] and mobile != me)
			found_friend := 1;
			if (!mobile.warmode)
				SendEvent (mobile, ev);
			endif
		endif
	endforeach

	if (found_friend)
		return 1;
	endif
	if (me.graphic != 400 and me.graphic != 401)
		return 1;
	endif

	found_friend := 0;
	foreach mobile in ListMobilesNearLocation ( me.x, me.y, me.z, (HALT_THRESHOLD*2), me.realm )
		if (mobile.npctemplate["praetorian"] and !mobile.warmode and mobile != me)
			if (distance (mobile, opponent) > distance (me, opponent) )
				found_friend := 1;
				RunToward (mobile);
				case (RandomInt (4))
					0:	Say ("We're under attack!");
					1:	Say ("To arms!  To arms!");
					2:	Say ("Rally to me!");
					3:	Say ("Beware, invaders!");
				endcase
				var runs := 0;
				repeat
					RunToward (mobile);
					runs := runs + 1;
				until (Distance (me, mobile) < 4 or runs > 10);
			endif
		endif
	endforeach

	if (!found_friend)
		return 0;
	endif

	foreach mobile in ListMobilesNearLocation ( me.x, me.y, me.z, HALT_THRESHOLD, me.realm )
		if (mobile.npctemplate["praetorian"] and !mobile.warmode and mobile != me)
			SendEvent (mobile, ev);
		endif
	endforeach
	return 1;
endfunction




///////////////////
//  determines when and how to change opponents
///////////////////

function ChangeOpponent (byref old_opponent, byref new_opponent)
	if (!IsGood (new_opponent) )
		return 0;
	endif
	if (GetObjProperty (new_opponent, "master") and !GetObjProperty (old_opponent, "master"))
		if (RandomInt (4))
			return 0;
		endif
	endif
	if (Distance (me, new_opponent) >= Distance (me, old_opponent) )
		if (RandomInt (8) )
			return 0;
		endif
	endif

	var myrank := GetObjProperty (me, "rank");
	if (!myrank)
		return 1;
	endif

	//check to see if there's anyone around who's a higher rank than me
	var mobile_count := 0;
	foreach mobile in ListMobilesNearLocation (me.x, me.y, me.z, HALT_THRESHOLD, me.realm)
		if (mobile.npctemplate ["praetorian"] and !mobile.script["tamed"] and mobile != me)
			mobile_count := mobile_count + 1;
			var rank := GetObjProperty (mobile, "rank");
			if (rank and rank > myrank)
				if (RandomInt (2) )
					var ev := struct;
					ev.+type := EVID_DAMAGED;
					ev.+source := new_opponent;
				endif
				return 0;
			elseif (rank == myrank)
				if (RandomInt (2) )
					return 0;
				endif
			endif
		endif
	endforeach

	//no one else around to boss around
	if (!mobile_count)
		return 1;
	endif

	//guess not, so I'm in charge, so issue the attack order
	var ev := struct;
	ev.+type := EVID_ALL_ATTACK_CMD;
	ev.+source := new_opponent;
	foreach mobile in ListMobilesNearLocation (me.x, me.y, me.z, HALT_THRESHOLD, me.realm)
		if (mobile.npctemplate ["praetorian"] and mobile != me)
			SendEvent (mobile, ev);
		endif
	endforeach

	case (RandomInt (2) )
		0:	Say ("Praetorians, attack!");
		1:	Say ("Get him!");
	endcase

	return 1;
endfunction
