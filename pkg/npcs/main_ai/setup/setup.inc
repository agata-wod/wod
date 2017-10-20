use uo;
use os;
use npc;
use util;
use cfgfile;

include "include/client";
include "include/objtype";
include "include/effects";
include "include/eventid";
include "include/sysevent";
include "include/string";
include "include/utility";
include "include/dotempmods";
include "include/attributes";
include "../pkg/npcs/npc_util";
include "../pkg/npcs/npcinfo";
include "../pkg/npcs/main_ai/combat/pathfinding";
include "../pkg/npcs/main_ai/combat/spellcasting";

var me := Self();
var next_look_around;

///////////////////
//  Called whenever the NPC is started or restarted
///////////////////

function DoStartupSetup ()
	var MyTemplateElem := GetNpcTemplateElem (me.npctemplate);
	if (!MyTemplateElem)
		KillMe ();
	endif

	if (MyTemplateElem.rank)
		var rank := CINT (MyTemplateElem.rank);
		SetObjProperty (me, "#rank", rank);
	endif
endfunction




///////////////////
//  called when the NPC is first created
///////////////////

function DoOnCreationSetup ()
	start_script (":npcs:npc_setup", me);
	
	//The NPC will normally try to walk around a bit when created to make sure they
	//weren't spawned somewhere where they'd be stuck, but skip that if there's a player
	//around
	foreach mobile in ListMobilesNearLocation (me.x, me.y, me.z, 10, me.realm)
		if (mobile.acctname)
			return;
		endif
	endforeach

	var myx := me.x;
	var myy := me.y;
	var myz := me.z;
	var myr := me.realm;

	if (!me.frozen and !me.paralyzed)
		var old_runto_speed := me.run_speed;
		if (!old_runto_speed)
			old_runto_speed := GetAttribute (me, "Dexterity");
		endif
		TurnTowardLocation (me.x, me.y-1);
		Move ("N");
		if (me.x == myx and me.y == myy)
			TurnTowardLocation (me.x, me.y+1);
			Move ("S");
			if (me.x == myx and me.y == myy)
				TurnTowardLocation (me.x+1, me.y);
				Move ("E");
				if (me.x == myx and me.y == myy)
					TurnTowardLocation (me.x-1, me.y);
					Move ("W");
					if (me.x == myx and me.y == myy)
						if (!me.frozen)
							KillMe ();
							return;
						endif
					endif
				endif
			endif
		endif
		me.run_speed := old_runto_speed;
		MoveObjectToLocation( me, myx, myy, myz, myr, MOVEOBJECT_FORCELOCATION );
	endif
endfunction
