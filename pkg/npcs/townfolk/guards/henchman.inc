Const MAX_HENCHMEN_PER_ACCOUNT	:= 20;
Const MAX_HENCHMEN_AT_A_TIME		:= 2;

include "include/client";
include "include/objtype";
include "include/dotempmods";
include "include/attributes";
include "include/utility";
include "include/eventid";
include "include/sysevent";
include "../pkg/npcs/npc_util";
include "../pkg/npcs/npcinfo";
include "../pkg/npcs/speech";
include "../pkg/npcs/main_ai/combat/pathfinding";


///////////////////
//  counts how many henchmen a particular account has
///////////////////

function AddToAccountHenchmen (masterserial)
	set_critical (1);
	var mymaster := SystemFindObjectBySerial (masterserial, SYSFIND_SEARCH_OFFLINE_MOBILES );
	if (!mymaster)
		set_critical(0);
		KillMe ();
		return;
	endif
	me.setmaster (mymaster);
	me.title_race := mymaster.name;

	var acctname := mymaster.acctname;
	if (!acctname)
		set_critical (0);
		return 0;
	endif

	var henchmen := GetGlobalProperty ("#henchmen_of_"+acctname);
	if (!henchmen)
		henchmen := array {};
	endif

	if (me.serial in henchmen)
		set_critical (0);
		return 1;
	endif
	henchmen.append (me.serial);
	SetGlobalProperty ("#henchmen_of_"+acctname, henchmen);
	set_critical(0);
	return 1;
endfunction




///////////////////
//  counts how many henchmen a particular account has
///////////////////

function CountAccountHenchmen (masterserial)
	var mymaster := SystemFindObjectBySerial (masterserial, SYSFIND_SEARCH_OFFLINE_MOBILES );
	if (!mymaster)
		return 0;
	endif

	var acctname := mymaster.acctname;
	if (!acctname)
		return 0;
	endif

	var henchmen := GetGlobalProperty ("#henchmen_of_"+acctname);
	if (!henchmen)
		return 0;
	endif

	var newhenchmen := array {};
	foreach henchserial in henchmen
		var thehenchman := SystemFindObjectBySerial (henchserial);
		if (thehenchman)
			newhenchmen.append (henchserial);
		endif
	endforeach
	SetGlobalProperty ("#henchmen_of_"+acctname, newhenchmen);
	return  (len (newhenchmen));
endfunction




///////////////////
//  counts the number of henchmen in the area that the player controls, and quits if there's too many
///////////////////

function AboveHenchCount (countme := 1)
	set_critical(1);
	var men := 0;
	foreach mobile in ListMobilesNearLocation (me.x, me.y, me.z, 8, me.realm)
		if (mobile.script[":guards:henchman"])
			if (GetObjProperty (mobile, "master") == masterserial)
				if (GetObjProperty (mobile, "#following") )
					if (countme)
						men := men + 1;
					elseif (me != mobile)
						men := men + 1;
					endif
				endif
			endif
		endif
	endforeach
	set_critical(0);

	if (men > MAX_HENCHMEN_AT_A_TIME)
		return 1;
	else
		return 0;
	endif
endfunction




///////////////////
//  counts the number of henchmen in the area that the player controls, and quits if there's too many
///////////////////

function BelowHenchCount (countme := 1)
	set_critical(1);
	var men := 0;
	foreach mobile in ListMobilesNearLocation (me.x, me.y, me.z, 8, me.realm)
		if (mobile.script[":guards:henchman"])
			if (GetObjProperty (mobile, "master") == masterserial)
				if (GetObjProperty (mobile, "#following") )
					if (countme)
						men := men + 1;
					elseif (me != mobile)
						men := men + 1;
					endif
				endif
			endif
		endif
	endforeach
	set_critical(0);

	if (men < MAX_HENCHMEN_AT_A_TIME)
		return 1;
	else
		return 0;
	endif
endfunction




///////////////////
//A bunch of different ways to say ok
///////////////////

