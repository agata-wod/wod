/*
        Maintened by *Edwards

        2010-03-21
        
        Original Idea from ZuluHotel
*/
use uo;
use util;


function RepairItem( character, repair_skill, item:=0 )
        if( !item )
                SendSysMessage( character, "Select the item you would like to repair." );
                item := Target( character );
                if( !item )
                        SendSysMessage( character, "Cancelled." );
                        return 0;
                endif
                
        endif
	if (!item.maxhp or !item.quality)
		SendSysmessage (character, "That item cannot be repaired.");
		return;
	endif

	//if the item is in full repair, try to do special stuff
	if (item.hp >= item.maxhp)
		SendSysMessage (character, "That item is in full repair.");
		return;
	endif
	if( !craft_config[item.objtype] )
		if (repair_skill == SKILLID_TINKERING)
			craft_config := ReadConfigFile( ":woodworking:carpentry" );
			if( !craft_config[item.objtype] )
				craft_config := ReadConfigFile( ":tailoring:tailoring" );
				if( !craft_config[item.objtype] )
					craft_config := ReadConfigFile( ":blacksmithy:blacksmithy" );
					if( !craft_config[item.objtype] )
						craft_config := ReadConfigFile( ":stonecrafting:stonecrafting" );
						if( !craft_config[item.objtype] )
							SendSysMessage( character, "You cannot upgrade the item with your tool." );
							return 0;
						endif
					endif
				endif
			endif
		else
			SendSysMessage( character, "You cannot repair the item with your tool." );
			return 0;
		endif
	endif
	var time_delay := craft_config[item.objtype].time;
	var skill_elem := menu_config[GetAttributeIDBySkillID(repair_skill)];
	//setup some variables
	var item_damaged_amount := item.maxhp - item.hp;

	//now determine how many material and how much skill it will take to repair this item
	var material_needed := CINT (item_damaged_amount/10);
	if (!material_needed)
		material_needed := 1;
	endif

	//allow the use to pick which logs to use
	var found_logs := MaterialSelection( character, GetAttributeIDBySkillID(repair_skill), craft_config[item.objtype].Type );
	if (!found_logs)
		SendSysMessage (character, "Canceled.");
		return;
	endif
	if (!ReserveItem (found_logs) )
		SendSysMessage (character, "You cannot use that right now.");
		return;
	endif
	if (!Accessible (character, found_logs) )
		SendSysMessage (character, "You can't reach that.");
		return;
	endif
	if (found_logs.amount < material_needed)
		SendSysMessage (character, "You need " + material_needed + " material.  That's only " + found_logs.amount + "!");
		return;
	endif

	Craft_PlayEffects( character, skill_elem );
	sleep(3);
	for i := 1 to time_delay
		Craft_PlayEffects( character, skill_elem );
		sleepms (1500);
	endfor
	

	SubtractAmount (found_logs, material_needed);

	var durability_loss := GetObjProperty (item, "durability_loss");
	if (!durability_loss)
		durability_loss := 0;
	endif

	if (!CheckSkill (character, repair_skill, -1, 0) )
		var thedamage := RandomInt(10);
		durability_loss := durability_loss + thedamage;
		item.maxhp_mod := item.maxhp_mod - thedamage;
		SetObjProperty (item, "durability_loss", durability_loss);
		item.name := item.desc;
		if (item.maxhp <= 0)
			DestroyItem (item);
			SendSysmessage (character, "The brittle material breaks when you handle it.");
			return;
		endif
		SendSysMessage (character, "You only manage to damage the item further...");
		return;
	endif

	var skill_bonus := CINT (GetAttribute (character, GetAttributeIDBySkillID(repair_skill)));
	if (item.name["fine"])
		skill_bonus := skill_bonus + 5;
	elseif (item.name["durable"])
		skill_bonus := skill_bonus + 10;
	elseif (item.name["rugged"])
		skill_bonus := skill_bonus + 15;
	elseif (item.name["tempered"])
		skill_bonus := skill_bonus + 20;
	elseif (item.name["indestructable"])
		skill_bonus := skill_bonus + 25;
	elseif (item.name["exceptional"])
		skill_bonus := skill_bonus + 25;
	endif
  
	var damage_chance := item_damaged_amount - CINT(skill_bonus/2);
	if (damage_chance > 120)
		damage_chance := 120;
	endif
	if (!CheckSkill (character, repair_skill, damage_chance, 0) )
		durability_loss := durability_loss + 1;
		item.maxhp_mod := item.maxhp_mod - 1;
		SetObjProperty (item, "durability_loss", durability_loss);
	endif
	item.hp := item.maxhp;
	item.name := item.desc;
	SendSysMessage (character, "You repair the item completely");
	return 1;
endfunction

function UpgradeItem ( character, repair_skill, item:=0 )
        if( !item )
                SendSysMessage( character, "Select the item you would like to upgrade." );
                item := Target( character );
                if( !item )
                        SendSysMessage( character, "Cancelled." );
                        return 0;
                endif
                
        endif
	if (!item.maxhp or !item.quality)
		SendSysmessage (character, "That item cannot be upgraded.");
		return;
	endif

	//if the item is in full repair, try to do special stuff
	if (item.hp < item.maxhp)
		SendSysMessage (character, "That item needs repairs first!");
		return;
	endif
	if( !craft_config[item.objtype] )
		if (repair_skill == SKILLID_TINKERING)
			craft_config := ReadConfigFile( ":woodworking:carpentry" );
			if( !craft_config[item.objtype] )
				craft_config := ReadConfigFile( ":tailoring:tailoring" );
				if( !craft_config[item.objtype] )
					craft_config := ReadConfigFile( ":blacksmithy:blacksmithy" );
					if( !craft_config[item.objtype] )
						craft_config := ReadConfigFile( ":stonecrafting:stonecrafting" );
						if( !craft_config[item.objtype] )
							SendSysMessage( character, "You cannot upgrade the item with your tool." );
							return 0;
						endif
					endif
				endif
			endif
		else
			SendSysMessage( character, "You cannot upgrade the item with your tool." );
			return 0;
		endif
	endif
	DoSpecialUpgradeStuff (character, item, repair_skill);
