var time_pathfinding := 0;


function CloseDistance ( byref opponent)
	var d := Distance (me, opponent);
	if (d == 1 or d == 0)
		return CLOSEDISTANCE_DIDNT_MOVE;
	elseif (d > 25 or opponent.hidden or opponent.concealed)
		return CLOSEDISTANCE_PEACEMADE;
	endif

	RunToward (opponent);
	sleepms (5);
	RunToward (opponent);
	sleepms (5);
	RunToward (opponent);

	case (Distance (me, opponent))
		1:	// the most likely, so first 
		0:
			return CLOSEDISTANCE_DIDNT_MOVE;
		default:
			RunToward (opponent);
			return CLOSEDISTANCE_MOVED;
	endcase
endfunction
