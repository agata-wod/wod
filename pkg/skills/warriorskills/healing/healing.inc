include "include/spawn";
include "include/poison";
include "../pkg/npcs/townfolk/guards/henchman";

///////////////////
//  tries to heal the patient
///////////////////

function HealPatient (healer, patient, bandages)
	if (Distance (healer, patient) > 2)
		SendSysMessage (healer, "The patient is too far away.", FONT_NORMAL, MESSAGE_COLOR_BAD);
		EraseObjProperty (healer, "#heal");
		return;
	endif

	var healer_healing := GetAttribute (healer, ATTRIBUTEID_HEALING);
	var healer_anatomy := GetAttribute (healer, ATTRIBUTEID_ANATOMY);
	var ignore_range := healer_healing/20;
	var bandaging_time := 20 - CINT(healer_anatomy/15);
	var heal_dice := DetermineHealDice (healer_healing, healer_anatomy);

	SendSysmessage (healer, "You begin applying a bandage...");
	if (patient != healer)
		SendSysMessage (patient, healer.name + " begins applying a bandage");
		bandaging_time := bandaging_time - 5;
	endif

	var DoneHealing := ReadGameClock() + bandaging_time;
	SetObjProperty (healer, "#heal", DoneHealing);
	detach();

	var starthp := GetVital (healer, "Life");
	var healer_dexterity := GetAttribute (healer, "Dexterity");
	var difficulty_mod := 0;

	repeat
		if (GetVital (healer, "Life") < CINT((starthp*(100-ignore_range))/100))
			//try to save based on dex
			if (RandomInt (100) > CINT (healer_dexterity/2) )
				SendSysMessage (healer, "Your fingers slip!", FONT_NORMAL, MESSAGE_COLOR_WARNING);
				difficulty_mod := difficulty_mod + 5;
			endif
			starthp := GetVital (healer, "Life");
		endif
		if (!patient or !healer)
			SendSysmessage (healer, "Now where did he go...", FONT_NORMAL, MESSAGE_COLOR_BAD);
			EraseObjProperty (healer, "#heal");
			return;
		elseif (patient.dead)
			SendSysMessage (healer, "You need to be a bit faster, next time.", FONT_NORMAL, MESSAGE_COLOR_BAD);
			EraseObjProperty (healer, "#heal");
			return;
		endif
		sleepms(250);
	until (ReadGameClock() >= DoneHealing);

	if (Distance (healer, patient) > 2)
		if (healer.npctemplate[":guards:"])
			PrintTextAbove (healer, "Argh, "+patient.name+" is too far away to heal!");
		else
			SendSysmessage (healer, "The patient is too far away", FONT_NORMAL, MESSAGE_COLOR_BAD);
		endif
		EraseObjProperty (healer, "#heal");
		return;
	endif

	if (GetVital (patient, "Life") >= GetVitalMaximumValue (patient, "Life"))
		SendSysMessage (healer, "The patient seems to be quite all right.", FONT_NORMAL, MESSAGE_COLOR_GOOD);
		EraseObjProperty (healer, "#heal");
		return;
	endif

	var difficulty := CINT (100 - ((GetVital (patient, "Life")*100)/GetVitalMaximumValue (patient, "Life")));

	difficulty := difficulty + difficulty_mod;
	if (difficulty > 100)
		difficulty := 100;
	endif