function SayOkay()
	if ( randomint(5)!=1 )
		say("Okay");
		return;
	endif

	var samegender := ( master.gender == me.gender );
	var msg := "Okay";
	case ( RandomInt(24) )
	0: msg := "Yes, Oh great one";
	1: msg := "Your wish is my command";
	2: msg := "Whatever you say";
	3: msg := "Of course";
	4: msg := "As you wish";
	5: msg := "My pleasure";
	6: msg := "You bet";
	7: msg := "k";
	8: msg := "Sure thing";
	9: msg := "np";
	10: msg := "You da boss";
	11: msg := "Okay, but you can rez', right?";
	12: msg := "Surely";
	13: msg := "Alrighty";
	14: msg := "If you promise to keep me alive";
	15: 
		msg := "Sir! Yes, Sir!";
		if ( master.gender ) msg := "Yes, Ma'am!"; endif
	16: 
		if (!samegender )
			msg := "You are such a hottie";
		elseif (me.gender)
			msg := "Let's go find some cute guys";
		else
			msg := "Where do the chicks hang out?";
		endif
	17: msg := "I joined the army for THIS?";
	18: msg := "*mumble* bossy *mumble*";
	19: msg := "Uh... well, ok";
	20: msg := "If you think I should";
	21: msg := "Will do";
	22: msg := "Huh?  Oh.  Okay";
	23: 
		if ( master.gender and !samegender )
			msg := "Yes, dear";
		elseif (!samegender)
			msg := "Okay, sweety";
		else
			msg := "Okay";
		endif
	default: msg := "Okay";
	endcase
	say (msg);
endfunction




///////////////////
//  Attempts to resurrect the warrior
///////////////////

function ResurrectHenchman (byref corpse)
	set_critical (1);
	var master_serial := GetObjProperty (corpse, "master");
	if (!master_serial)
		set_critical (0);
		return 0;
	endif

	var mymaster := SystemFindObjectBySerial (master_serial, SYSFIND_SEARCH_OFFLINE_MOBILES );
	if (!mymaster)
		set_critical(0);
		return 0;
	endif

	var rescount := GetObjProperty (corpse, "rescount");
	if (!rescount)
		rescount := 0;
	endif

	if (rescount > GetAttribute (mymaster, ATTRIBUTEID_LEADERSHIP))
		set_critical(0);
		return 0;
	endif

	var parms := struct {};
	parms.+script := ":guards:henchman";
	if (GetObjProperty (corpse, "myscript") )
		parms.script := GetObjProperty (corpse, "myscript");
	endif

	var new_npc := CreateNPCFromTemplate (":guards:warrior", corpse.x, corpse.y, corpse.z, parms, corpse.realm);
	if (!new_npc)
		set_critical(0);
		return 0;
	endif

	SetObjProperty (new_npc, "master", master_serial);
	SetVital (new_npc, "Life", 100);
	new_npc.gender := GetObjProperty (corpse, "gender");
	new_npc.color := GetObjProperty (corpse, "color");
  new_npc.race := GetObjProperty (corpse, "race");
  case (new_npc.race)
    RACE_ELF: new_npc.graphic := 605 + new_npc.gender;
    RACE_GARGOYLE: new_npc.graphic := 666 + new_npc.gender;
    default: new_npc.graphic := 400 + new_npc.gender;
  endcase
	SetObjProperty (new_npc, "gender", new_npc.gender);
	SetObjProperty (new_npc, "color", new_npc.color);
	SetObjProperty (new_npc, "race", new_npc.race);
	if (GetObjProperty (corpse, "virtue") )
		var virtue := CINT (0.95 * GetObjProperty (corpse, "virtue"));
		SetObjProperty (new_npc, "virtue", virtue);
	endif

	var temp_string := corpse.desc;
	temp_string["A corpse of "] := "";
	new_npc.name := temp_string;
	SetObjProperty (new_npc, "rescount", rescount + 1);

	var newbackpack := CreateItemAtLocation (5288 , 1176 , 0, UOBJ_BACKPACK, 1, REALM_BRITANNIA);
	if (!EquipItem (new_npc, newbackpack))
		DestroyItem (newbackpack);
	endif

	var leadership := GetAttribute (mymaster, ATTRIBUTEID_LEADERSHIP);
	var skills := GetObjProperty (corpse, "skills");
	if (!skills)
		skills := array {};
	endif
	
	for i := 0 to 57
		var skill := skills[i];
		if (!skill)
			skill := 0;
		endif
		if (GetAttribute (mymaster, GetAttributeIDBySkillID(i)) > skill)
			skill := GetAttribute (mymaster, GetAttributeIDBySkillID(i));
		endif
		
		if (skill > leadership + 20)
			skill := leadership + 20;
		endif
		SetAttributeBaseValue (new_npc, GetAttributeIDBySkillID(i), skill*10);
	endfor

	var stats := GetObjProperty (corpse, "stats");
	if (stats)
		SetAttributeBaseValue (new_npc, "Strength", stats[1] * 10);
		SetAttributeBaseValue (new_npc, "Intelligence", stats[2] * 10);
		SetAttributeBaseValue (new_npc, "Dexterity", stats[3] * 10);
	endif

	foreach item in ListRootItemsInContainer (corpse)
		if (!EquipItem (new_npc, item))
			MoveItemToContainer (item, new_npc.backpack);
		endif
	endforeach

	if (GetObjProperty (corpse, "hairstyle"))
		var hairstyle := GetObjProperty (corpse, "hairstyle");
		if (hairstyle[1])
			var hair := CreateItemAtLocation (5288 , 1176 , 0, CINT (hairstyle[1]), 1, REALM_BRITANNIA);
			hair.color := hairstyle[2];
			if (!EquipItem (new_npc, hair))
				DestroyItem (hair);
			endif
		endif
		if (hairstyle[3])
			var beard := CreateItemAtLocation (5288 , 1176 , 0, CINT (hairstyle[3]), 1, REALM_BRITANNIA);
			beard.color := hairstyle[4];
			if (!EquipItem (new_npc, beard))
				DestroyItem (beard);
			endif
		endif
	endif

	DestroyItem (corpse);
	set_critical (0);
	return 1;
