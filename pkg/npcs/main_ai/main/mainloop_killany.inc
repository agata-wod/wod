function main_AI_loop()
	var ev;
	var wanders := 100;
	next_look_around := ReadGameClock () + 30;

	while (me)
		if (wanders > 60)
			wanders := 0;
			ev := sleepmode ();
		else
			ev := wait_for_event (2);
		endif

		if (ReadGameClock() > next_look_around)
			look_around ();
			if (IAmALooter ())
				GrabLoot ();
			endif
			next_look_around := ReadGameClock () + 30;
		endif

		if (GetObjProperty (me, "destinationx"))
			var mydestx := GetObjProperty (me, "destinationx");
			var mydesty := GetObjProperty (me, "destinationy");
			var steps := GetObjProperty (me, "destinationsteps");
			SetObjProperty (me, "destinationsteps", (steps + 1));
			WalkTowardLocation (mydestx, mydesty);
			if ( (me.x == mydestx and me.y == mydesty) or ( steps > 10 ) )
				EraseObjProperty (me, "destinationx");
				EraseObjProperty (me, "destinationy");
				EraseObjProperty (me, "destinationsteps");
			endif
		else	
			wander();
			wanders := wanders +1;
		endif

		if (ev)
			repeat
				case (ev.type)
					EVID_ENGAGED:
						if (distance (me, ev.source) < 6)
							GetHelpFromAllies (ev.source);
							Fight (ev.source);
						endif
					EVID_ENTEREDAREA:
					EVID_ALL_ATTACK_CMD:
					EVID_DAMAGED:
						if (!ev.source.script["humuc"] and !ev.source.script ["merchant"])
							GetHelpFromAllies (ev.source);
							Fight (ev.source);
						endif
					EVID_ALL_FOLLOW_CMD:
						SetObjProperty (me, "destinationx", ev.x);
						SetObjProperty (me, "destinationy", ev.y);
						SetObjProperty (me, "destinationsteps", 0);
						drop_anchor ();
						WalkTowardLocation (ev.x, ev.y);
				endcase
				ev := wait_for_event ( 2 );
			until (!ev);
		endif

		if (ReadGameClock () > next_look_around)
			next_look_around := ReadGameClock () + 15;
			look_around ();
		endif
	endwhile
endfunction

function look_around ()
	foreach critter in ListMobilesInLineOfSight( me, HALT_THRESHOLD )
		if (critter.script != "humuc" and !critter.script["merchant"])
              	Fight (critter);
			return;
		endif
	endforeach
endfunction

function EnableMainEvents()
	DisableEvents (SYSEVENT_SPEECH + SYSEVENT_LEFTAREA + SYSEVENT_DISENGAGED + SYSEVENT_OPPONENT_MOVED );
	EnableEvents (SYSEVENT_ENGAGED + SYSEVENT_DAMAGED, 20);
	EnableEvents (SYSEVENT_ENTEREDAREA, HALT_THRESHOLD);
endfunction

function DisableMainEvents()
	DisableEvents (SYSEVENT_ENGAGED + SYSEVENT_DAMAGED + SYSEVENT_ENTEREDAREA);
endfunction