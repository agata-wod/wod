function process_combat_event (byref ev)
	if (ev.type == EVID_DAMAGED)
		if (GetVital (me, "Life") < CINT (GetVitalMaximumValue (me, "Life")/10))
			EraseObjProperty (me, "#flees");
			Flee (ev.source);
		endif
	endif
endfunction

function in_combat_event_loop (byref opponent)
  if (opponent)
  endif
endfunction

function post_combat()
	DisableCombatEvents();
	EnableMainEvents();

	SetWarMode( 0 );
	SetOpponent( 0 );

	sleep (1);
	look_around();
endfunction

function prepare_for_fight(opponent)
  if (opponent)
  endif
	DisableMainEvents();
	EnableCombatEvents();
endfunction
