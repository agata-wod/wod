
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
	EnableEvents (SYSEVENT_ENGAGED + SYSEVENT_DAMAGED, 20);
	if (GetAttribute (me, "Hiding") and !me.script["merchant"])
		if (RandomInt (100) < GetAttribute (me, "Hiding"))
			me.hidden := 1;
			EnableEvents (SYSEVENT_ENTEREDAREA, 4);
		else
			EnableEvents (SYSEVENT_ENTEREDAREA, 18);
	 	endif
 	else
		EnableEvents (SYSEVENT_ENTEREDAREA, 18);
 	endif

	var loops := 0;
	var ev;

	while (me)
		ev := os::wait_for_event (120);
		case (ev.type)
			EVID_ENGAGED:
			EVID_DAMAGED:
			EVID_ALL_ATTACK_CMD:
				if (me.hidden)
					me.hidden := 0;
					say ("*" + me.name + " springs from hiding!*");
				endif
				EnableMainEvents ();
				return ev;
			EVID_ENTEREDAREA:
				if (!ev.source.dead and !ev.source.concealed and (!ev.source.cmdlevel or GetObjProperty (ev.source, "attackable")))
					if ( (GetVital (me, "Life") > CINT (GetVitalMaximumValue (me, "Life")/2)) or (!me.hidden) )
						if (!ev.source.npctemplate or ev.source.script["tamed"])
							if (me.hidden)
								me.hidden := 0;
								say ("*" + me.name + " springs from hiding!*");
							endif
							EnableMainEvents ();
							return ev;
						endif
					endif
				endif
			EVID_ALL_FOLLOW_CMD:
				return ev;
		endcase

		loops := loops +1;
		if (GetObjProperty (me, "boatspawn") and loops > 10)
			KillMe ();
		endif
	endwhile      
endfunction       
