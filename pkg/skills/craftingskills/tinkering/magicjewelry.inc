///////////////////
//
//  I split this off from the main tinkering script because it was getting rather unwieldy
//
///////////////////



///////////////////
//  Makes jewelry magical, giving stat bonuses
///////////////////

function MakeMagicItem (character, item)
	var gemtype := GetObjProperty (item, "gem");
	if (!gemtype)
		return;
	endif

	var diffi := IsAGem (gemtype);
	if (!diffi)
		SendSysMessage (character, "You can't enchant that item.");
		return;
	endif

	var required_obsidian := GetObsidianRequiredByGemtype (gemtype);
	if (!required_obsidian)
		SendSysMessage (character, "You can't enchant that item");
		return;
	endif

	var found_obsidian := FindObjtypeInContainer (character.backpack, UOBJ_OBSIDIAN);
	if (!found_obsidian)
		SendSysMessage (character, "You don't have any obsidian.");
		return;
	elseif (!ReserveItem (found_obsidian) )
		SendSysMessage (character, "You can't use your obsidian right now.");
		return;
	elseif (found_obsidian.amount < required_obsidian)
		SendSysMessage (character, "You don't have enough obsidian.");
		return;
	elseif (GetAttribute (character, "Magery") < (diffi - 15) )
		SendSysMessage (character, "You are not a good enough mage to work with that.");
		return;
	elseif (GetAttribute (character, "Tinkering") < (diffi - 15) )
		SendSysMessage (character, "You are not a good enough tinker to work with that.");
		return;
	endif

	if (GetObjProperty (item, "str") or GetObjProperty (item, "int") or GetObjProperty (item, "dex"))
		if (GetObjProperty (item, "unidentified"))
			SendSysMessage (character, "You must identify the item first!");
			return;
		endif
		UpgradeStatJewelry (character, item, found_obsidian);
		return;
	endif

	PlaySoundEffect (character, SFX_ANVIL);
	sleep(1);
	PlaySoundEffect (character, SFX_GLASSBREAK4);
	sleep(1);

	if ( (!CheckSkill (character, SKILLID_MAGERY, diffi, 0)) or (!CheckSkill (character, SKILLID_TINKERING, diffi, 0) ) )
		SendSysMessage (character, "You fail to make anything useful.");
		SubtractAmount (found_obsidian, RandomInt (CINT (required_obsidian/2))+10);
		DestroyItem (item);
		return;
	endif

	SubtractAmount (found_obsidian, required_obsidian);
	PlaySoundEffect (character, SFX_1EB);
	SendSysMessage (character, "You enchant the item!");
  SetObjProperty (item, "unidentified", 1);

	var max_stat_boost := GetMaxStatBoost (gemtype);
	var stat_bonus :=  max_stat_boost - 2 + RandomInt (3);

	var statboost_type := GetStatBoostTypeByGemtype (gemtype);
	SetObjProperty (item, statboost_type, stat_bonus);
	
	//save some memory here and erase the gem type if the item is at its max - we won't need it anymore (hopefully)
	if (stat_bonus >= max_stat_boost)
		EraseObjProperty (item, "gem");
	endif

	var stat_bonus_desc := GetStatBonusDescription (stat_bonus);
	var gem_description := GetGemDescription (gemtype);
	var item_description := GetItemDescriptionOfJewelry (item.objtype);
	var statboost_desc := GetStatBoostDescriptionByGemtype (gemtype);
	
	item.name := "a magic " + item_description;
	var tname := CSTR ("a " + stat_bonus_desc + " " + gem_description + " " + item_description + " of " + statboost_desc);
	SetObjProperty (item, "tname", tname);
endfunction




///////////////////
//  Upgrade stat enhancing jewelry
///////////////////

function UpgradeStatJewelry (character, item, obsidian)
	var gemtype := GetObjProperty (item, "gem");
	if (!gemtype)
		return;
	endif

	var diffi := IsAGem (gemtype);
	if (!diffi)
		SendSysMessage (character, "You can't enchant that item.");
		return;
	endif

	var required_obsidian := GetObsidianRequiredByGemtype (gemtype);
	if (!required_obsidian)
		SendSysMessage (character, "You can't enchant that item");
		return;
	endif

	var statboost_type := GetStatBoostTypeByGemtype (gemtype);
	var current_statboost := GetObjProperty (item, statboost_type);
	if (!current_statboost)
		SendSysMessage (character, "You can't enchant that item");
		return;
	endif

	var max_stat_boost := GetMaxStatBoost (gemtype);
	if (current_statboost >= max_stat_boost)
		SendSysMessage (character, "That's as good as its going to get.");
		EraseObjProperty (item, "gem");
		return;
	endif

	PlaySoundEffect (character, SFX_ANVIL);
	sleep(1);
	PlaySoundEffect (character, SFX_GLASSBREAK4);
	sleep(1);

	if ( (!CheckSkill (character, SKILLID_MAGERY, diffi, 0)) or (!CheckSkill (character, SKILLID_TINKERING, diffi, 0) ) )
		SendSysMessage (character, "You fail, destroying the item.");
		SubtractAmount (obsidian, RandomInt (CINT (required_obsidian/2))+10);
		DestroyItem (item);
		return;
	endif
	
	SubtractAmount (obsidian, required_obsidian);
	PlaySoundEffect (character, SFX_1EB);
	SendSysMessage (character, "You strengthen the item's enchantment!");

	var stat_bonus := current_statboost + RandomInt (2) + 1;
	if (stat_bonus >= max_stat_boost)
		stat_bonus := max_stat_boost;
	endif
	SetObjProperty (item, statboost_type, stat_bonus);
	
	//save some memory here and erase the gem type if the item is at its max - we won't need it anymore (hopefully)
	if (stat_bonus >= max_stat_boost)
		EraseObjProperty (item, "gem");
	endif

	var stat_bonus_desc := GetStatBonusDescription (stat_bonus);
	var gem_description := GetGemDescription (gemtype);
	var item_description := GetItemDescriptionOfJewelry (item.objtype);
	var statboost_desc := GetStatBoostDescriptionByGemtype (gemtype);
	
	item.name := "a magic " + item_description;
	var tname := CSTR ("a " + stat_bonus_desc + " " + gem_description + " " + item_description + " of " + statboost_desc);
	SetObjProperty (item, "tname", tname);
	SetObjProperty (item, "unidentified", 1);
