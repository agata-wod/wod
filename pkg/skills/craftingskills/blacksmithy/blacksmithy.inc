include "include/attributes";

///////////////////
//  Allows the user to try to repair a blacksmithy item
///////////////////

function RepairBlacksmithyItem (byref character, byref tool, byref item, repair_skill)
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
		DoSpecialBlacksmithingStuff (character, item, repair_skill);
		return;
	endif

	//setup some variables
	var item_damaged_amount := item.maxhp - item.hp;

	//now determine how many ingots and how much skill it will take to repair this item
	var ingots_needed := CINT (item_damaged_amount/20);
	if (!ingots_needed)
		ingots_needed := 1;
	endif

	//allow the use to pick which ingots to use
	SendSysmessage (character, "Target the ingots to use:");
	var found_ingots := Target (character);
	if (!found_ingots)
		SendSysMessage (character, "Canceled.");
		return;
	endif
	if (!IsIngot (found_ingots.objtype) )
		SendSysMessage (character, "That's not an ingot!");
		return;
	endif
	if (!ReserveItem (found_ingots) )
		SendSysMessage (character, "You cannot use that right now.");
		return;
	endif
	if (!Accessible (character, found_ingots) )
		SendSysMessage (character, "You can't reach that.");
		return;
	endif
	if (found_ingots.amount < ingots_needed)
		SendSysMessage (character, "You need " + ingots_needed + " ingots.  That's only " + found_ingots.amount + "!");
		return;
	endif

	PlaySoundEffect(character, SFX_ANVIL);
	sleep (2);
	PlaySoundEffect(character, SFX_ANVIL);
	sleep (2);
	PlaySoundEffect(character, SFX_ANVIL);
	sleep (2);

	SubtractAmount (found_ingots, ingots_needed);
  
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

	var skill_bonus := CINT (GetAttribute (character, GetAttributeIDBySkillID (repair_skill)));
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




///////////////////
//  checks what special options are available for upgrading a blacksmithy item
///////////////////

function DoSpecialBlacksmithingStuff (byref character, byref item, repair_skill)
	//nothing we can do with player made items at the highest upgrade level
	if (item.desc["exceptional"] or item.desc["Exceptional"])
		SendSysMessage (character, "That item is in full repair.");
		return;
	endif

	//if its a magic item and they're using blacksmithy, they can change the color of the item, or
	//if its a breastplate, they can change its gender
	if (IsMagicalItem (item))
		if (repair_skill != SKILLID_BLACKSMITHY)
			SendSysMessage (character, "That item is in full repair.");
			return;
		endif

		if (item.graphic == UOBJ_PLATE_BREASTPLATE or item.graphic == UOBJ_PLATE_BREASTPLATE2 or
				item.graphic == UOBJ_PLATE_FEMALE or item.graphic == UOBJ_PLATE_FEMALE2)
			var menu := CreateMenu ("Options:");
			AddMenuItem (menu, 0, "Laminate");
			AddMenuItem (menu, 0, "Change gender");

			var selection := SelectMenuItem2 (character, menu);
			if (!selection)
				SendSysMessage (character, "Canceled.");
				return;
			elseif (selection.index == 1)
				DoColorCoating (character, item, repair_skill);
				return;
			elseif (selection.index == 2)
				ChangeGenderOfMetalArmor (character, item, repair_skill);
				return;
			else
				SendSysMessage (character, "You're not supposed to get this message.  Whoops.");
				return;
			endif
			return;
		else
			DoColorCoating (character, item, repair_skill);
			return;
		endif

	//otherwise try to upgrade it
	else
		DoBlacksmithyItemUpgrade (character, item, repair_skill);
		return;
	endif
endfunction




///////////////////
//  Tries to upgrade the item that was selected.  This section only confirms the upgrade, then
//  calls a seperate section of code
///////////////////

