use uo;
use os;
use util;
use cfgfile;
use cliloc;

include "include/client";
include "include/objtype";
include "include/wodinc";
include "include/magic";
include "include/poison";
include "include/utility";
include "include/dotempmods";
include "include/attributes";
include "include/itemtypes";
include "../pkg/npcs/npcinfo";
include "../pkg/npcs/npc_util";
include "../pkg/character/virtue/virtue";
include ":weapons:abilityIcons";



///////////////////
//  Figures out the defender's AR rating.  For players, this is done by randomly choosing a zone in which
//  they get hit.  For NPCs, its a combination of any armor they have equipped (if any) and their
//  entry in npcdesc.cfg
///////////////////

function GetDefendersAR (defender)
	//Figure out which zone got hit
	var hitzone := "";
	var therandomnumber := RandomInt (100);
	if (therandomnumber < 7)
		hitzone := "Hands";
	elseif (therandomnumber < 14)
		hitzone := "Neck";
	elseif (therandomnumber < 28)
		hitzone := "Legs/feet";
	elseif (therandomnumber < 42)
		hitzone := "Head";
	elseif (therandomnumber < 56)
		hitzone := "Arms";
	else
		hitzone := "Body";
	endif

	var zone_ar_rating := 0;
	var coverage_array;
	foreach item in ListEquippedItems (defender)
		coverage_array := GetArmorCoverageArray (item);
		if (len (coverage_array))
			if (hitzone in coverage_array)
				//Do some damage to the armor
				if (item.hp or item.hp == 0)
					if (RandomInt (1000) == 1)
						if (DoDamageToEquipment (defender_armor, 1))
							if (defender_armor.desc)
								SendSysMessage (defender, "Your " + defender_armor.desc + " breaks!");
							endif
							DestroyItem (defender_armor);
						endif
					endif
				endif
				if (item.ar > zone_ar_rating)
					zone_ar_rating := item.ar;
					defender_armor := item;
				endif
			endif
		endif
	endforeach

	//Do some damage to the chosen armor, on top of the minor amount of damage already done to all
	//armor covering the zone
	if (defender_armor)
		if (defender_armor.hp or defender_armor.hp == 0)
			if (RandomInt (150) == 1)
				if (DoDamageToEquipment (defender_armor, 1))
					if (defender_armor.desc)
						SendSysMessage (defender, "Your " + defender_armor.desc + " breaks!");
					endif
					DestroyItem (defender_armor);
				endif
			endif
		endif
	endif

	//Add in the bonus from AR modifiers from spells and the like
	if (defender.ar_mod)
		zone_ar_rating := zone_ar_rating + defender.ar_mod;
	endif

	//For NPCs, use the higher AR rating of what they have equipped and their entry in NPCdesc.cfg
	if (defender.npctemplate)
		var base_ar := defender_npcdesc_elem.AR;
		//Add in the bonus from AR modifiers from spells and the like
		if (defender.ar_mod)
			base_ar := base_ar + defender.ar_mod;
		endif
		if (base_ar > 90)
			base_ar := 90;
		endif
		if (base_ar > zone_ar_rating)
			return base_ar;
		endif
	elseif (zone_ar_rating > 85)
		zone_ar_rating := 85;
	endif

	return zone_ar_rating;
endfunction

function DoDamageToEquipment (byref item, thedamage)
	if (item.hp > item.maxhp)
		item.hp := item.maxhp;
	endif
	if (item.hp >= thedamage)
		item.hp := item.hp - thedamage;
		item.name := item.desc;
		return 0;
	else
		thedamage := thedamage - item.hp;
		item.hp := 0;
		var durability_loss := GetObjProperty (item, "durability_loss");
		if (!durability_loss)
			durability_loss := 0;
		endif
		if (item.maxhp > thedamage)
			item.maxhp_mod := item.maxhp_mod - thedamage;
			durability_loss := durability_loss + thedamage;
			SetObjProperty (item, "durability_loss", durability_loss);
			item.name := item.desc;
			return 0;
		else
			item.maxhp_mod := item.maxhp_mod - thedamage;
			durability_loss := durability_loss + thedamage;
			SetObjProperty (item, "durability_loss", durability_loss);
			item.name := item.desc;
			return 1;
		endif
	endif
	return 0;
