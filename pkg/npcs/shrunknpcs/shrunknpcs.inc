///////////////////
//  Shrink the targetted NPC, turning it into a totem stored in given container
///////////////////

function ShrinkNPC (mobile, container)
	if (!mobile or !container)
		return CreateError ("Attempted to shink a non-existant NPC");
	elseif (!mobile.npctemplate)
		return CreateError ("Mobile is not an NPC");
	endif

	var totem := CreateItemInContainer (container, 0x170bc, 1);
	if (!totem)
		return CreateError ("Could not create totem - full pack");
	endif

	//This property lets us insert items into the totem.  Prevents problems
	SetObjProperty (totem, "#addingitems", 1);

	SetObjProperty (totem, "npctemplate", mobile.npctemplate);
	SetObjProperty (totem, "script", mobile.script);
	SetObjProperty (totem, "graphic", mobile.graphic);
	totem.color := mobile.color;
	totem.name := mobile.name;
	totem.newbie := 1;

	var trackingcfg := ReadConfigFile (":tracking:tracking");
	var elem := FindConfigElem (trackingcfg, mobile.graphic);
	if (elem)
		totem.graphic := elem.tile;
	else
		totem.graphic := 0x3a;
	endif

	foreach property in GetObjPropertyNames (mobile)
		var value := GetObjProperty (mobile, property);
		SetObjProperty (totem, property, value);
	endforeach

	var equippack := CreateItemInContainer (totem, UOBJ_BACKPACK, 1);
	SetObjProperty (totem, "equippack", equippack.serial);
	foreach item in ListEquippedItems (mobile)
		if (item != mobile.backpack)
			CopyItemInContainer (item, equippack);
		endif
	endforeach

	var lootbag := CreateItemInContainer (totem, UOBJ_BACKPACK, 1);
	SetObjProperty (totem, "lootbag", lootbag.serial);
	foreach item in ListRootItemsInContainer (mobile.backpack)
		CopyItemInContainer (item, lootbag);
	endforeach

	SetObjProperty (totem, "str", CINT (GetAttributeBaseValue (mobile, "Strength")/10));
	SetObjProperty (totem, "int", CINT (GetAttributeBaseValue (mobile, "Intelligence")/10));
	SetObjProperty (totem, "dex", CINT (GetAttributeBaseValue (mobile, "Dexterity")/10));

	SetObjProperty (totem, "str_mod", CINT (GetAttributeTemporaryMod (mobile, "Strength")/10));
	SetObjProperty (totem, "int_mod", CINT (GetAttributeTemporaryMod (mobile, "Intelligence")/10));
	SetObjProperty (totem, "dex_mod", CINT (GetAttributeTemporaryMod (mobile, "Dexterity")/10));
	SetObjProperty (totem, "ar_mod", mobile.ar_mod);

	var skills := array{};
	for i := 0 to 57
		skills[i] := GetAttribute (mobile, GetAttributeIDBySkillID (i));
	endfor

	SetObjProperty (totem, "skills", skills);
	SetObjProperty (totem, "gender", mobile.gender);
	
	if (mobile.title_prefix)
		SetObjProperty (totem, "title_prefix", mobile.title_prefix);
	endif
	if (mobile.title_suffix)
		SetObjProperty (totem, "title_suffix", mobile.title_suffix);
	endif
	if (mobile.title_race)
		SetObjProperty (totem, "title_race", mobile.title_race);
	endif
	if (mobile.title_guild)
		SetObjProperty (totem, "title_guild", mobile.title_guild);
	endif

	//prevent more items from being added to the totem
	EraseObjProperty (totem, "#addingitems");
	return totem;
endfunction




///////////////////
//  Unshrinks the totem at the specified location
///////////////////

function UnshrinkNPC (totem, x, y, z := 0, rlm)
	return run_script_to_completion (":shrunknpcs:unshrink_totem", array{totem, x, y, z, rlm});
endfunction






