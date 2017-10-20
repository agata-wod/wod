///////////////////
//  this function is the main fight loop for most AIs
///////////////////

function Fight (byref opponent)
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
	var oldd := distance (me, opponent);
	var wasmoving := 1;

	while (opponent and !opponent.dead and !opponent.hidden and !opponent.concealed) 
		//If our opponent is pretty far away, look around for teleporters
		if (distance (me, opponent) > 21)
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

		if ( (me.z > (opponent.z + 20)) or (me.z < (opponent.z - 20)) )
			post_combat ();
			return;
		endif

		case ( CloseDistance ( opponent ) )
			0:	//Closedistance returned 0, which means the mob needs to move
				wasmoving := 1;
				loops := loops + 1;
				steps := steps + 1;
				waittime := 0;
				if ( ( oldx >= (opponent.x-1) and (oldx <= opponent.x+1)) and 
						( oldy >= (opponent.y-1) and (oldy <= opponent.y+1)) and (oldd > 1) )

					//our opponent hasn't moved much but we did, so:
					if ( (steps > 4) and (oldd <= distance (me, opponent)) )
						//try to open a door first...
						if (OpenDoors (opponent) )
							steps := 0;
						//Then see if they're on a boat, and if they are, teleport to them
						elseif ((opponent.multi).isa(POLCLASS_BOAT) and oldd < 3)
	MoveObjectToLocation( me, opponent.x, opponent.y, opponent.z, opponent.realm, MOVEOBJECT_FORCELOCATION );

						//try to cast the teleport spell to get closer to them...
						elseif (CastTeleportSpell (opponent) )
							steps := 0;
						//if all else fails, just try to run around randomly, or else flee
						else
							randomruns := randomruns + 1;
							if ( RunRandom (opponent, (randomruns*2)))
								PeaceMade ();
								return;
							endif

							oldd := distance(me,opponent);
							steps := 0;
							if ( randomruns > 5 )
								//screw it, we can't get to him
								flee(opponent);
							endif
						endif
					else
						//if we're closing in, lets reset the distance and steps
						if ( distance(me,opponent) < oldd )
							steps := 0;
							oldd := distance (me, opponent);
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

			1:	//Closedistance returned 1, which means the mob doesn't need to move
				//unless he can't see his opponent
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

			2:	//closedistance returned 2, which means the mob was peacemade:
				PeaceMade();
				return;

			3:	//closedistance returned 3, which means the mob is running away
				loops := 0;  //closedistance maintains its own runaway counter
				steps := 0;
				randomruns := 0;
				waittime := 0;
				wasmoving := 1;
		endcase

		in_combat_event_loop (opponent);

		ev := wait_for_event ( waittime );
		case (ev.type)
			EVID_ALL_ATTACK_CMD:
				if (ev.source != me and !ev.source.script["humuc"])
					if ( (me.z < (ev.source.z + 25)) and (me.z > (ev.source.z - 25)) )
						if (!ev.source.cmdlevel or GetObjProperty (ev.source, "attackable"))
							opponent := ev.source;
							prepare_for_fight (opponent);
							SetOpponent (opponent);
							turntoward (opponent);
							SetObjProperty (me, "#lastchangeopponent", ReadGameClock()+2);
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
							prepare_for_fight ( opponent );
							SetOpponent ( opponent );
							turntoward ( opponent );
						endif
					endif
				endif
			EVID_PEACEMADE:
				PeaceMade ();
				return;
		endcase

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

///////////////////
//  cast the teleport spell to try to get to them
///////////////////

function CastTeleportSpell (byref opponent)
	if (GetVital (me, "Mana") < 1000)
		return 0;
	endif

	if (Distance (me, opponent) < 2)
		return 0;
	endif

	if (Distance (me, opponent) > 15)
		return 0;
	endif

	if (CheckLoSAt (me, opponent.x, opponent.y, opponent.z))
		return 0;
	endif

	if (RandomInt (2))
		return 0;
	endif

	var weapon := GetEquipmentByLayer (me, LAYER_HAND1);
	if (weapon)
		MoveItemToContainer (weapon, me.backpack);
		PlaySoundEffect (me, 0x0057);
	endif

	var shield := GetEquipmentByLayer (me, LAYER_HAND2);
	if (shield)
		MoveItemToContainer (shield, me.backpack);
	endif
	sleep (2);
	CastSpell (me, opponent, ":travel_spells:teleport");
	sleep (1);
			
	if (weapon)
		PlaySoundEffect (me, 0x0057);
		EquipItem (me, weapon);
	endif
	if (shield)
		EquipItem (me, shield);
	endif
	return 1;
endfunction


