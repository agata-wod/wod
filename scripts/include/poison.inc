include "include/buffdebuff";

///////////////////
//  New poison system, rewritten for POL 0.94.  Poison used to be a subset of the dotempmods functions
//  and relied on regen rates to work.  It now used the /pkg/skills/rogueskills/poisoning/poisondamage.src
//  script to do the damage.  This include function is the (hopefully) convenient front-end for the poison
//  system
//
//  Format for the #poisoninfo cprop:
//	[1] - poison level
//	[2] - time the poisoning should end
//	[3] - PID of poison damage script
///////////////////




function SetPoison (byref who, plvl, msg := "")
	//:poisoning:poisondamage is called, which does all the
	//work.  This is so we can tweak poisondamage
	//on-the-fly, and only have to recompile the one script

	var parms := array{};
	parms[1] := who;
	parms[2] := plvl;
	parms[3] := msg;
	start_script (":poisoning:poisondamage", parms);
endfunction




///////////////////
//  Cures the poison
///////////////////

function CurePoison (byref who, sound := 0, msg := "")
	EraseObjProperty (who, "#poisoninfo");
	who.SetPoisoned (0);
	BuffDebuffIcon(who, BUFFICON_POISON, BUFFSTATE_DISABLED);
	if (sound)
		PlaySoundEffect (who, sound);
	endif
	if (msg)
		PrintTextAbovePrivate (who, msg, who);
	endif
endfunction




///////////////////
//  if we're currently poisoned, figures out how poisoned.
///////////////////

function GetPoisonLevel (byref who)
	var poisoned := GetObjProperty (who, "#poisoninfo");
	if (!poisoned)
		return 0;
	endif
	
	return (poisoned [1]);
endfunction