endfunction


///////////////////
//  Read an array of Coverage elements from the item's itemdesc.cfg entry
///////////////////

function GetArmorCoverageArray (item)
	case (item.objtype)
		UOBJ_BACKPACK:
		UOBJ_MOUNT:
			return 0;
	endcase

	var armor_itemdesc_elem := FindConfigElem (armorcfgfile, item.objtype);
	if (armor_itemdesc_elem)
		var coverage_array := GetConfigStringArray (armor_itemdesc_elem, "Coverage");
		if (len (coverage_array))
			return coverage_array;
		endif
	endif
	return 0;
endfunction




///////////////////
//  This function is slightly less accurate than the general-purpose AwardRawAttributePoints,
//  but is quite a bit faster (by about 40%).  Since the combat advancement systemhook
//  is called so often, this function is used.
///////////////////

function QuickAwardRawAttributePoints (character, attributeid, rawpoints)
	var current := GetAttributeBaseValue (character, attributeid);
	var to_increase := GetNeededRawPointsToIncrease (current);
	if (RandomInt (to_increase) < rawpoints)
		var skillcap := GetBaseSkillCap (character, GetSkillIDByAttributeID (attributeid));
		if (skillcap and !character.cmdlevel)
			if (current == skillcap)
				return;
			elseif (current+1 > skillcap)
				current := skillcap-1;
			endif
		endif
		SetAttributeBaseValue (character, attributeid, current+1);
	endif
endfunction


///////////////////
//  Some effects can add bonus damage, which can be either fire, electric or cold.
//  These are set as a dictionary on the player with the keys 'fire', 'electrical', and 'cold'
//  Each of these is an array:  {bonus amount, end time, name (for .status)}
///////////////////

function DoHitSpecialDamage (attacker, defender, attacker_weapon);
	set_critical (1);
	var damage_bonus_dict := GetObjProperty (attacker, "#combat_damage_bonus");
	if (damage_bonus_dict)
		var damage_bonus := damage_bonus_dict["fire"];
		if (damage_bonus)
			var endtime := damage_bonus[2];
			if (endtime and endtime < ReadGameClock())
				damage_bonus_dict.erase ("fire");
				if (len (damage_bonus_dict.keys()))
					SetObjProperty (attacker, "#combat_damage_bonus", damage_bonus_dict);
				else
					EraseObjProperty (attacker, "#combat_damage_bonus");
				endif
			else
				DoDamageByType (defender, attacker, damage_bonus[1], DAMAGETYPE_FIRE);
			endif
		endif
		
		damage_bonus := damage_bonus_dict["electrical"];
		if (damage_bonus)
			var endtime := damage_bonus[2];
			if (endtime and endtime < ReadGameClock())
				damage_bonus_dict.erase ("electrical");
				if (len (damage_bonus_dict.keys()))
					SetObjProperty (attacker, "#combat_damage_bonus", damage_bonus_dict);
				else
					EraseObjProperty (attacker, "#combat_damage_bonus");
				endif
			else
				DoDamageByType (defender, attacker, damage_bonus[1], DAMAGETYPE_ELECTRICAL);
			endif
		endif
		
		damage_bonus := damage_bonus_dict["cold"];
		if (damage_bonus)
			var endtime := damage_bonus[2];
			if (endtime and endtime < ReadGameClock())
				damage_bonus_dict.erase ("cold");
				if (len (damage_bonus_dict.keys()))
					SetObjProperty (attacker, "#combat_damage_bonus", damage_bonus_dict);
				else
					EraseObjProperty (attacker, "#combat_damage_bonus");
				endif
			else
				DoDamageByType (defender, attacker, damage_bonus[1], DAMAGETYPE_COLD);
			endif
		endif
	endif
	
	if (attacker_weapon)
		damage_bonus_dict := GetObjProperty (attacker_weapon, "#combat_damage_bonus");
		if (damage_bonus_dict)
			var damage_bonus := damage_bonus_dict["fire"];
			if (damage_bonus)
				var endtime := damage_bonus[2];
				if (endtime and endtime < ReadGameClock())
					damage_bonus_dict.erase ("fire");
					if (len (damage_bonus_dict.keys()))
						SetObjProperty (attacker_weapon, "#combat_damage_bonus", damage_bonus_dict);
					else
						EraseObjProperty (attacker_weapon, "#combat_damage_bonus");
					endif
				else
					DoDamageByType (defender, attacker, damage_bonus[1], DAMAGETYPE_FIRE);
				endif
			endif
			
			damage_bonus := damage_bonus_dict["electrical"];
			if (damage_bonus)
				var endtime := damage_bonus[2];
				if (endtime and endtime < ReadGameClock())
					damage_bonus_dict.erase ("electrical");
					if (len (damage_bonus_dict.keys()))
						SetObjProperty (attacker_weapon, "#combat_damage_bonus", damage_bonus_dict);
					else
						EraseObjProperty (attacker_weapon, "#combat_damage_bonus");
					endif
				else
					DoDamageByType (defender, attacker, damage_bonus[1], DAMAGETYPE_ELECTRICAL);
				endif
			endif
			
			damage_bonus := damage_bonus_dict["cold"];
			if (damage_bonus)
				var endtime := damage_bonus[2];
				if (endtime and endtime < ReadGameClock())
					damage_bonus_dict.erase ("cold");
					if (len (damage_bonus_dict.keys()))
						SetObjProperty (attacker_weapon, "#combat_damage_bonus", damage_bonus_dict);
					else
						EraseObjProperty (attacker_weapon, "#combat_damage_bonus");
					endif
				else
					DoDamageByType (defender, attacker, damage_bonus[1], DAMAGETYPE_COLD);
				endif
			endif
		endif
	endif
	set_critical (0);	

