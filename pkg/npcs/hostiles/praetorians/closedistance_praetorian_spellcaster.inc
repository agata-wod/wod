var runaway := 0;

function CloseDistance (byref opponent)
	var d:=distance (me, opponent);
	if (d > 10)
		RunToward (opponent);
		return 0;
	endif

	var found_praetorian := 0;
	var nearest_praetorian := 0;
	var old_distance := 100;
	foreach mobile in ListMobilesNearLocation (me.x, me.y, me.z, HALT_THRESHOLD, me.realm)
		var masterserial := GetObjProperty (mobile, "master");
		if (mobile.npctemplate["praetorian"] or (masterserial and masterserial == me.serial))
			found_praetorian := 1;
			var new_distance := Distance (me, opponent);
			if (new_distance < old_distance)
				nearest_praetorian := mobile;
				old_distance := new_distance;
			endif
		endif
	endforeach

	if (!found_praetorian)
		return DefaultCloseDistance (opponent);
	endif

	if (distance (me, nearest_praetorian) > 4)
		RunToward (nearest_praetorian);
		return 0;
	endif

	case (d)
		1:		// the most likely, so first 
		0:
			if ( runaway < 15 )
				if (GetVital (me, "Mana") > CINT (GetVitalMaximumValue (me, "Mana")/4))
					RunAwayFrom (opponent);
					return 3;
				endif
				runaway := runaway + 1;
				if (runaway == 15)
					turntoward (opponent);
				endif
			endif
			return 1;
		default:
			runaway := 0;
			if (GetVital (me, "Mana") < CINT (GetVitalMaximumValue (me, "Mana")/4))
				RunToward (opponent);
				return 0;
			elseif (!CheckLoSAt (me, opponent.x, opponent.y, opponent.z) )
				if (RunRandom (opponent))
					return 2;
				endif
			else
				SpellAttack (opponent);
			endif
			return 1;
	endcase


endfunction

function DefaultCloseDistance (byref opponent)

	var d:=distance(me,opponent);
	if ( d > 10 )
		RunToward( opponent );
		return 0;
	endif

	case (d)
		1:		// the most likely, so first 
		0:
			if ( runaway < 15 )
				if (GetVital (me, "Mana") > CINT (GetVitalMaximumValue (me, "Mana")/3))
					RunAwayFrom ( opponent );
					return 3;
				endif
				runaway := runaway + 1;
				if (runaway == 15)
					turntoward ( opponent );
				endif
			endif
			return 1;
		default:
			runaway := 0;
			if (GetVital (me, "Mana") < CINT (GetVitalMaximumValue (me, "Mana")/3))
				RunToward ( opponent );
				return 0;
			elseif (!CheckLoSAt (me, opponent.x, opponent.y, opponent.z) )
				if (runrandom(opponent))
					return 2;
				endif
			else
				spellattack(opponent);
			endif
			return 1;
	endcase

endfunction