//	if (patient.acctname or patient.master)
//		difficulty := CINT (difficulty / 2);
//	endif
	SubtractAmount (bandages, 1);

	var waspoisoned := 0;
	if (patient.poisoned)
		var poison_difficulty := 50 + GetPoisonLevel (patient)*5;
		if (CheckSkill (healer, SKILLID_HEALING, poison_difficulty, 0) )
			if (healer != patient)
				SendSysMessage (healer, "You have cured " + patient.name, FONT_NORMAL, MESSAGE_COLOR_GOOD);
				SendSysMessage (patient, healer.name + " has cured you!", FONT_NORMAL, MESSAGE_COLOR_GOOD);
			else
				SendSysMessage(healer,"You have cured yourself!", FONT_NORMAL, MESSAGE_COLOR_GOOD);
			endif
			CurePoison (patient, 0, 0);
			waspoisoned := 1;
		else
			if (healer != patient)
				SendSysMessage (healer, "You have FAILED to cure " + patient.name, FONT_NORMAL, MESSAGE_COLOR_BAD);
				SendSysMessage (patient, healer.name + " has FAILED to cure you!", FONT_NORMAL, MESSAGE_COLOR_BAD);
			else
				SendSysMessage (healer,"You have FAILED to cure yourself!", FONT_NORMAL, MESSAGE_COLOR_BAD);
			endif
		endif		
	endif

	if (CheckSkill (healer, SKILLID_HEALING, difficulty, 0) )
		var points_healed := RandomDiceRoll (heal_dice);
		if (waspoisoned)
			points_healed := CINT (points_healed * 2 / 3);
		endif
		var patient_wounds := GetVitalMaximumValue (patient, "Life") - GetVital (patient, "Life");
		patient_wounds := CINT (patient_wounds/100);
		if (points_healed > patient_wounds)
			points_healed := patient_wounds;
		endif
		
		HealDamage (patient, points_healed);
		SendSysMessage (healer, "You have healed " + points_healed + " hit points.", FONT_NORMAL, MESSAGE_COLOR_GOOD);
		if (patient != healer)
			SendSysMessage (patient, healer.name + " has healed " + points_healed + " hit points.", FONT_NORMAL, MESSAGE_COLOR_GOOD);
		endif
	else
		SendSysMessage (healer, "You were unsuccessful at binding the wound.", FONT_NORMAL, MESSAGE_COLOR_BAD);
		if (patient != healer)
			SendSysMessage (patient, healer.name + " was unable to bind your wound.", FONT_NORMAL, MESSAGE_COLOR_BAD);
		endif
	endif
	EraseObjProperty (healer, "#heal");
endfunction




///////////////////
//  Attempts to resurrect the chosen player
///////////////////

function ResurrectPlayer (healer, patient, bandages)
	var healer_healing := GetAttribute (healer, ATTRIBUTEID_HEALING);
	var healer_anatomy := GetAttribute (healer, ATTRIBUTEID_ANATOMY);
	var res_chance := DetermineResChance (healer_healing, healer_anatomy);
	if ( !res_chance )
		SendSysMessage (healer, "You lack the skill to resurrect the dead.", FONT_NORMAL, MESSAGE_COLOR_BAD);
		if (healer.npctemplate)
			PrintTextAbove (healer, "Sorry, I don't know how to do that.");
		endif
		EraseObjProperty (healer, "#heal");
		return;
	endif

	if (Distance (healer, patient) > 2)
		SendSysMessage (healer, "The patient is too far away.", FONT_NORMAL, MESSAGE_COLOR_BAD);
		EraseObjProperty (healer, "#heal");
		return;
	endif

	SendSysMessage (healer, "You begin applying a bandage...");
	SendSysMessage (patient, healer.name + " attempts to resurrect you...");
	var wait_time := 15 - CINT (healer_anatomy/20);
	sleep (wait_time);

	if (Distance (healer, patient) > 2)
		SendSysMessage (healer, "The patient is too far away.", FONT_NORMAL, MESSAGE_COLOR_BAD);
		EraseObjProperty (healer, "#heal");
		return;
	endif

	SubtractAmount (bandages, 1);
	if (!patient.dead)
		SendSysmessage (healer, "Guess he's not dead after all...", FONT_NORMAL, MESSAGE_COLOR_GOOD);
		EraseObjProperty (healer, "#heal");
		return;
	endif

	if (RandomInt (100) <= res_chance)
		if (!yesnores (patient, "Live Again?"))
			SendSysMessage (healer, patient.name + " shuns your help, prefering to remain in the spirit world.", FONT_NORMAL, MESSAGE_COLOR_WARNING);
			EraseObjProperty (healer, "#heal");
			return;
		else
			Resurrect (patient);
			SendSysmessage (healer, "You have resurrected the patient.", FONT_NORMAL, MESSAGE_COLOR_GOOD);
			AwardVirtueForRes (healer, patient);
		endif
	else
		SendSysmessage (healer, "You were unsuccessful at returning the patient to this world.", FONT_NORMAL, MESSAGE_COLOR_BAD);
		SendSysmessage (patient, healer.name + " fails to resurrect you.", FONT_NORMAL, MESSAGE_COLOR_BAD);
		if (healer.npctemplate)
			case (RandomInt (5))
				0:
					PrintTextAbove (healer, "Dang, this is hard.");
				1:
					PrintTextAbove (healer, "What do I look like, a doctor?");
				2:
					PrintTextAbove (healer, "So, ready to give up and ask for help yet?");
				3:
					PrintTextAbove (healer, "Mom always told me I should have went to medical school.");
				default:
					PrintTextAbove (healer, "Leg bone connected to the...  Um...  jaw bone?  No, that's not it...");
			endcase
		endif
	endif
	EraseObjProperty (healer, "#heal");
endfunction