endfunction




///////////////////
//  returns the difficulty of working with a particular gem
///////////////////

function IsAGem (gemobjtype)
	case (gemobjtype)
		UOBJ_GEM_DIAMOND:
		UOBJ_GEM_SAPPHIRE:
		UOBJ_GEM_STAR_SAPPHIRE:
			return 90;
		UOBJ_GEM_EMERALD:
		UOBJ_GEM_RUBY:
		UOBJ_GEM_AMETHYST:
			return 80;
		UOBJ_GEM_CITRINE:
		UOBJ_GEM_TOURMALINE:
		UOBJ_GEM_AMBER:
			return 70;
		default:
			return 0;
	endcase
endfunction




///////////////////
//  returns the Obsidian required for a particular gem type
///////////////////

function GetObsidianRequiredByGemtype (gemobjtype)
	case (gemobjtype)
		UOBJ_GEM_DIAMOND:
		UOBJ_GEM_SAPPHIRE:
		UOBJ_GEM_STAR_SAPPHIRE:
			return 50;
		UOBJ_GEM_EMERALD:
		UOBJ_GEM_RUBY:
		UOBJ_GEM_AMETHYST:
			return 30;
		UOBJ_GEM_CITRINE:
		UOBJ_GEM_TOURMALINE:
		UOBJ_GEM_AMBER:
			return 15;
		default:
			return 0;
	endcase
endfunction




///////////////////
//  Figures out the maximum stat boost possible
///////////////////

function GetMaxStatBoost (gemtype)
	case (gemtype)
		UOBJ_GEM_DIAMOND:
		UOBJ_GEM_SAPPHIRE:
		UOBJ_GEM_STAR_SAPPHIRE:
			return 5;
		UOBJ_GEM_EMERALD:
		UOBJ_GEM_RUBY:
		UOBJ_GEM_AMETHYST:
			return 4;
		default:
			return 3;
	endcase
	return 3;
endfunction




///////////////////
//  Figures out the description of the stat boost
///////////////////

function GetStatBonusDescription (stat_bonus)
	case (stat_bonus)
		1:	return "polished";
		2:	return "glowing";
		3:	return "lustrous";
		4:	return "radiant";
		5:	return "shining";
		default:
			return "really screwed up";
	endcase
endfunction




///////////////////
//  Gets a description of the gem
///////////////////

function GetGemDescription (gemtype)
	case (gemtype)
		UOBJ_GEM_DIAMOND:
			return "diamond";
		UOBJ_GEM_SAPPHIRE:
			return "sapphire";
		UOBJ_GEM_STAR_SAPPHIRE:
			return "star sapphire";
		UOBJ_GEM_EMERALD:
			return "emerald";
		UOBJ_GEM_RUBY:
			return "ruby";
		UOBJ_GEM_AMETHYST:
			return "amethyst";
		UOBJ_GEM_CITRINE:
			return "citrine";
		UOBJ_GEM_TOURMALINE:
			return "tourmaline";
		UOBJ_GEM_AMBER:
			return "amber";
		default:
			return "unknown";
	endcase
endfunction




///////////////////
//  Gets the description of the actual item
///////////////////

function GetItemDescriptionOfJewelry (objtype)
	case (objtype)
		0x1085:
		0x1088:
		0x1089:
			return "necklace";
		0x108b:
			return "bead necklace";
		0x1086:
			return "bracelet";
		0x1087:
			return "earrings";
		0x108a:
			return "ring";
		0x0DF2:
		0x0DF3:
		0x0DF4:
		0x0DF5:
			return "wand";
		default:
			return "unknown";
	endcase

endfunction




///////////////////
//  used to make magic jewelry
///////////////////

function GetStatBoostTypeByGemtype (gemtype)
	case (gemtype)
		UOBJ_GEM_DIAMOND:
		UOBJ_GEM_EMERALD:
		UOBJ_GEM_CITRINE:
			return "str";
		UOBJ_GEM_SAPPHIRE:
		UOBJ_GEM_RUBY:
		UOBJ_GEM_TOURMALINE:
			return "dex";
		UOBJ_GEM_STAR_SAPPHIRE:
		UOBJ_GEM_AMETHYST:
		UOBJ_GEM_AMBER:
			return "int";
		default:
			return "error";
	endcase
	return "error";
endfunction




///////////////////
//  used to make magic jewelry
///////////////////

function GetStatBoostDescriptionByGemtype (gemtype)
	case (gemtype)
		UOBJ_GEM_DIAMOND:
		UOBJ_GEM_EMERALD:
		UOBJ_GEM_CITRINE:
			return "strength";
		UOBJ_GEM_SAPPHIRE:
		UOBJ_GEM_RUBY:
		UOBJ_GEM_TOURMALINE:
			return "agility";
		UOBJ_GEM_STAR_SAPPHIRE:
		UOBJ_GEM_AMETHYST:
		UOBJ_GEM_AMBER:
			return "intelligence";
		default:
			return "error";
	endcase
	return "error";
endfunction
