function main_AI_loop()
	var ev;
	var wanders := 100;
	next_look_around := ReadGameClock () + 30;
	var mynoise := GetAnimalNoise ();

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
						if (!ev.source.npctemplate)
							if (RandomInt (4) == 1)
								PlaySoundEffect (me, mynoise);
							endif
						endif
					EVID_ALL_ATTACK_CMD:
						Fight ( ev.source );
					EVID_ENGAGED:
						RunLikeHell (ev.source);
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
//  animals are cowardly - when engaged, they try to run, though they sometimes fight back when hurt
///////////////////

function RunLikeHell (byref opponent)
	var ev;
	while ( opponent and !opponent.dead and !opponent.hidden and !opponent.concealed
				and (distance(me,opponent) < 25) ) 
		Runawayfrom (opponent);
		repeat
			case (ev.type)
				EVID_DAMAGED:
					if (ev.source)
						GetHelpFromAllies (ev.source);
						fight (ev.source);
						return;
					endif
			endcase
			ev := wait_for_event (1);
		until (!ev);
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

function GetAnimalNoise ()
	case (me.graphic)
		CID_BIRD:
			case (Randomint (3))
				0:	return 0x1B;
				1:	return 0x18;
				2:	return 0x277;
			endcase
		CID_CAT:
			return 0x6B;
		CID_COW:
		CID_COW2:
			return 0x7A;
		CID_BULL:
		CID_BULL2:
			return 0x66;
		CID_DOG:
			case (RandomInt (2))
				0:	return 0x86;
				1:	return 0x87;
			endcase
		CID_GOAT:
			case (randomint(2))
				0:	return 0x9A;
				1:	return 0x9B;
			endcase	
		CID_PIG:
			return 0xC6;
		CID_LIGHT_HORSE:
		CID_GREY_HORSE:
		CID_TAN_HORSE:
		CID_BROWN_HORSE:
		CID_PACK_HORSE:
			case (Randomint (4))
				0:	return 0xA9;
				1:	return 0xAA;
				2:	return 0xAB;
				3:	return 0xAC;
			endcase
		CID_LLAMA:
			return 0xb8;
		CID_TOAD:
			case (randomint(4))
				0:	return 0x267;
				1:	return 0x268;
				2:	return 0x26A;
				3:	return 0x26D;
			endcase
		CID_BROWN_BEAR:
		CID_GRIZZLY_BEAR:
		CID_POLAR_BEAR:
			case (randomint(5))
				0:	return 0xA4;
				1:	return 0xA5;
				2:	return 0xA6;
				3:	return 0xA7;
				4:	return 0xA8;
			endcase
		CID_APE:
			case (randomint(3))
				0:	return 0x9F;
				1:	return 0xA0;
				2:	return 0xA2;
			endcase
		CID_DOLPHIN:
			case (randomint(3))
				0:	return 0x8b;
				1:	return 0x8c;
				2:	return 0x8d;
			endcase
	endcase

	return 0;
endfunction