function DoBlacksmithyItemUpgrade (byref character, byref item, repair_skill)
	var smith_cfg_file := ReadConfigFile( ":blacksmithy:blacksmithy" );
	var elem := FindConfigElem (smith_cfg_file, item.graphic);
	if (!elem)
		SendSysMessage (character, "That item is in full repair.");
		return;
	endif

	var material_needed := CINT (elem.material/2);
	var time_delay := elem.time;
	if (time_delay > 1)
		time_delay := time_delay - 1;
	endif;

	var item_name := elem.name;
	var item_skill := elem.skill + 15;
	if (item.desc["quality"])
		item_skill := item_skill + 10;
	endif
	if (item_skill > 110)
		item_skill := 110;
	endif

	SendSysMessage (character, "That item is in full repair.");
	SendSysMessage (character, "Upgrading that item will take " + material_needed + " ingots and " + item_skill + " skill.");
	if (item_skill >= GetAttribute (character, GetAttributeIDBySkillID (repair_skill)) + 20)
		SendSysMessage (character, "Your skill is too low to upgrade that item.");
		return;
	endif

	//allow the use to pick which ingots to use
	SendSysmessage (character, "Target the ingots to use:");
	var found_ingots := Target (character);
	if (!found_ingots)
		SendSysMessage (character, "Canceled.");
		return;
	endif

	if (!IsIngot (found_ingots.objtype) )
		SendSysMessage (character, "That's not an ingot!");
		return;
	endif
	if (!ReserveItem (found_ingots) )
		SendSysMessage (character, "You cannot use that right now.");
		return;
	endif
	if (!Accessible (character, found_ingots) )
		SendSysMessage (character, "You can't reach that.");
		return;
	endif
	if (found_ingots.amount < material_needed)
		SendSysMessage (character, "You need " + material_needed + " ingots.  That's only " + found_ingots.amount + "!");
		return;
	endif

	PerformBlacksmithyItemUpgrade (character, item, found_ingots, material_needed, repair_skill, item_skill, time_delay, item_name);
endfunction




///////////////////
//  this function does the actual item upgrade
///////////////////

function PerformBlacksmithyItemUpgrade (byref character, byref item, byref found_ingots, material_needed, repair_skill, item_skill, time_delay, item_name, only_upgrade_to_quality := 0)
	var sx := character.x;
	var sy := character.y;

	repeat
		for i := 1 to time_delay
			PlaySoundEffect(character, SFX_ANVIL);
			sleep (2);
		endfor
		PlaySoundEffect(character, SFX_ANVIL);

		if (CheckSkill (character, repair_skill, item_skill, 0) )
			SendSysMessage (character, "You succeed in upgrading the item.");
			SubtractAmount (found_ingots, material_needed);

			if (item.desc ["quality"])
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
				
				if (only_upgrade_to_quality)
					return;
				endif

				item_skill := item_skill + 10;
				if (item_skill > 110)
					item_skill := 110;
				endif

				if (item_skill > GetAttribute (character, GetAttributeIDBySkillID (repair_skill)) + 20)
					SendSysMessage (character, "Your skill is too low to upgrade that item.");
					return;
				endif
			endif
		else
			SubtractAmount (found_ingots, RandomInt (CInt (material_needed/2)) + 1 );
			if (RandomInt (10) == 0)
				SendSysMessage (character, "You ruin the item trying to upgrade it.");
				DestroyItem (item);
				return;
			else
				SendSysMessage (character, "You destroy some material.");
			endif
		endif

		if (!found_ingots or found_ingots.amount < material_needed)
			SendSysMessage (character, "You don't have enough material to continue.");
			return;
		endif
	until (character.x != sx or character.y != sy);
endfunction





///////////////////
//  allows the user to coat the given piece of equipment with another color
///////////////////

