
function sleepmode ()
	foreach critter in ListMobilesNearLocationEx (me.x, me.y, me.z, 20,
			LISTEX_FLAG_NORMAL+LISTEX_FLAG_HIDDEN+LISTEX_FLAG_GHOST, me.realm);
		if (!critter.npctemplate)
			return 0;
		endif
	endforeach

	if (GetObjProperty (me, "killme"))
		KillMe ();
	endif

	if (GetObjProperty (me, "myhome"))
		WalkHome ();
	endif
	EraseObjproperty (me, "#flees");
	EraseObjProperty (me, "#provoked");
	EraseObjProperty (me, "#peacemade");
	EraseObjProperty (me, "#summons");
	EraseObjProperty (me, "#lastspell");
	EraseObjProperty (me, "#lastpotion");
	EraseObjProperty (me, "#lastbreath");
	EraseObjProperty (me, "#opendoors");
	EraseObjProperty (me, "#nexthealing");
	EraseObjProperty (me, "#usedbandagecount");
	EraseObjProperty (me, "#hitlist");

	DisableMainEvents ();
	EnableEvents (SYSEVENT_ENGAGED + SYSEVENT_DAMAGED + SYSEVENT_ENTEREDAREA + SYSEVENT_SPEECH, 20);
    
	var ev;
	while (me)
		ev := os::wait_for_event (120);
		case (ev.type)
			EVID_ENGAGED:
			EVID_DAMAGED:
			EVID_SPEECH:
			EVID_ALL_FOLLOW_CMD:
			EVID_ALL_ATTACK_CMD:
				EnableMainEvents ();
				return ev;
			EVID_ENTEREDAREA:
				if (!ev.source.npctemplate)
					EnableMainEvents();
					return ev;
				endif
		endcase
	endwhile      
endfunction       
