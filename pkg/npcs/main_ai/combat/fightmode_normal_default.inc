///////////////////
//  this function is the main fight loop for most warrior-type AIs
///////////////////

function Fight (opponent)
	if (opponent == me or opponent.dead)
		setwarmode (0);
		return;
	elseif (opponent.script["humuc"])
		setwarmode (0);
		return;
	elseif ((me.z > (opponent.z + 20)) or (me.z < (opponent.z - 20)) )
		setwarmode (0);
		return;
	elseif (opponent.cmdlevel)
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
	var oldd := distance (me, opponent);
	var wasmoving := 1;

	while (opponent and !opponent.dead and !opponent.hidden and !opponent.concealed) 
		//If our opponent is pretty far away, look around for teleporters
		if (Distance (me, opponent) > 21)
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
		endif

		//check our Z again
		if ( (me.z > (opponent.z + 20)) or (me.z < (opponent.z - 20)) )
			post_combat ();
			return;
		endif
		
		//Call closedistance, which will try to do some pathfinding and return different
		//values based on result
		case (CloseDistance (opponent))
			CLOSEDISTANCE_MOVED:
				wasmoving := 1;
				loops := loops + 1;
				steps := steps + 1;
				waittime := 0;

				//check to see if our opponent moved
				if (coordist (oldx, oldy, opponent.x, opponent.y) < 2)
					//our opponent hasn't moved much but we did, so check if we're closing in
					if (oldd < distance (me, opponent))
						//yahoo, we're getting closer!
						steps := 0;
						oldd := distance (me, opponent);
					else	//we're not getting closer
						if (steps > 4)	//we've been running a bit, so lets try something different
							//try to open a door first...
							if (OpenDoors (opponent) )
								steps := 0;
							//Then see if they're on a boat, and if they are, teleport to them
							elseif ((opponent.multi).isa(POLCLASS_BOAT) and oldd < 3)
                MoveObjectToLocation( me, opponent.x, opponent.y, opponent.z, opponent.realm, MOVEOBJECT_FORCELOCATION );

							//if all else fails, just try to run around randomly, or else flee
							else
								randomruns := randomruns + 1;
								if ( RunRandom (opponent, (randomruns*2)))
									PeaceMade ();
									return;
								endif
	
								oldd := distance (me, opponent);
								steps := 0;
								if ( randomruns > 5 )
									//screw it, we can't get to him
									flee (opponent);
								endif
							endif
						endif
					endif
				else		//we moved, but so did our opponent, so lets chase him longer:
					oldx := opponent.x;
					oldy := opponent.y;
					steps := 0;
					if ( loops > 50 )
						//screw it, we can't catch him
						Flee (opponent);
					endif
				endif

			CLOSEDISTANCE_DIDNT_MOVE:
				//We're in a decent place to fight, unless we can't see our opponent
				if (wasmoving)
					TurnToward (opponent);
				endif
				wasmoving := 0;

				steps := 0;
				randomruns := 0;
				if (CheckLoSAt (me, opponent.x, opponent.y, opponent.z))
					loops := 0;
					waittime := 1;
					EraseObjProperty (me, "#flees");
				else
					loops := loops +1;
					waittime := 1;
					RunTowardLocation (me.x+(randomint(3)-1), me.y+(randomint(3)-1));
				endif

			CLOSEDISTANCE_PEACEMADE:
				PeaceMade();
				return;

			CLOSEDISTANCE_FLEEING:
				loops := 0;  //closedistance maintains its own runaway counter
				steps := 0;
				randomruns := 0;
				waittime := 0;
				wasmoving := 1;
		endcase

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
	endwhile

	post_combat();
endfunction

function EnableCombatEvents ()
    EnableEvents (SYSEVENT_ENGAGED + SYSEVENT_DAMAGED, 20);
endfunction

function DisableCombatEvents ()
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
