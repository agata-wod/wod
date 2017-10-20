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
			next_look_around := ReadGameClock () + 30;
		endif

		if (ev)
			repeat
				case (ev.type)
					EVID_ENTEREDAREA:
						Fight (ev.source);
					EVID_ENGAGED:
					EVID_DAMAGED:
						Fight( ev.source );
				endcase
				ev := wait_for_event (120);
			until (!ev);
		endif
	endwhile
endfunction

function look_around ()
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