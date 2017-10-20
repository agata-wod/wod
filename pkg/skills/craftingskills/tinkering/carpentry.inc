///////////////////
//  Allows the user to try to repair a carpentry item
///////////////////

function RepairCarpentryItem (character, tool, item, repair_skill)
	if (!item.maxhp or !item.quality)
		SendSysmessage (character, "That item cannot be repaired.");
    if (tool)
    endif
		return;
	endif

	//if the item is in full repair, try to do special stuff
	if (item.hp >= item.maxhp)
		if ( GetObjProperty (item,"ownerserial") )
			SendSysMessage (character, "That item is in full repair.");
			SendSysMessage (character, "That has an owner's mark!");
			return;
		endif
    DoSpecialCarpentryStuff (character, item, repair_skill);
		return;
	endif

	//setup some variables
	var item_damaged_amount := item.maxhp - item.hp;

	//now determine how many material and how much skill it will take to repair this item
	var material_needed := CINT (item_damaged_amount/10);
	if (!material_needed)
		material_needed := 1;
	endif

	//allow the use to pick which logs to use
	SendSysmessage (character, "Target the logs to use:");
	var found_logs := Target (character);
	if (!found_logs)
		SendSysMessage (character, "Canceled.");
		return;
	endif
	if (found_logs.objtype != UOBJ_LOG)
		SendSysMessage (character, "That's not a log!");
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
		SendSysMessage (character, "You need " + material_needed + " logs.  That's only " + found_logs.amount + "!");
		return;
	endif

	PlaySoundEffect(character, SFX_HAMMER);
	sleep (2);
	PlaySoundEffect(character, SFX_HAMMER);
	sleep (2);
	PlaySoundEffect(character, SFX_HAMMER);
	sleep (2);

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

	var skill_bonus := CINT (GetAttribute (character, repair_skill));
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
	return;
endfunction

function DoSpecialCarpentryStuff (byref character, byref item, repair_skill)
	//nothing we can do with player made items at the highest upgrade level
	if (item.desc["exceptional"] or item.desc["Exceptional"])
		SendSysMessage (character, "That item is in full repair.");
		return;
	endif

	//if its a magic item and they're using carpentry, they can change the color of the item, or
	//if its a breastplate, they can change its gender
	if (IsMagicalItem (item))
		if (repair_skill != SKILLID_CARPENTRY)
			SendSysMessage (character, "That item is in full repair.");
			return;
		endif

		if (item.graphic ==  0x2b6d or item.graphic == 0x2b67)
			var menu := CreateMenu ("Options:");
			AddMenuItem (menu, 0, "Laminate");
			AddMenuItem (menu, 0, "Change gender");

			var selection := SelectMenuItem2 (character, menu);
			if (!selection)
				SendSysMessage (character, "Canceled.");
				return;
			elseif (selection.index == 1)
				DoWoodColorCoating (character, item, repair_skill);
				return;
			elseif (selection.index == 2)
				ChangeGenderOfWoodenArmor (character, item, repair_skill);
				return;
			else
				SendSysMessage (character, "You're not supposed to get this message.  Whoops.");
				return;
			endif
			return;
		else
			DoWoodColorCoating (character, item, repair_skill);
			return;
		endif

	//otherwise try to upgrade it
	else
		DoCarpentryItemUpgrade (character, item, repair_skill);
		return;
	endif
endfunction




///////////////////
//  Tries to upgrade the item that was selected
///////////////////

