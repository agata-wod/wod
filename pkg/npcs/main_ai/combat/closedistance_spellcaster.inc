var runaway := 0;

function CloseDistance (byref opponent)
	var d:=distance (me, opponent);
	if (d > 10)
		RunToward (opponent);
		return CLOSEDISTANCE_MOVED;
	endif

	case (d)
		1:		// the most likely, so first 
		0:
			if ( runaway < 15 )
				if (GetVital (me, "Mana") > CINT(GetVitalMaximumValue (me, "Mana")/3))
					RunAwayFrom (opponent);
					return CLOSEDISTANCE_FLEEING;
				endif
				runaway := runaway + 1;
				if (runaway == 15)
					turntoward (opponent);
				endif
			endif
			return CLOSEDISTANCE_DIDNT_MOVE;
		default:
			runaway := 0;
			if (GetVital (me, "Mana") < CINT(GetVitalMaximumValue (me, "Mana")/3))
				RunToward (opponent);
				return CLOSEDISTANCE_MOVED;
			elseif (!CheckLoSAt (me, opponent.x, opponent.y, opponent.z))
				if (RunRandom (opponent))
					return CLOSEDISTANCE_PEACEMADE;
				endif
			else
				SpellAttack (opponent);
			endif
			return CLOSEDISTANCE_DIDNT_MOVE;
	endcase

endfunction
