///////////////////
//  this function is the main fight loop for most warrior-type AIs
///////////////////

function Fight (opponent)
	if (opponent == me or opponent.script["humuc"] or opponent.dead)
		setwarmode (0);
		return;
	endif

	if ((me.z > (opponent.z + 20)) or (me.z < (opponent.z - 20)) )
		setwarmode (0);
		return;
	endif

	if (opponent.cmdlevel)
		if (!GetObjProperty (opponent, "attackable"))
			setwarmode (0);
			return;
		endif
	endif

	SetOpponent (opponent);
	prepare_for_fight (opponent);
	TurnToward (opponent);

	if (GetObjProperty (me, "keywordbattlecry"))
		Say (GetObjProperty (me, "keywordbattlecry"));
	endif

	var steps := 0;	//steps is a short-term counter for closing in on the opponent
	var loops := 0;	//loops counts multiple steps
    	var ev;
	var waittime := 0;	//Waittime is the length of time to wait for events
	var randomruns := 0;
	var oldx := opponent.x;
	var oldy := opponent.y;
	var d;			//Distance from be to opponent
	var oldd := distance (me, opponent);
	var wasmoving := 1;
	var runaways := 0;	//run away from opponent if we're high on mana

	while (opponent and !opponent.dead and !opponent.hidden and !opponent.concealed)
		d := Distance (me, opponent);

		//If our opponent is pretty far away, look around for teleporters
		if (d > 21)
			var thegate := FindTeleporterToPlayer (opponent);
			if (thegate)
				RunToLocation (thegate.x, thegate.y, 10);
				waittime := 1;
				wasmoving := 1;
			elseif (me.x < 12 AND me.realm == REALM_BRITANNIA)
				if (opponent.x > 5090 AND opponent.realm == REALM_BRITANNIA)
MoveObjectToLocation( me, 5100, me.y, me.z, me.realm, MOVEOBJECT_FORCELOCATION );
				else
					post_combat ();
					return;
				endif
			elseif (me.x > 5090 AND me.realm == REALM_BRITANNIA)
				if (opponent.x < 12 AND opponent.realm == REALM_BRITANNIA)
MoveObjectToLocation( me, 10, me.y, me.z, me.realm, MOVEOBJECT_FORCELOCATION );
				else
					post_combat ();
					return;
				endif
			else
				post_combat ();
				return;
			endif

		//If we're pretty far away, run toward the opponent
		elseif (d > 10)
			RunToward (opponent);
			waittime := 0;
			wasmoving := 1;

		//We're close to the opponent.  If we have mana, we need to run away a bit.  Otherwise melee
		elseif (d < 2)
			//They're on a different floor, which is too difficult to deal with
			if ( (me.z > (opponent.z + 20)) or (me.z < (opponent.z - 20)) )
				post_combat ();
				return;
			endif

			if (runaways < 5)
				wasmoving := 1;
				waittime := 0;
				if (GetVital (me, "Mana") * 3 > GetVitalMaximumValue (me, "Mana"))
					TurnAwayFrom (opponent);
					RunAwayFrom (opponent);
					RunAwayFrom (opponent);
					RunAwayFrom (opponent);
					runaways := runaways + 1;
				else
					runaways := 20;
				endif
			else
				//reset runaway if we have a decent amount of mana
				if (GetVital (me, "Mana") * 2 > GetVitalMaximumValue (me, "Mana"))
					runaways := 0;
				endif
				steps := 0;
				randomruns := 0;
				if (CheckLoSAt (me, opponent.x, opponent.y, opponent.z))
					loops := 1;
					waittime := 1;
					EraseObjProperty (me, "#flees");
					if (wasmoving)
						TurnToward (opponent);
						wasmoving := 0;
					endif
				else
					waittime := 0;
					loops := loops + 1;
					RunTowardLocation (me.x+(randomint(3)-1), me.y+(randomint(3)-1));
				endif
			endif
		//We're a good distance away to zap them with spells, if we have mana
		else
			if (GetVital (me, "Mana") * 3 > GetVitalMaximumValue (me, "Mana"))
				if (CheckLoSAt (me, opponent.x, opponent.y, opponent.z))
					loops := 1;
					waittime := 1;
					EraseObjProperty (me, "#flees");
					if (wasmoving)
						TurnToward (opponent);
						wasmoving := 0;
					endif
				else
					waittime := 0;
					loops := loops + 1;
					RunTowardLocation (me.x+(randomint(3)-1), me.y+(randomint(3)-1));
				endif
			//too low on mana, goto melee
			else
				runaways := 20;
				wasmoving := 1;
				waittime := 0;
						
				if (steps < 6)
					steps := steps + 1;
					RunToward (opponent);
				elseif (oldx >= (opponent.x-1) and oldx <= (opponent.x+1) and 
						oldy >= (opponent.y-1) and oldy <= (opponent.y+1))
			
					//our opponent hasn't moved much but we did, so:
					if (oldd <= d)
						//try to open a door first...
						if (OpenDoors (opponent))
							steps := 0;
						//Then see if they're on a boat, and if they are, teleport to them
						elseif ((opponent.multi).isa(POLCLASS_BOAT) and oldd < 3)
