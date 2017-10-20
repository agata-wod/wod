///////////////////
//  this function is the main fight loop for most AIs
///////////////////

function Fight ( byref opponent )

	if (opponent == me or opponent.npctemplate == "humuc")
		setwarmode(0);
		opponent := 0;
		return;
	endif

	if ( (me.z > (opponent.z + 20)) or (me.z < (opponent.z - 20)) )
		setwarmode(0);
		opponent := 0;
		return;
	endif

	if (opponent.cmdlevel > 0)
		var attackable := GetObjProperty (opponent, "attackable");
		if (!attackable)
			setwarmode(0);
			opponent := 0;
			return;
		endif
	endif

	set_priority (50);
	SetOpponent (opponent);
	prepare_for_fight (opponent);
	TurnToward (opponent);

	if ( GetObjProperty (me, "keywordbattlecry") )
		Say ( GetObjProperty (me, "keywordbattlecry") );
	endif

    	var ev;

	while ( opponent and !opponent.dead and !opponent.hidden and !opponent.concealed
			and (distance (me, opponent) < 20) ) 

		if ( (me.z > (opponent.z + 20)) or (me.z < (opponent.z - 20)) )
			post_combat ();
			return;
		endif

		in_combat_event_loop (opponent);

		ev := wait_for_event (1);
		case (ev.type)
			EVID_DAMAGED:
			EVID_ENGAGED:
				if (ev.source)
					process_combat_event (ev);
					if ( (ev.source != opponent) and (ev.source != me) )
						if ( (RandomInt(6)==1) or ( distance(ev.source,me) < distance(me, opponent) ) )
							if ( IsGood(ev.source) )
								opponent := ev.source;
								prepare_for_fight ( opponent );
								SetOpponent ( opponent );
								turntoward ( opponent );
								SetObjProperty (me, "#lastchangeopponent", ReadGameClock()+2);
							endif
						endif
					endif
				endif
			EVID_PEACEMADE:
				PeaceMade ();
				return;
		endcase

	endwhile

	post_combat();

endfunction

function EnableCombatEvents()
    EnableEvents (SYSEVENT_ENGAGED + SYSEVENT_DAMAGED, 20);
endfunction

function DisableCombatEvents()
    DisableEvents (SYSEVENT_ENGAGED + SYSEVENT_DAMAGED);
endfunction

function PeaceMade ()

	SetWarMode ( 0 );
	SetOpponent ( 0 );
	sleep (1);
	DisableCombatEvents ();
	EnableMainEvents ();

	next_look_around := ReadGameClock () + 30;

endfunction
