var hatelist := array{
	":wild_animals:rabbit",
	":wild_animals:deer",
	":wild_animals:gheart",
	":wild_animals:sheep",
	":wild_animals:forestostard",
	":wild_animals:desertostard",
	":wild_animals:hildisvini",
	":domestic_animals:cat",
	":domestic_animals:dog",
	":domestic_animals:cow",
	":domestic_animals:cow2",
	":domestic_animals:pig",
	":domestic_animals:goat",
	":domestic_animals:sheep",
	":domestic_animals:horse",
	":domestic_animals:horse2",
	":domestic_animals:horse3",
	":domestic_animals:horse4",
	":domestic_animals:horse5",
	":domestic_animals:chicken"
};

function main_AI_loop()
	var ev;
	var wanders := 100;
	next_look_around := ReadGameClock () + 30;
	look_around();
	EnableMainEvents();

	while (me)
		if (wanders > 60)
			wanders := 0;
			ev := sleepmode();
		else
			ev := wait_for_event ( 2 );
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
					EVID_ENTEREDAREA:
						if ( me.npctemplate["dog"] and !ev.source.npctemplate )
							case (RandomInt (2))
								0:	PlaySoundEffect (me, 0x86);
								1:	PlaySoundEffect (me, 0x87);
							endcase
						elseif ((ev.source.npctemplate) in hatelist)
							GetHelpFromAllies (ev.source);
							Fight ( ev.source );
						elseif ( ev.source.npctemplate == me.npctemplate)
							WalkToward (ev.source);
							ev := wait_for_event (1);
							WalkToward (ev.source);
						endif
					EVID_ALL_ATTACK_CMD:
						Fight ( ev.source );
					EVID_ENGAGED:
						if (distance (me, ev.source) < 6)
							GetHelpFromAllies (ev.source);
							Fight (ev.source);
						endif
					EVID_DAMAGED:
						if (ev.source)
							GetHelpFromAllies ( ev.source );
							fight (ev.source);
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
	foreach critter in ListMobilesInLineOfSight (me, HALT_THRESHOLD)
		if (critter.npctemplate)
			if ((critter.npctemplate) in hatelist)
				Fight ( critter );
				return;
			endif
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