function DoCarpentryItemUpgrade (character, item, repair_skill)
	//make sure its an item that can be upgraded
	if (item.desc["xceptional"])
		SendSysMessage (character, "That item is in full repair.");
		return;
	endif

	if (IsMagicalItem (item) )
		SendSysMessage (character, "That item is in full repair.");
		return;
	endif

	var smith_cfg_file := ReadConfigFile( ":carpentry:carpentry" );
	var elem := FindConfigElem (smith_cfg_file, item.graphic);
	if (!elem)
		SendSysMessage (character, "I don't know how to repair that.");
		return;
	endif

	var material_needed := cint (elem.material/2);
	var item_name := elem.name;
	var item_skill := elem.skill + 20;
	if (item.desc["quality"])
		item_skill := item_skill + 10;
	endif
	if (item_skill > 110)
		item_skill := 110;
	endif

	SendSysMessage (character, "That item is in full repair.");
	SendSysMessage (character, "Upgrading that item will take " + material_needed + " logs and " + item_skill + " skill.");
	if (item_skill >= GetAttribute (character, GetAttributeIDBySkillID (repair_skill)) + 20)
		SendSysMessage (character, "Your skill is too low to upgrade that item.");
		return;
	endif

	var found_lumber := FindObjtypeInContainer (character.backpack, UOBJ_LOG);
	if (!found_lumber or found_lumber.amount < material_needed)
		SendSysMessage (character, "You don't have enough logs to upgrade this item.");
		return;
	endif

	if (!ReserveItem (found_lumber) )
		SendSysMessage (character, "You can't do this right now.");
		return;
	endif

	var confirm_upgrade := YesNo (character, "Upgrade?");
	if (!confirm_upgrade)
		SendSysMessage (character, "Canceled.");
		return;
	endif

	PerformCarpentryItemUpgrade (character, item, found_lumber, material_needed, repair_skill, item_skill, item_name);
endfunction




///////////////////
//  this function does the actual item upgrade
///////////////////

function PerformCarpentryItemUpgrade (character, item, found_lumber, material_needed, repair_skill, item_skill, item_name)
	var sx := character.x;
	var sy := character.y;

	repeat
		for i := 1 to 4
			PlaySoundEffect(character, SFX_HAMMER);
			sleep (2);
		endfor
		PlaySoundEffect(character, SFX_HAMMER);

		if (CheckSkill (character, repair_skill, item_skill, 0) )
			SendSysMessage (character, "You succeed in upgrading the item.");
			SubtractAmount (found_lumber, material_needed);

			if (item.desc ["quality"])
				item.name := "an exceptional " + TruncateArticle(item_name) + " [crafted by " + character.name + "]";
				item.quality := 1.2;
				item.hp := item.maxhp;
				item.buyprice := 0;
				return;
			else
				item.name := "a quality " + TruncateArticle(item_name) + " [crafted by " + character.name + "]";
				item.quality := 1.0;
				item.hp := item.maxhp;
				item.buyprice := 0;

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
			SubtractAmount (found_lumber, RandomInt(material_needed) );
			if (RandomInt (10) == 0)
				SendSysMessage (character, "You ruin the item trying to upgrade it.");
				DestroyItem (item);
				return;
			else
				SendSysMessage (character, "You destroy some material.");
			endif
		endif

		if (!found_lumber or found_lumber.amount < material_needed)
			SendSysMessage (character, "You don't have enough material to continue.");
			return;
		endif

	until (character.x != sx or character.y != sy);

endfunction

///////////////////
//  allows the user to coat the given piece of equipment with another color
///////////////////

