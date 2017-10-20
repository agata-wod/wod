///////////////////
//
//  Random functions related to pathfinding
//
///////////////////

use uo;
use os;
use util;

include "include/utility";

Const CLOSEDISTANCE_MOVED			:= 0;
Const CLOSEDISTANCE_DIDNT_MOVE		:= 1;
Const CLOSEDISTANCE_PEACEMADE			:= 2;
Const CLOSEDISTANCE_FLEEING			:= 3;

///////////////////
//  If the player being attacked goes through a gate, this function tries to find it
///////////////////

function FindTeleporterToPlayer (opponent)
	if (me.frozen or me.paralyzed)
		sleep (1);
		return 0;
	elseif (me.script["immobile"])
		sleep (1);
		return 0;
	endif

	//Timer between tries
	var lookforgates := GetObjProperty (me, "#lookforgates");
	if (lookforgates and lookforgates > ReadGameClock())
		return;
	endif
	SetObjProperty (me, "#lookforgates", ReadGameClock() + 5);

	//Look for both teleporters and gates
	var nearbygates := array {};
	foreach item in ListItemsNearLocation (me.x, me.y, me.z, 7, me.realm)
		if (item.objtype == 0x16203 or item.objtype == 0x0f6c)
			nearbygates.append (item);
		endif
	endforeach

	if (!len (nearbygates))
		SetObjProperty (me, "#lookforgates", ReadGameClock() + 15);
		return;
	endif
	
	//If there's only one, use that one
	if (len (nearbygates) == 1)
		return (nearbygates[1]);
	endif

	//Otherwise, we'll try to figure out the best one
	foreach gate in nearbygates
		var destx := GetObjProperty (gate, "DestX");
		var desty := GetObjProperty (gate, "DestY");
		var destr := GetObjProperty (gate, "DestR");
		if (!destr)
			destr := REALM_BRITANNIA;
		endif
		if (destx and desty and destr == opponent.realm)
			if (CoorDist (opponent.x, opponent.y, destx, desty) < 10)
				return gate;
			endif
		endif
	endforeach

	return (nearbygates[1]);
endfunction




///////////////////
//  NPC looks around for doors and opens any they find
///////////////////

function OpenDoors (byref opponent)
	if (me.frozen or me.paralyzed)
		sleep (1);
		return 0;
	elseif (me.script["immobile"])
		sleep (1);
		return 0;
	endif

	//only check occasionally
	var nextopendoors := GetObjProperty (me, "#opendoors");
	if ( nextopendoors and nextopendoors > ReadGameClock() )
		return 0;
	endif

	//We're right by our opponent.  Why are we looking for doors?
	if ( distance (me, opponent) < 2 )
		SetObjProperty (me, "#opendoors", ReadGameClock() + 10 );
		return 0;
	endif

	//first, lets see if there's any doors nearby
	var nearbydoors := array {};
	foreach item in ListItemsNearLocation (me.x, me.y, me.z, 5, me.realm)
		if (item.isa (POLCLASS_DOOR))
			if (!item.locked)
				if (item.z > me.z-10 and item.z < me.z + 10)
					nearbydoors.append (item);
				endif
			endif
		endif
	endforeach

	//if there's no doors around here, give up
	if (!len (nearbydoors) )
		SetObjProperty (me, "#opendoors", ReadGameClock() + 10 );
		return 0;
	endif
	
	//If there's only one, use that one
	if (len (nearbydoors) == 1)
		OpenSelectedDoor (nearbydoors [1]);
		SetObjProperty(me, "#opendoors", ReadGameClock() + 30 );
		return 1;
	endif

	//first we try to be real picky about choosing a door
	var lesserxpos := me.x;
	if (opponent.x < me.x)
		lesserxpos := opponent.x;
	endif
	var lesserypos := me.y;
	if (opponent.y < me.y)
		lesserypos := opponent.y;
	endif

	foreach item in nearbydoors
		//make sure its helpful and we're not running in the opposite direction or something, and that we can probably get there
		if (item.x > lesserxpos or item.y > lesserypos)
			if (CheckLOSAt (me, item.x, me.y, (GetMapInfo(item.x, me.y, me.realm).z) ) or CheckLOSAt (me, me.x, item.y, (GetMapInfo(me.x, item.y, me.realm).z)))
				OpenSelectedDoor (item);
				SetObjProperty(me, "#opendoors", ReadGameClock() + 30 );
				return 1;
			endif
		endif
	endforeach

	//OK, being real picky didn't work, so lets be a bit less picky...
	foreach item in nearbydoors
		//make sure its helpful and we're not running in the opposite direction or something
		if (item.x > lesserxpos or item.y > lesserypos)
			OpenSelectedDoor (item);
			SetObjProperty(me, "#opendoors", ReadGameClock() + 30 );
			return 1;
		endif
	endforeach

	//OK, lets just try to open ANY door
	foreach item in nearbydoors
		OpenSelectedDoor (item);
		SetObjProperty (me, "#opendoors", ReadGameClock() + 30 );
		return 1;
	endforeach

	return 0;
endfunction




///////////////////
//  Subfunction of 'OpenDoors', this function opens the selected door and tries to stear the NPC through it
///////////////////