///////////////////
//  Attempts to resurrect the chosen NPC based on the corpse
///////////////////

function TryToResurrectHenchman (healer, corpse, bandages)
	var master_serial := GetObjProperty (corpse, "master");
	if (!master_serial)
		SendSysMessage (healer, "You can't resurrect that!");
		EraseObjProperty (healer, "#heal");
		return;
	endif

	var master := SystemFindObjectBySerial (master_serial);
	if (!master)
		SendSysMessage (healer, "You can't resurrect that!");
		EraseObjProperty (healer, "#heal");
		return;
	endif

	var healer_healing := GetAttribute (healer, ATTRIBUTEID_HEALING);
	var healer_anatomy := GetAttribute (healer, ATTRIBUTEID_ANATOMY);
	var res_chance := DetermineResChance (healer_healing, healer_anatomy);
	if (!res_chance)
		SendSysMessage (healer, "You lack the skill to resurrect the dead.", FONT_NORMAL, MESSAGE_COLOR_BAD);
		EraseObjProperty (healer, "#heal");
		return;
	endif

	if (Distance (healer, corpse) > 2)
		SendSysMessage (healer, "The patient is too far away.", FONT_NORMAL, MESSAGE_COLOR_BAD);
		EraseObjProperty (healer, "#heal");
		return;
	endif

	SendSysMessage (healer, "You begin applying a bandage...");
	var wait_time := 15 - CINT (healer_anatomy/20);
	sleep (wait_time);

	if (Distance (healer, corpse) > 2)
		SendSysMessage (healer, "The patient is too far away.", FONT_NORMAL, MESSAGE_COLOR_BAD);
		EraseObjProperty (healer, "#heal");
		return;
	endif

	SubtractAmount (bandages, 1);

	if (RandomInt (100) <= res_chance)
		if (!ResurrectHenchman (corpse))
			SendSysMessage (healer, "You cannot resurrect the patient");
		endif
	else
		SendSysmessage (healer, "You were unsuccessful at returning the patient to this world.", FONT_NORMAL, MESSAGE_COLOR_BAD);
 	endif
	EraseObjProperty (healer, "#heal");
endfunction




///////////////////
//  Attempts to resurrect the chosen NPC based on the corpse
///////////////////