endfunction




///////////////////
//records the henchperson's stats so that, if they die, they can be resurrected intact.
///////////////////

function RecordStats ()
	var basestr := CINT (GetAttributeBaseValue (me, "Strength")/10);
	var baseint := CINT (GetAttributeBaseValue (me, "Intelligence")/10);
	var basedex := CINT (GetAttributeBaseValue (me, "Dexterity")/10);

	var stats := array;
	stats[1] := basestr;
	stats[2] := baseint;
	stats[3] := basedex;
	SetObjProperty (me, "stats", stats);

	var hairstyle := array;
	var myhair := GetEquipmentByLayer (me, LAYER_HAIR);
	if (myhair)
		hairstyle[1] := myhair.objtype;
		hairstyle[2] := myhair.color;
	else
		hairstyle[1] := 0;
		hairstyle[2] := 0;
	endif

	var mybeard := GetEquipmentByLayer (me, LAYER_BEARD);
	if (mybeard)
		hairstyle[3] := mybeard.objtype;
		hairstyle[4] := mybeard.color;
	else
		hairstyle[3] := 0;
		hairstyle[4] := 0;
	endif

	SetObjProperty (me, "hairstyle", hairstyle);
endfunction




///////////////////
//  warrior tries to use a bandage or potion
///////////////////