function OpenSelectedDoor (door)
	var diddooropen := 0;
	var runstodoor := 0;
	
	//Doors tend to move when you open them, so we have to save the old position
	var olddoorx := door.x;
	var olddoory := door.y;
	
	repeat
		runstodoor := runstodoor + 1;
		if (Distance (me, door) <= 2 and !diddooropen)
			start_script (":doors:npcdoor", me);
			diddooropen := 1;
		endif
		RunTowardLocation (olddoorx, olddoory);
		if (me.x == olddoorx and me.y == olddoory)
			return;
		endif
	until (runstodoor > 10);
endfunction




///////////////////
//Runs towards a location, if we can
///////////////////

function RunToLocation (x, y, maxruns := 10)
	if (me.frozen or me.paralyzed)
		sleep (1);
		return 0;
	elseif (me.script["immobile"])
		sleep (1);
		return 0;
	endif

	var myoldx := me.x;
	var myoldy := me.y;
	
	for i := 1 to maxruns
		RunTowardLocation (x, y);
		if (x == me.x and y == me.y or GetObjProperty (me, "#justgated"))
			return;
		endif
		
		//This is in case we went through a teleporter
		if (CoorDist (me.x, me.y, myoldx, myoldy) > 20)
			return;
		endif
	endfor
endfunction




///////////////////
//  Running straight hasn't gotten us any closer to the player, so try running in some direction or other
///////////////////

function RunRandom (byref opponent, numruns := 2)
	if (me.frozen or me.paralyzed)
		sleep (1);
		return 0;
	elseif (me.script["immobile"])
		sleep (1);
		return 0;
	endif

	var stepstotake := (numruns)+4;
	if (numruns > 3)
		RunAwayFrom (opponent);
	endif

	var tox := me.x;
	var toy := me.y;

	var highx := tox - opponent.x;
	if ( highx < 0 )
		highx := 0 - highx;
	endif
	var highy := toy - opponent.y;
	if ( highy < 0 )
		highy := 0 - highy;
	endif

	if ( highy > highx )	//we want to run along the x access
		if (!CheckLOSAt (me, (me.x - stepstotake), me.y, (GetMapInfo ((me.x - stepstotake), me.y, me.realm).z)))
			tox := (me.x + stepstotake);
		elseif (!CheckLOSAt (me, (me.x + stepstotake), me.y, (GetMapInfo ((me.x + stepstotake), me.y, me.realm).z)))
			tox := (me.x - stepstotake);
		elseif (RandomInt (2) )	//to the right
			tox := (me.x - stepstotake);
		else		//to the left
			tox := (me.x + stepstotake);
		endif
		if (randomint(2))
			toy := opponent.y;
		endif
	else		//run along the y access
		if (!CheckLOSAt (me, me.x, (me.y - stepstotake), (GetMapInfo (me.x, (me.y - stepstotake), me.realm).z)))
			toy := (me.y + stepstotake);
		elseif (!CheckLOSAt (me, me.x, (me.y + stepstotake), (GetMapInfo (me.x, (me.y + stepstotake), me.realm).z)))
			toy := (me.y - stepstotake);
		elseif (RandomInt (2) )	//run to the right
			toy := (me.y - stepstotake);
		else		//run to the left
			toy := (me.y + stepstotake);
		endif
		if (randomint(2))
			tox := opponent.x;
		endif
	endif

	var runs := 0;
	var ev;
	while ( (distance(me, opponent) > 1) and (runs <= stepstotake) )
		RunTowardLocation (tox, toy);
		in_combat_event_loop(opponent);
		runs := runs + 1;
		ev := wait_for_event(0);
		if (ev)
			case (ev.type)
			EVID_PEACEMADE:
				return 1;
			EVID_DAMAGED:
				process_combat_event (ev);
				if (ev.source.serial == opponent.serial)
					Flee (opponent);
				endif
			endcase
		endif
		if ( (me.x == tox) and (me.y == toy) )
			return 0;
		endif
		if (CheckLOSAt (me, opponent.x, opponent.y, opponent.z) )
			return 0;
		endif
	endwhile

	return 0;
endfunction




///////////////////
//  Flee from our opponent
///////////////////

function flee (byref opponent)
	if ( me.script["immobile"] or (me.frozen) or (me.paralyzed) )
		sleep (1);
		return;
	endif

	var numflees := getobjproperty (me, "#flees");
	if (numflees)
		if ( numflees > 3 )
			KillMe ();
		else
			numflees := numflees + 1;
		endif
	else
		numflees := 1;
	endif

	setobjproperty (me, "#flees", numflees);

	var runs := 0;
	var ev;
	while ( (distance(me, opponent) < 25) and (runs < 50) )
		if (GetVital (me, "Life") < CINT (GetVitalMaximumValue (me, "Life")/10) )
			WalkAwayFrom (opponent);
		else
			RunAwayFrom (opponent);
		endif
		in_combat_event_loop (opponent);
		runs := runs + 1;
		ev := wait_for_event(0);
		if (ev and ev.type == EVID_DAMAGED)
			process_combat_event (ev);
		endif
	endwhile

	RestartScript ( me );

endfunction



