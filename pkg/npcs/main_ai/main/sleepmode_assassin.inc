function sleepmode()

	foreach critter in ListMobilesNearLocation( me.x,me.y,me.z, 10, me.realm );
		if (!critter.npctemplate)
			return 0;
		endif
	endforeach

	if (GetObjProperty (me,"killme"))
		KillMe ();
	endif

	Eraseobjproperty (me, "#flees");
	EraseObjProperty (me, "#hitlist");

	DisableMainEvents ();
	EnableEvents (SYSEVENT_ENGAGED + SYSEVENT_DAMAGED, 20);
	me.hidden := 1;
	EnableEvents (SYSEVENT_LEFTAREA, 4);

	var ev;
	while (me)
		ev := wait_for_event ( 120 );
		case (ev.type)
			EVID_ENGAGED:
			EVID_DAMAGED:
			EVID_ALL_ATTACK_CMD:
				if (me.hidden)
					me.hidden := 0;
					say ("*" + me.name + " springs from hiding!*");
				endif
				EnableMainEvents();
				return ev;
			EVID_LEFTAREA:
				if ( (!ev.source.dead) and (GetVital (me, "Life") > 1000) )
					if ( (!ev.source.npctemplate) or (ev.source.script["tamed"]) )
						if (me.hidden)
							me.hidden := 0;
							say ("*" + me.name + " springs from hiding!*");
						endif
						EnableMainEvents();
						ev.type := EVID_ENTEREDAREA;
						return ev;
					endif
				endif
		endcase
	endwhile      

endfunction       