MoveObjectToLocation( me, opponent.x, opponent.y, opponent.z, opponent.realm, MOVEOBJECT_FORCELOCATION );
						//if all else fails, just try to run around randomly, or else flee
						else
							randomruns := randomruns + 1;
							if (RunRandom (opponent, (randomruns*2)))
								PeaceMade ();
								return;
							endif
							oldd := d;
							steps := 0;
							if ( randomruns > 5 )
								//screw it, we can't get to him
								flee (opponent);
							endif
						endif
					else
						//if we're closing in, lets reset the distance and steps
						if (d < oldd)
							steps := 0;
							oldd := d;
						endif
					endif
				else		//we moved, but so did our opponent, so lets chase him longer:
					loops := loops + 1;
					oldx := opponent.x;
					oldy := opponent.y;
					oldd := d;
					steps := 0;
					if ( loops > 50 )
						//screw it, we can't catch him
						Flee (opponent);
					endif
				endif
			endif
		endif
		in_combat_event_loop (opponent);

		ev := wait_for_event (waittime);
		if (ev)
			repeat
				case (ev.type)
					EVID_ALL_ATTACK_CMD:
						if (ev.source != me and !ev.source.script["humuc"])
							if ( (me.z < (ev.source.z + 25)) and (me.z > (ev.source.z - 25)) )
								if (!ev.source.cmdlevel or GetObjProperty (ev.source, "attackable"))
									opponent := ev.source;
									prepare_for_fight (opponent);
									SetOpponent (opponent);
									turntoward (opponent);
									SetObjProperty (me, "#lastchangeopponent", ReadGameClock());
								endif
							endif
						endif
					EVID_DAMAGED:
					EVID_ENGAGED:
						if (ev.source)
							process_combat_event (ev);
							if (ev.source != opponent and ev.source != me)
								if (ChangeOpponent (opponent, ev.source))
									opponent := ev.source;
									prepare_for_fight (opponent);
									SetOpponent (opponent);
									turntoward (opponent);
								endif
							endif
						endif
					EVID_PEACEMADE:
						PeaceMade ();
						return;
				endcase
				ev := wait_for_event (0);
			until (!ev);
		endif

		oldd := d;
		oldx := opponent.x;
		oldy := opponent.y;
	endwhile

	post_combat();

endfunction

function EnableCombatEvents()
    EnableEvents (SYSEVENT_ENGAGED + SYSEVENT_DAMAGED, 20);
endfunction

function DisableCombatEvents()
    DisableEvents (SYSEVENT_ENGAGED + SYSEVENT_DAMAGED);
endfunction

function PeaceMade ()
	SetWarMode ( 0 );
	SetOpponent ( 0 );
	sleep (1);
	DisableCombatEvents ();
	EnableMainEvents ();

	next_look_around := ReadGameClock () + 30;

endfunction