endfunction

function WeaponSpecial (ability_index, attacker, defender, raw_damage)
	if (!ability_index)
		return 0;
	endif
	var attacker_weaponskill := 0;
	if (attacker_weapon.attribute)
		attacker_weaponskill := GetAttribute (attacker, (attacker_weapon.attribute));
	else
		attacker_weaponskill := GetAttribute (attacker, ATTRIBUTEID_WRESTLING);
	endif
	if (attacker_weaponskill < 80)
		SendSysMessageCL (attacker, 1060182, CAscZ("80"));
		ResetWeaponAbilities (attacker);
		return 0;
	endif
	case (ability_index)
		0x1:	// Armor Ignore
			return run_script_to_completion ( ":weaponabilities:armorignore", array {attacker, defender, raw_damage});
		0x2:	// Bleed Attack
			//return run_script_to_completion ( ":weaponabilities:bleedattack", array {attacker, defender, raw_damage});
			return 0;
		0x3:	// Concussion Blow
			return run_script_to_completion ( ":weaponabilities:concussionblow", array {attacker, defender, raw_damage});
		0x4:	// Crushing Blow
			return run_script_to_completion ( ":weaponabilities:crushingblow", array {attacker, defender, raw_damage});
		0x5:	// Disarm
			return run_script_to_completion ( ":weaponabilities:disarm", array {attacker, defender, raw_damage});
		0x6:	// Dismount
			//return run_script_to_completion ( ":weaponabilities:dismount", array {attacker, defender, raw_damage});
			return 0;
		0x7:	// Double Strike
			//return run_script_to_completion ( ":weaponabilities:doublestrike", array {attacker, defender, raw_damage});
			return 0;
		0x8:	// Infectious Strike
			//return run_script_to_completion ( ":weaponabilities:infectiousstrike", array {attacker, defender, raw_damage});
			return 0;
		0x9:	// Mortal Strike
			//return run_script_to_completion ( ":weaponabilities:mortalstrike", array {attacker, defender, raw_damage});
			return 0;
		0xa:	// Moving Shot
			//return run_script_to_completion ( ":weaponabilities:movingshot", array {attacker, defender, raw_damage});
			return 0;
		0xb:	// Paralyzing Blow
			//return run_script_to_completion ( ":weaponabilities:paralyzingblow", array {attacker, defender, raw_damage});
			return 0;
		0xc:	// Shadow Strike
			//return run_script_to_completion ( ":weaponabilities:shadowstrike", array {attacker, defender, raw_damage});
			return 0;
		0xd:	// Whirlwind Attack
			//return run_script_to_completion ( ":weaponabilities:whirlwindattack", array {attacker, defender, raw_damage});
			return 0;
		0xe:	// Riding Swipe
			//return run_script_to_completion ( ":weaponabilities:ridingswipe", array {attacker, defender, raw_damage});
			return 0;
		0xf:	// Frenzied Whirlwind
			//return run_script_to_completion ( ":weaponabilities:frenziedwhirlwind", array {attacker, defender, raw_damage});
			return 0;
		0x10:	// Block
			//return run_script_to_completion ( ":weaponabilities:block", array {attacker, defender, raw_damage});
			return 0;
		0x11:	// Defense Mastery
			//return run_script_to_completion ( ":weaponabilities:defensemastery", array {attacker, defender, raw_damage});
			return 0;
		0x12:	// Nerve Strike
			//return run_script_to_completion ( ":weaponabilities:nervestrike", array {attacker, defender, raw_damage});
			return 0;
		0x13:	// Talon Strike
			//return run_script_to_completion ( ":weaponabilities:talonstrike", array {attacker, defender, raw_damage});
			return 0;
		0x14:	// Feint
			//return run_script_to_completion ( ":weaponabilities:feint", array {attacker, defender, raw_damage});
			return 0;
		0x15:	// Dual Wield
			//return run_script_to_completion ( ":weaponabilities:dualwield", array {attacker, defender, raw_damage});
			return 0;
		0x16:	// Double Shot
			//return run_script_to_completion ( ":weaponabilities:doubleshot", array {attacker, defender, raw_damage});
			return 0;
		0x17:	// Armor Pierce
			//return run_script_to_completion ( ":weaponabilities:armorpierce", array {attacker, defender, raw_damage});
			return 0;
		0x18:	// Bladeweave
			//return run_script_to_completion ( ":weaponabilities:bladeweave", array {attacker, defender, raw_damage});
			return 0;
		0x19:	// Force Arrow
			//return run_script_to_completion ( ":weaponabilities:forcearrow", array {attacker, defender, raw_damage});
			return 0;
		0x1a:	// Lightning Arrow
			//return run_script_to_completion ( ":weaponabilities:lightningarrow", array {attacker, defender, raw_damage});
			return 0;
		0x1b:	// Psychic Attack
			//return run_script_to_completion ( ":weaponabilities:psychicattack", array {attacker, defender, raw_damage});
			return 0;
		0x1c:	// Serpent Arrow
			//return run_script_to_completion ( ":weaponabilities:serpentarrow", array {attacker, defender, raw_damage});
			return 0;
		0x1d:	// Force of Nature
			//return run_script_to_completion ( ":weaponabilities:forceofnature", array {attacker, defender, raw_damage});
			return 0;
		0x1e:	// Infused Throw
			//return run_script_to_completion ( ":weaponabilities:infusedthrow", array {attacker, defender, raw_damage});
			return 0;
		0x1f:	// Mystic Arc
			//return run_script_to_completion ( ":weaponabilities:mysticarc", array {attacker, defender, raw_damage});
			return 0;
	endcase
	return 0;
endfunction

function ThrowingSpecial (ability_index, attacker, defender, raw_damage, weapon_type)
	if (weapon_type)
	endif
	ResetWeaponAbilities (attacker);
	return 0;
	if (!ability_index)
		return 0;
	endif
	var attacker_weaponskill := 0;
	if (attacker_weapon.attribute)
		attacker_weaponskill := GetAttribute (attacker, (attacker_weapon.attribute));
	else
		attacker_weaponskill := GetAttribute (attacker, ATTRIBUTEID_WRESTLING);
	endif
	if (attacker_weaponskill < 80)
		SendSysMessageCL (attacker, 1060182, CAscZ("80"));
		ResetWeaponAbilities (attacker);
		return 0;
	endif
	case (ability_index)
		0x5:	// Disarm
			return run_script_to_completion ( ":weaponabilities:disarm", array {attacker, defender, raw_damage});
		0xb:	// Paralyzing Blow
			//return run_script_to_completion ( ":weaponabilities:paralyzingblow", array {attacker, defender, raw_damage});
			return 0;
	endcase
	return 0;
endfunction