function DoWoodColorCoating (byref character, byref item, repair_skill)
	var carpentry_cfg_file := ReadConfigFile( ":carpentry:carpentry" );
	var elem := FindConfigElem (carpentry_cfg_file, item.graphic);
	if (!elem)
		SendSysMessage (character, "That item is in full repair.");
		return;
	endif

	var material_needed := cint (elem.material/2);
	var time_delay := elem.time;
	if (time_delay > 1)
		time_delay := time_delay - 1;
	endif;

	SendSysMessage (character, "That item is in full repair.");
	SendSysMessage (character, "Laminating that item will take " + material_needed + " logs and 90 skill.");
	if (GetAttribute (character, GetAttributeIDBySkillID (repair_skill)) < 90)
		SendSysMessage (character, "Your skill is too low to laminate that item.");
		return;
	endif

	SendSysmessage (character, "Target some logs to laminate the item with.");
	var found_logs := Target (character);
	if (!found_logs)
		SendSysMessage (character, "Canceled.");
		return;
	endif
	if (!ReserveItem (found_logs) )
		SendSysMessage (character, "You cannot use that right now.");
		return;
	endif
	if (!IsLogType (found_logs) )
		SendSysMessage (character, "That's not a log!");
		return;
	endif
	if (!Accessible (character, found_logs) )
		SendSysMessage (character, "You can't reach that.");
		return;
	endif
	if (found_logs.amount < material_needed)
		SendSysMessage (character, "You need " + material_needed + " logs.  That's only " + found_logs.amount + "!");
		return;
	endif

	var confirm_upgrade := YesNo (character, "Laminate?");
	if (!confirm_upgrade)
		SendSysMessage (character, "Canceled.");
		return;
	endif

	for i := 1 to time_delay
		PlaySoundEffect(character, SFX_HAMMER);
		sleep (2);
	endfor
	PlaySoundEffect(character, SFX_HAMMER);

	if (RandomInt (1000) == 0)
		if (!elem.laminate_nodestroy)
			SendSysMessage (character, "You fail, destroying the item.");
			DestroyItem (item);
		else
			SendSysMessage (character, "You fail, destroying some logs.");
			SubtractAmount (found_logs, RandomInt (material_needed) );
		endif
		return;
	elseif ( RandomInt (100) < 10 )
		SendSysMessage (character, "You fail, destroying some logs.");
		SubtractAmount (found_logs, RandomInt (material_needed) );
		return;
	else
		SendSysMessage (character, "You succeed in laminating the item.");
		item.color := found_logs.color;
		SubtractAmount (found_logs, material_needed);
		return;
	endif

endfunction




///////////////////
//  changes the gender of plate armor from male to female and back again
///////////////////

function ChangeGenderOfWoodenArmor (byref character, byref item, repair_skill)
	var carpentry_cfg_file := ReadConfigFile( ":carpentry:carpentry" );
	var elem := FindConfigElem (carpentry_cfg_file, item.graphic);
	if (!elem)
		SendSysMessage (character, "That item is in full repair.");
		return;
	endif

	var material_needed := cint (elem.material/2);
	var time_delay := elem.time;
	if (time_delay > 1)
		time_delay := time_delay - 1;
	endif;

	SendSysMessage (character, "That item is in full repair.");
	SendSysMessage (character, "Changing the gender of that item will take " + material_needed + " logs and 90 skill.");
	if (GetAttribute (character, GetAttributeIDBySkillID (repair_skill)) < 90)
		SendSysMessage (character, "Your skill is too low to do this.");
		return;
	endif

	SendSysmessage (character, "Target the logs to use:");
	var found_logs := Target (character);
	if (!found_logs)
		SendSysMessage (character, "Canceled.");
		return;
	endif
	if (!IsLogType (found_logs) )
		SendSysMessage (character, "That's not a log!");
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
		SendSysMessage (character, "You need " + material_needed + " logs.  That's only " + found_logs.amount + "!");
		return;
	endif

	var confirm_upgrade := YesNo (character, "Change gender?");
	if (!confirm_upgrade)
		SendSysMessage (character, "Canceled.");
		return;
	endif

	for i := 1 to time_delay
		PlaySoundEffect (character, SFX_HAMMER);
		sleep (2);
	endfor
	PlaySoundEffect(character, SFX_HAMMER);

	if (RandomInt (1000) == 0)
		SendSysMessage (character, "You fail, destroying the item.");
		DestroyItem (item);
		return;
	elseif ( RandomInt (100) < 10 )
		SendSysMessage (character, "You fail, destroying some logs.");
		SubtractAmount (found_logs, RandomInt (material_needed) );
		return;
	else
		SendSysMessage (character, "You succeed in modifying the item.");
		case (item.graphic)
			0x2b67:
				item.graphic := 0x2b6d;
			0x2b6d:
				item.graphic := 0x2b67;
			default:
				SendSysMessage (character, "Um, never mind.  I was thinking of a different type of item, I guess.");
				return;
		endcase
		SubtractAmount (found_logs, material_needed);
		return;
	endif

endfunction


