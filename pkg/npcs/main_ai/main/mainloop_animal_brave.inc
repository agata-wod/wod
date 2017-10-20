function main_AI_loop()
	var ev;
	var wanders := 100;
	next_look_around := ReadGameClock () + 30;

	while (me)
		if (wanders > 60)
			wanders := 0;
			ev := sleepmode();
		else
			ev := wait_for_event ( 2 );
			PlayAnimalSoundEffects ();
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
			wanders := wanders + 1;
		endif

		if (ev)
			repeat
				case (ev.type)
					EVID_ENTEREDAREA:
						PlayAnimalSoundEffects ();
					EVID_ALL_ATTACK_CMD:
						Fight ( ev.source );
					EVID_DAMAGED:
						if (ev.source)
							GetHelpFromAllies ( ev.source );
							fight(ev.source);
						endif
					EVID_ALL_FOLLOW_CMD:
						SetObjProperty (me, "destinationx", ev.x);
						SetObjProperty (me, "destinationy", ev.y);
						SetObjProperty (me, "destinationsteps", 0);
						drop_anchor ();
						WalkTowardLocation (ev.x, ev.y);
				endcase
				ev := wait_for_event (2);
			until (!ev);
		endif
	endwhile
endfunction




///////////////////
//  the look_around function, which really isn't used for animals, so its just blank (but needed)
///////////////////

function look_around()
	return;
endfunction

function EnableMainEvents()
	DisableEvents (SYSEVENT_SPEECH + SYSEVENT_LEFTAREA + SYSEVENT_DISENGAGED + SYSEVENT_OPPONENT_MOVED );
	EnableEvents (SYSEVENT_ENGAGED + SYSEVENT_DAMAGED, 20);
	EnableEvents (SYSEVENT_ENTEREDAREA, HALT_THRESHOLD);
endfunction

function DisableMainEvents()
	DisableEvents (SYSEVENT_ENGAGED + SYSEVENT_DAMAGED + SYSEVENT_ENTEREDAREA);
endfunction

function PlayAnimalSoundEffects ()
	if (RandomInt (4))
		SetObjProperty (me, "#nextsound", ReadGameClock() + 3 + RandomInt (5));
		return;
	endif

	var nextsound := GetObjProperty (me, "#nextsound");
	if (nextsound)
		if (nextsound > ReadGameClock ())
			return;
		endif
	endif
	SetObjProperty (me, "#nextsound", ReadGameClock() + 3 + RandomInt (5));

	case (me.graphic)
		CID_BIRD:
			case (Randomint (3))
				0:	PlaySoundEffect (me, 0x1B);
				1:	PlaySoundEffect (me, 0x18);
				2:	PlaySoundEffect (me, 0x277);
			endcase
		CID_CAT:
			PlaySoundEffect (me, 0x6B);
		CID_COW:
		CID_COW2:
			PlaySoundEffect (me, 0x7A);
		CID_DOG:
			case (RandomInt (2))
				0:	PlaySoundEffect (me, 0x86);
				1:	PlaySoundEffect (me, 0x87);
			endcase
		CID_GOAT:
			case (randomint(2))
				0:	PlaySoundEffect (me, 0x9A);
				1:	PlaySoundEffect (me, 0x9B);
			endcase	
		CID_PIG:
			PlaySoundEffect (me, 0xC6);
		CID_LIGHT_HORSE:
		CID_GREY_HORSE:
		CID_TAN_HORSE:
		CID_BROWN_HORSE:
		CID_PACK_HORSE:
			case (Randomint (4))
				0:	PlaySoundEffect (me, 0xA9);
				1:	PlaySoundEffect (me, 0xAA);
				2:	PlaySoundEffect (me, 0xAB);
				3:	PlaySoundEffect (me, 0xAC);
			endcase
		CID_LLAMA:
			PlaySoundEffect (me, 0xb8);
		CID_TOAD:
			case (randomint(4))
				0:	PlaySoundEffect (me, 0x267);
				1:	PlaySoundEffect (me, 0x268);
				2:	PlaySoundEffect (me, 0x26A);
				3:	PlaySoundEffect (me, 0x26D);
			endcase
		CID_BROWN_BEAR:
		CID_GRIZZLY_BEAR:
		CID_POLAR_BEAR:
			case (randomint(5))
				0:	PlaySoundEffect (me, 0xA4);
				1:	PlaySoundEffect (me, 0xA5);
				2:	PlaySoundEffect (me, 0xA6);
				3:	PlaySoundEffect (me, 0xA7);
				4:	PlaySoundEffect (me, 0xA8);
			endcase
		CID_APE:
			case (randomint(3))
				0:	PlaySoundEffect (me, 0x9F);
				1:	PlaySoundEffect (me, 0xA0);
				2:	PlaySoundEffect (me, 0xA2);
			endcase
		CID_DOLPHIN:
			case (randomint(3))
				0:	PlaySoundEffect (me, 0x8b);
				1:	PlaySoundEffect (me, 0x8c);
				2:	PlaySoundEffect (me, 0x8d);
			endcase
		254:
			case (randomint(2))
				0:	PlaySoundEffect (me, 0x4d9);
				1:	PlaySoundEffect (me, 0x4da);
			endcase
		248:
			case (randomint(3))
				0:	PlaySoundEffect (me, 0x4f8);
				1:	PlaySoundEffect (me, 0x4f9);
				2:	PlaySoundEffect (me, 0x4fa);
			endcase
	endcase

	return 0;
endfunction