function DoColorCoating (byref character, byref item, repair_skill)
	var smith_cfg_file := ReadConfigFile( ":blacksmithy:blacksmithy" );
	var elem := FindConfigElem (smith_cfg_file, item.graphic);
	if (!elem)
		if (item.objtype == UOBJ_CHAINMAIL_GLOVES or item.objtype == UOBJ_RINGMAIL_COIF)
			elem := FindConfigElem (smith_cfg_file, item.objtype);
		endif
		if (!elem)
			SendSysMessage (character, "That item is in full repair.");
			return;
		endif
	endif

	var material_needed := cint (elem.material/2);
	var time_delay := elem.time;
	if (time_delay > 1)
		time_delay := time_delay - 1;
	endif;

	SendSysMessage (character, "That item is in full repair.");
	SendSysMessage (character, "Laminating that item will take " + material_needed + " ingots and 90 skill.");
	if (GetAttribute (character, GetAttributeIDBySkillID (repair_skill)) < 90)
		SendSysMessage (character, "Your skill is too low to laminate that item.");
		return;
	endif

	SendSysmessage (character, "Target some ingots to laminate the item with.");
	var found_ingots := Target (character);
	if (!found_ingots)
		SendSysMessage (character, "Canceled.");
		return;
	endif
	if (!ReserveItem (found_ingots) )
		SendSysMessage (character, "You cannot use that right now.");
		return;
	endif
	if (!IsIngot (found_ingots.objtype) )
		SendSysMessage (character, "That's not an ingot!");
		return;
	endif
	if (!Accessible (character, found_ingots) )
		SendSysMessage (character, "You can't reach that.");
		return;
	endif
	if (found_ingots.amount < material_needed)
		SendSysMessage (character, "You need " + material_needed + " ingots.  That's only " + found_ingots.amount + "!");
		return;
	endif

	var confirm_upgrade := YesNo (character, "Laminate?");
	if (!confirm_upgrade)
		SendSysMessage (character, "Canceled.");
		return;
	endif

	for i := 1 to time_delay
		PlaySoundEffect(character, SFX_ANVIL);
		sleep (2);
	endfor
	PlaySoundEffect(character, SFX_ANVIL);

	if (RandomInt (1000) == 0)
		if (!elem.laminate_nodestroy)
			SendSysMessage (character, "You fail, destroying the item.");
			DestroyItem (item);
		else
			SendSysMessage (character, "You fail, destroying some ingots.");
			SubtractAmount (found_ingots, RandomInt (material_needed) );
		endif
		return;
	elseif ( RandomInt (100) < 10 )
		SendSysMessage (character, "You fail, destroying some ingots.");
		SubtractAmount (found_ingots, RandomInt (material_needed) );
		return;
	else
		SendSysMessage (character, "You succeed in laminating the item.");
		item.color := found_ingots.color;
		SubtractAmount (found_ingots, material_needed);
		return;
	endif

endfunction




///////////////////
//  changes the gender of plate armor from male to female and back again
///////////////////

function ChangeGenderOfMetalArmor (byref character, byref item, repair_skill)
	var smith_cfg_file := ReadConfigFile( ":blacksmithy:blacksmithy" );
	var elem := FindConfigElem (smith_cfg_file, item.graphic);
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
	SendSysMessage (character, "Changing the gender of that item will take " + material_needed + " ingots and 90 skill.");
	if (GetAttribute (character, GetAttributeIDBySkillID (repair_skill)) < 90)
		SendSysMessage (character, "Your skill is too low to do this.");
		return;
	endif

	SendSysmessage (character, "Target the ingots to use:");
	var found_ingots := Target (character);
	if (!found_ingots)
		SendSysMessage (character, "Canceled.");
		return;
	endif
	if (!IsIngot (found_ingots.objtype) )
		SendSysMessage (character, "That's not an ingot!");
		return;
	endif
	if (!ReserveItem (found_ingots) )
		SendSysMessage (character, "You cannot use that right now.");
		return;
	endif
	if (!Accessible (character, found_ingots) )
		SendSysMessage (character, "You can't reach that.");
		return;
	endif
	if (found_ingots.amount < material_needed)
		SendSysMessage (character, "You need " + material_needed + " ingots.  That's only " + found_ingots.amount + "!");
		return;
	endif

	var confirm_upgrade := YesNo (character, "Change gender?");
	if (!confirm_upgrade)
		SendSysMessage (character, "Canceled.");
		return;
	endif

	for i := 1 to time_delay
		PlaySoundEffect (character, SFX_ANVIL);
		sleep (2);
	endfor
	PlaySoundEffect(character, SFX_ANVIL);

	if (RandomInt (1000) == 0)
		SendSysMessage (character, "You fail, destroying the item.");
		DestroyItem (item);
		return;
	elseif ( RandomInt (100) < 10 )
		SendSysMessage (character, "You fail, destroying some ingots.");
		SubtractAmount (found_ingots, RandomInt (material_needed) );
		return;
	else
		SendSysMessage (character, "You succeed in modifying the item.");
		case (item.graphic)
			UOBJ_PLATE_BREASTPLATE:
				item.graphic := UOBJ_PLATE_FEMALE;
			UOBJ_PLATE_BREASTPLATE2:
				item.graphic := UOBJ_PLATE_FEMALE2;
			UOBJ_PLATE_FEMALE:
				item.graphic := UOBJ_PLATE_BREASTPLATE;
			UOBJ_PLATE_FEMALE2:
				item.graphic := UOBJ_PLATE_BREASTPLATE2;
			default:
				SendSysMessage (character, "Um, never mind.  I was thinking of a different type of item, I guess.");
				return;
		endcase
		SubtractAmount (found_ingots, material_needed);
		return;
	endif

endfunction