function ResurrectPet (healer, corpse, bandages)
	var npctemplate := GetObjProperty (corpse, "npctemplate");
	if (!npctemplate)
		SendSysMessage (healer, "You can't resurrect that!");
		EraseObjProperty (healer, "#heal");
		return;
	endif

	var master_serial := GetObjProperty (corpse, "master");
	if (!master_serial)
		SendSysMessage (healer, "You can't resurrect that!");
		EraseObjProperty (healer, "#heal");
		return;
	endif

	var master := SystemFindObjectBySerial (master_serial);
	if (!master)
		SendSysMessage (healer, "You can't resurrect that!");
		EraseObjProperty (healer, "#heal");
		return;
	endif

	var healer_healing := GetAttribute (healer, ATTRIBUTEID_HEALING);
	var healer_anatomy := GetAttribute (healer, ATTRIBUTEID_ANATOMY);
	var res_chance := DetermineResChance (healer_healing, healer_anatomy);
	if (!res_chance)
		SendSysMessage (healer, "You lack the skill to resurrect the dead.", FONT_NORMAL, MESSAGE_COLOR_BAD);
		EraseObjProperty (healer, "#heal");
		return;
	endif

	if (Distance (healer, corpse) > 2)
		SendSysMessage (healer, "The patient is too far away.", FONT_NORMAL, MESSAGE_COLOR_BAD);
		EraseObjProperty (healer, "#heal");
		return;
	endif

	SendSysMessage (healer, "You begin applying a bandage...");
	var wait_time := 15 - CInt (healer_anatomy/20);
	sleep (wait_time);

	if (Distance (healer, corpse) > 2)
		SendSysMessage (healer, "The patient is too far away.", FONT_NORMAL, MESSAGE_COLOR_BAD);
		EraseObjProperty (healer, "#heal");
		return;
	endif

	SubtractAmount (bandages, 1);
	if (! (RandomInt (100) <= res_chance))
		SendSysmessage (healer, "You were unsuccessful at returning the patient to this world.", FONT_NORMAL, MESSAGE_COLOR_BAD);
		EraseObjProperty (healer, "#heal");
		return;
	endif

	var rescount := GetObjProperty (corpse, "rescount");
	if (!rescount)
		rescount := 0;
	endif

	if (rescount * 10 > GetAttribute (master, ATTRIBUTEID_TAMING))
		SendSysMessage (healer, "It seems to have gone to the big pet shop in the sky...");
		EraseObjProperty (healer, "#heal");
		return;
	endif

	set_critical (1);
	var new_npc := SpawnNPCAtLocation (npctemplate, corpse.x, corpse.y, corpse.z, 0, corpse.realm);
	if (!new_npc)
		SendSysMessage (healer, "It seems to have gone to the big pet shop in the sky...");
		EraseObjProperty (healer, "#heal");
		return;
	endif

	var newbackpack := CreateItemAtLocation (5288 , 1176 , 0, UOBJ_BACKPACK, 1, REALM_BRITANNIA);
	if (!EquipItem (new_npc, newbackpack))
		DestroyItem (newbackpack);
	endif

	foreach item in ListRootItemsInContainer (corpse)
		if (!equipItem (new_npc, item))
			MoveItemToContainer (item, new_npc.backpack);
		endif
	endforeach

	if (GetObjProperty (corpse, "color") )
		new_npc.color := GetObjProperty (corpse, "color");
		SetObjProperty (new_npc, "color", GetObjProperty (corpse, "color"));
	endif

	SetObjProperty (new_npc, "master", master.serial);
	new_npc.script := "::tamed";
	RestartScript (new_npc);

	var skills := GetObjProperty (corpse, "skills");
	if (skills)
		for i := 0 to 57
			var skill := skills[i];
			if (!skill)
				skill := 0;
			endif
			SetAttributeBaseValue (new_npc, GetAttributeIDBySkillID(i), skill*10);
		endfor
		SetObjProperty (new_npc, "skills", skills);
	endif
	
	var stats := GetObjProperty (corpse, "stats");
	if (stats)
		SetAttributeBaseValue (new_npc, "Strength", stats[1] * 10);
		SetAttributeBaseValue (new_npc, "Intelligence", stats[2] * 10);
		SetAttributeBaseValue (new_npc, "Dexterity", stats[3] * 10);
		SetObjProperty (new_npc, "stats", stats);
		RecalcVitals (new_npc);
	endif

	SetVital (new_npc, "Life", 100);

	var temp_string := corpse.desc;
	temp_string["A corpse of "] := "";
	new_npc.name := temp_string;
	SetObjProperty (new_npc, "rescount", rescount + 1);

	DestroyItem (corpse);
	set_critical (0);
	EraseObjProperty (healer, "#heal");
endfunction




///////////////////
//  determines the healers chance of successfully resurrecting the patient, based on their
//  healing and anatomy skill
///////////////////

function DetermineResChance (healer_healing, healer_anatomy)
	var res_chance := 0;
	var skill_level := CINT (healer_healing/10 + healer_anatomy/20);
	if (skill_level > 15)
		skill_level := 15;
	endif

	case (skill_level)
		6:
			res_chance := 5;
		7:
			res_chance := 10;
		8:
			res_chance := 15;
		9:
			res_chance := 20;
		10:
			res_chance := 25;
		11:
			res_chance := 30;
		12:
			res_chance := 35;
		13:
			res_chance := 40;
		14:
			res_chance := 50;
		15:
			res_chance := 60;
		default:
			res_chance := 0;
	endcase;

	return res_chance;
endfunction




///////////////////
//  determines the healers chance of successfully resurrecting the patient, based on their
//  healing and anatomy skill
///////////////////

function DetermineHealDice (healer_healing, healer_anatomy)
	var heal_dice := "1d5";
	var skill_level := CINT (healer_healing/10) + CINT(healer_anatomy/20);
	if (skill_level > 19)
		skill_level := 19;
	endif

	case (skill_level)
		1:
			heal_dice := "2d10";
		2:
			heal_dice := "2d10";
		3:
			heal_dice := "2d10";
		4:
			heal_dice := "2d10";
		5:
			heal_dice := "3d10";
		6:
			heal_dice := "4d10";
		7:
			heal_dice := "5d10";
		8:
			heal_dice := "6d10";
		9:
			heal_dice := "7d10";
		10:
			heal_dice := "8d10";
		11:
			heal_dice := "9d10";
		12:
			heal_dice := "9d10";
		13:
			heal_dice := "10d10";
		14:
			heal_dice := "10d10";
		15:
			heal_dice := "11d10";
		16:
			heal_dice := "11d10+1";
		17:
			heal_dice := "11d10+2";
		18:
			heal_dice := "11d10+3";
		19:
			heal_dice := "11d10+4";
		default:
			heal_dice := "1d5";
	endcase;

	return heal_dice;
endfunction