function ApplyHenchmanHealing (byref who)
	if (GetVital (me, "Life") < CINT (GetVitalMaximumValue (me, "Life")/2))
		if (GetVital (me, "Life") < 3000)
			DrinkHeal ();
			return;
		elseif (me.poisoned)
			DrinkCure ();
		endif
	endif

	if (GetObjProperty (me, "#heal") and GetObjProperty (me, "#heal") > readgameclock())
        	return 0;
	endif

	if (distance (me, who) > 3)
		say (who.name + " is too far away!");
		return 1;
	endif

	if (me.poisoned)
		if (!GetObjProperty (me, "#spammed"))
			say ("I'm poisoned!");
			SetObjProperty (me, "#spammed", 1);
		endif

		var difficulty := 40 + GetPoisonLevel (who)*10;
		if (checkskill (me, SKILLID_HEALING, difficulty, 0))
			CurePoison (who, 0, who.name + " has CURED you!");
			if (who == me)
				say ("*Whew!*  I'm cured!");
			endif
			SetObjProperty (me, "#heal", ReadGameClock() + 10);
			return 1;
		endif
	else
		EraseObjProperty (me, "#spammed");
	endif

	if (me.poisoned)
		DrinkCure ();
	endif

	var parms := array { };
	parms[1] := me;
	parms[2] := who;

	foreach item in EnumerateItemsInContainer (me.backpack)
		if ( item.objtype == UOBJ_BANDAGE)
			parms[3] := item;
			start_script (":healing:dohealing",parms);
			return 1;
		endif
	endforeach

	//this isn't needed for the healing script.  Instead it just makes the warrior shut up if
	//he's out of bandages and hurt
	SetObjProperty (me, "#heal", ReadGameClock() + 10);
	Say ("I don't have any bandages!");
	return 1;
endfunction




///////////////////
//  If we're really low on HP, try to drink a heal potion
///////////////////

function DrinkHeal ()
	if (CINT (GetObjProperty (me, "#potion")) > ReadGameClock()  )
        	return 0;
	endif

	foreach item in EnumerateItemsInContainer (me.backpack)
		if (item.objtype == UOBJ_HEAL_POTION or item.objtype == UOBJ_GREATER_HEAL_POTION)
			PlaySoundEffect (me, SFX_DRINK);
			PerformAction (me, ANIM_EAT);
			CreateItemInBackpack (me, UOBJ_EMPTY_BOTTLE, 1);
			var heal_amount;
			if (item.objtype == 0xDC02)
				heal_amount := RandomDiceRoll ("3d12+3");
			else
				heal_amount := RandomDiceRoll ("5d12+3");
			endif			
			HealDamage (me, heal_amount);
			SubtractAmount (item, 1);
			PlaySoundEffect (me,  SFX_HEAL);
			SetObjProperty (me, "#potion", (ReadGameClock() + 20));
			return 1;
		endif
	endforeach

	return 0;
endfunction




///////////////////
//  If we're really low on HP, try to drink a heal potion
///////////////////

function DrinkCure ()
	if (Cint (GetObjProperty (me, "#potion")) > ReadGameClock()  )
        	return 0;
	endif

	foreach item in EnumerateItemsInContainer (me.backpack)
		if (item.objtype == UOBJ_CURE_POTION or item.objtype == UOBJ_GREATER_CURE_POTION)
			PlaySoundEffect (me, SFX_DRINK);
			PerformAction (me, ANIM_EAT);
			CreateItemInBackpack (me, UOBJ_EMPTY_BOTTLE, 1);
			if (item.objtype == UOBJ_CURE_POTION)
				if (GetPoisonLevel (me) <= 7 or RandomInt (2) )
					CurePoison (me, SFX_CURE, 0);
				endif
			else
				CurePoison (me, SFX_CURE, 0);
			endif			
			SubtractAmount (item, 1);
			SetObjProperty (me, "#potion", (ReadGameClock() + 20));
			return 1;
		endif
	endforeach

	return 0;
endfunction




///////////////////
//  Try to resurrect the target
///////////////////

function ResurrectTarget (deadguy)
	if (!deadguy.dead)
		Say ("But you're not even dead!");
		return;
	endif

	if (distance (me, deadguy) > 3)
		say ("What, you want me to walk all the way over THERE?");
		return 1;
	endif

	if (!CheckLoSAt (me, deadguy.x, deadguy.y, deadguy.z))
		say ("Sorry, can't see you.");
		return 1;
	endif

	var parms := array { };
	parms[1] := me;
	parms[2] := deadguy;
	foreach item in EnumerateItemsInContainer (me.backpack)
		if ( item.objtype == UOBJ_BANDAGE)
			parms[3] := item;
			start_script (":healing:dohealing",parms);
			return 1;
		endif
	endforeach
	Say ("Sorry, no bandages.");

endfunction


