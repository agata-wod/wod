function main_AI_loop (next_wander := 0)

	var ev;
	var wanders := 100;
	next_look_around := ReadGameClock () + 300;
	look_around();
	EnableMainEvents();

	while (me)
		if (wanders > 60)
			wanders := 0;
			ev := sleepmode();
		else
			ev := wait_for_event (2);
		endif

		if (ReadGameClock () > next_wander)
			if (GetObjProperty (me, "destinationx"))
				var mydestx := GetObjProperty (me, "destinationx");
				var mydesty := GetObjProperty (me, "destinationy");
				var steps := CINT (GetObjProperty (me, "destinationsteps"));
				SetObjProperty (me, "destinationsteps", CINT(steps + 1));
				WalkTowardLocation (mydestx, mydesty);
				if ( (me.x == mydestx and me.y == mydesty) or ( steps > 10 ) )
					EraseObjProperty (me, "destinationx");
					EraseObjProperty (me, "destinationy");
					EraseObjProperty (me, "destinationsteps");
				endif
				next_wander := ReadGameClock ();
			else	
				wander();
				wanders := wanders + 1;
				next_wander := ReadGameClock () + 3;
			endif
		endif

		if (ev)
			repeat
				case (ev.type)
					EVID_SPEECH:
						if (!ev.source.npctemplate)
							next_wander := ReadGameClock () + 30;
							wanders := 0;
							check_speech (ev.text, ev.source);
						endif
					EVID_ITEM_GIVEN:
						TakeItem (ev.source, ev.item);
						next_wander := ReadGameClock () + 60;
					EVID_ALL_ATTACK_CMD:
					EVID_ENGAGED:
					EVID_DAMAGED:
						if (ev.source)
							GetHelpFromAllies ( ev.source );
							Run (ev.source);
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
	endwhile
endfunction

function look_around ()
	return;
endfunction

function EnableMainEvents()
	DisableEvents (SYSEVENT_LEFTAREA + SYSEVENT_DISENGAGED + SYSEVENT_OPPONENT_MOVED );
	EnableEvents (SYSEVENT_ENGAGED + SYSEVENT_DAMAGED, 20);
	EnableEvents (SYSEVENT_ENTEREDAREA, HALT_THRESHOLD);
	EnableEvents (SYSEVENT_SPEECH + SYSEVENT_ITEM_GIVEN, 3);
endfunction

function DisableMainEvents()
	DisableEvents (SYSEVENT_ENGAGED + SYSEVENT_DAMAGED + SYSEVENT_ENTEREDAREA);
	DisableEvents (SYSEVENT_SPEECH + SYSEVENT_ITEM_GIVEN);
endfunction

function Run (opponent)
	TurnAwayFrom (opponent);
	DisableEvents (EVID_SPEECH + EVID_ITEM_GIVEN);
	EnableEvents (EVID_DISENGAGED);
	var waittime, loops := 0;

	outer:
	while (opponent and !opponent.dead)
		loops := loops + 1;
		waittime := OpenDistance (opponent) * 15;
		if (waittime)
			break outer;
		endif		

		var ev := wait_for_event( waittime );
		case (ev.type)
			EVID_DISENGAGED:
				if (ev.source == opponent)
					say ("Whew!");
					break outer;
				endif
			EVID_ENGAGED:
			EVID_DAMAGED:
				Spout();
		endcase

		if (loops > 100)
			sleep (1);
			Fight (opponent);
		endif
	endwhile

	DisableEvents (EVID_DISENGAGED);
	EnableEvents (EVID_ITEM_GIVEN);
	EnableEvents (EVID_SPEECH, 2);
	SetWarMode (0);
endfunction

function Spout()

	case (RandomInt(60))
	0:	say( "Ack! Leave me alone!" );
	1:	say( "Pick on someone your own size!" );
	2:	say( "Why are you doing this?  WHY?!" );
	3:	say( "Leave me be!  I've done you no harm!" );
	4:	say( "Kal Ort Por" );
		say( "Damn! No regs...");
	5:	say( "Ah!  But I'm too young to die!" );
	6:	say( "Brute!  Fiend!  Aaaah!" );
	7:	say( "He who runs and runs away, lives to run another day!" );
	8:	say( "So... wanna fight eh?  Fight someone else!" );
	9:	say( "Aaahh!  NPCK!!" );
	10:	say( "I'm going to get help!" );
	11:	say( "Ouch!  Help!!" );
	12:	say( "Bet I can outrun you!" );
	13:	say( "Stop!  I'm serious!  Just stop!" );
	14:	say( "Aaah!  Spare me!  Please, Spare me!" );
	15:	say( "Ack!  I'm innocent I tell you!" );
	16:	say( "Find a more worthy opponent!" );
	17:	say( "I'm not prepared to meet my maker!" );
	18:	say( "Wait!  Stop!  I'm a BLEEDER!" );
	19:	say( "My family will have vengeance!" );
	20:	say( "Look behind you!  What is that terrible thing!?" );
	21:	say( "Not in the face!  Not in the face!" );
	22:	say( "Aaahhh! Help! Help!  I'm dying!");
	23:	say( "By the virtues! Why? WHY??" );
	24:	say( "Perhaps it will help to confuse you if I run away some more..." );
	25:	say( "Run!  Hide!  We're all gonna die!" );
	default:	return;
	endcase

	return;

endfunction

function OpenDistance (byref opponent)

    case (Distance( me, opponent ))
        20:
            return 1;
        
        default:
            RunAwayFrom( opponent );
            return 0;
    endcase

endfunction