endfunction

///////////////////
//  Tries to upgrade the item that was selected
///////////////////

function DoItemUpgrade (character, item, repair_skill)
	if (!craft_config[item.objtype].Exceptional)
		SendSysmessage (character, "That item cannot be upgraded.");
		return;
	endif
	//make sure its an item that can be upgraded
	if (item.desc["xceptional"])
		SendSysMessage (character, "That item can't be upgraded further.");
		return;
	endif

	if (IsMagicalItem (item) )
		SendSysMessage (character, "That item can't be upgraded.");
		return;
	endif

	var elem := FindConfigElem (craft_config, item.objtype);
	if (!elem)
		SendSysMessage (character, "I don't know how to upgrade that.");
		return;
	endif

	var material_needed := cint (elem.material/2);
	var material_type := elem.type;
	var item_name := elem.name;
	var item_skill := elem.skill + 20;
	if (item.desc["quality"])
		item_skill := item_skill + 10;
	endif
	if (item_skill > 110)
		item_skill := 110;
	endif

	SendSysMessage (character, "Upgrading that item will take " + material_needed + " " + material_type +" and " + item_skill + " skill.");
	if (item_skill >= GetAttribute (character, GetAttributeIDBySkillID (repair_skill)) + 20)
		SendSysMessage (character, "Your skill is too low to upgrade that item.");
		return;
	endif

	var found_logs := MaterialSelection( character, GetAttributeIDBySkillID(repair_skill), material_type );
	if (!found_logs)
		SendSysMessage (character, "Canceled.");
		return;
	endif
	if (!ReserveItem (found_logs) )
		SendSysMessage (character, "You cannot use that right now.");
		return;
	endif
	if (!Accessible (character, found_logs) )
		SendSysMessage (character, "You can't reach that.");
		return;
	endif
	if (found_logs.amount < material_needed)
		SendSysMessage (character, "You need " + material_needed + " " + material_type + ".  That's only " + found_logs.amount + "!");
		return;
	endif

	var confirm_upgrade := YesNo (character, "Upgrade?");
	if (!confirm_upgrade)
		SendSysMessage (character, "Canceled.");
		return;
	endif

	PerformItemUpgrade (character, item, found_logs, material_needed, repair_skill, item_skill, item_name, 2);
endfunction

///////////////////
//  this function does the actual item upgrade
///////////////////

function PerformItemUpgrade (character, item, found_material, material_needed, repair_skill, item_skill, item_name, upgrade_to := 2)
	var sx := character.x;
	var sy := character.y;
	var skill_elem := menu_config[craft_skill];
	var time_delay := craft_config[item.objtype].time;
	if (upgrade_to > 2)
		upgrade_to := 2;
	endif
	var upgrade_level := 0;
	if (item.desc ["quality"])
		upgrade_level := 1;
	elseif (item.desc ["xceptional"])
		upgrade_level := 2;
	endif
	if (upgrade_level >= upgrade_to)
		SendSysMessage (character, "That item is already at the desired upgrade level!");
		return;
	endif

	repeat
		Craft_PlayEffects( character, skill_elem );
		sleep(3);
		for i := 1 to time_delay
			Craft_PlayEffects( character, skill_elem );
			sleepms(1500);
		endfor

		if (CheckSkill (character, repair_skill, item_skill, 0) )
			SendSysMessage (character, "You succeed in upgrading the item.");
			SubtractAmount (found_material, material_needed);

			if (upgrade_level == 1)
				item.name := "an exceptional " + TruncateArticle(item_name);
				SetObjProperty (item, "craftername", character.name);
				item.quality := 1.2;
				item.hp := item.maxhp;
				item.buyprice := 0;
				return;
			else
				item.name := "a quality " + TruncateArticle(item_name);
				SetObjProperty (item, "craftername", character.name);
				item.quality := 1.1;
				item.hp := item.maxhp;
				item.buyprice := 0;
				if (upgrade_to == 1)
					return;
				endif
				
				upgrade_level := 1;
				item_skill := item_skill + 10;
				if (item_skill > 110)
					item_skill := 110;
				endif
				
				if (item_skill > GetAttribute (character, GetAttributeIDBySkillID (repair_skill)) + 20)
					SendSysMessage (character, "Your skill is too low to upgrade the item any further.");
					return;
				endif
			endif
		else
			SubtractAmount (found_material, RandomInt(material_needed) );
			if (RandomInt (10) == 0)
				SendSysMessage (character, "You ruin the item trying to upgrade it.");
				DestroyItem (item);
				return;
			else
				SendSysMessage (character, "You destroy some material.");
			endif
		endif

		if (!found_material or found_material.amount < material_needed)
			SendSysMessage (character, "You don't have enough material to continue.");
			return;
		endif

	until (character.x != sx or character.y != sy);

endfunction

