include "include/locations";
include "../pkg/combat/loot/loot";

//Const UOBJECT_TREASURE_CHEST := 0x0E40;
Const UOBJECT_TREASURE_CHEST := 0x4106;

///////////////////
//  called by mining when a map is targetted
///////////////////

function TryToDigUpTreasureMap (character, map)
	if (!ReserveItem (map))
		SendSysMessage (character, "Someone else seems to be using that map already.");
		return;
	endif

	var chestx := GetObjProperty (map, "chestx");
	var chesty := GetObjProperty (map, "chesty");
	var chestr := GetObjProperty (map, "chestr");
	if (!chestr)
		chestr := _DEFAULT_REALM;
	endif

	if(!chestx or !chesty)	//z might be 0!
		SendSysmessage (character, "That is not a treasure map.");
		return;
	endif

	//if this isn't the person who decoded the map, they need to do it again
	if (character.serial != GetObjProperty (map, "decoder") )
		DecodeMapAgain (character, map);
		return;
	endif
	
	if (chestr != character.realm)
		SendSysmessage (character, "This map belongs to a different realm");
		return;
	endif

	var range := GetAttribute (character, ATTRIBUTEID_MINING)/10 + 1;
	if (!range)
		range := 2;
	elseif (range > 10)
		range := 10;
	endif

	var distn := CoorDist (character.x, character.y, chestx, chesty);
	var direction := GetDirection (character, chestx, chesty);

	if (distn > range)
		SendSysmessage (character, "This map leads " + direction);
		return;
	endif

	var createdx := character.x;
	var createdy := character.y;

	var chest := FindOrCreateTreasureChest (map, createdx, createdy, character.z - 5, character.realm);
	if (!chest) 
		SendSysmessage (character, "This map is a fake!");
		DestroyItem (map);
		return;
	endif

	if (!ReserveItem (chest))
		return;
	endif
	var z := character.z;

	var i;
	for (i := 5; i >= 0; i := i - 1)
		PerformAction (character, 0x0B);
		PlaySoundEffect (character, 0x126);
		sleep (1);
		chest.movable := 1;
		MoveObjectToLocation (chest, createdx, createdy, z - i, character.realm, MOVEOBJECT_FORCELOCATION);
		chest.movable := 0;
	endfor

	DestroyItem (map);
	ReleaseItem (chest);
	detach ();
	sleep (1800);
	DestroyItem (chest);
endfunction




///////////////////
//  decodes an already decoded map, if the person who's digging it up isn't the person who decoded it
//  (Anti-mule code)
///////////////////

function DecodeMapAgain (character, map)
	var maplevel := GetObjProperty (map, "level");
	var diffi := maplevel * 10;

	if (diffi > GetAttribute (character, ATTRIBUTEID_CARTOGRAPHY)+15)
		SendSysmessage (character, "You cannot comprehend the map.");
		return;
	endif

	if (!CheckSkill (character, SKILLID_CARTOGRAPHY, diffi, 0))
		SendSysmessage (character , "You fail to decode the map...");
		return;
	endif

	SetObjProperty (map, "decoder", character.serial);
	SendSysmessage (character, "You successfully decode the map.");
endfunction




///////////////////
//  creates a treasure chest, or, for seer created maps, finds the pre-buried one
///////////////////

function FindOrCreateTreasureChest (map, x, y, z, rlm)
	var chest_serial := GetObjProperty (map, "chestserial");
	if (chest_serial)
		var storage_area := find_or_create_storage ("seer_chest_storage");
		if (storage_area)
			var storagebox := find_or_create_item (storage_area, CSTR ("map_" + map.serial), 0xE7C);
			if (storagebox)
				foreach item in EnumerateItemsInContainer (storagebox)
					if (item.serial == chest_serial)
						item.usescript := ":treasuremap:treasurechest";
						
						//put the guardian mob pack in the chest to spring later
						var guardian_bag_serial := GetObjProperty (item, "guardian_pack");
						if (guardian_bag_serial)
							var guardian_bag;
							foreach newitem in EnumerateItemsInContainer (storagebox)
								if (newitem.serial == guardian_bag_serial)
									guardian_bag := newitem;
									break;
								endif
							endforeach
							if (guardian_bag)
								MoveItemToContainer (guardian_bag, item);
							endif
						endif
						return item;
					endif
				endforeach
			endif
		endif
		
		//old seer map system, kept (for now) for backwards-compatibility
		var chest := SystemFindObjectBySerial (chest_serial);
		if (chest)
			chest.usescript := ":treasuremap:treasurechest";
			return chest;
		endif
	endif

	//There wasn't a previously defined map, or else if it couldn't be found
	var chest := CreateItemAtLocation (x, y, z, UOBJECT_TREASURE_CHEST, 1, rlm);
	var lvl := GetObjProperty (map, "level");
	if (!lvl)
		lvl := 2;
	elseif (lvl > 10)
		lvl := 10;
	endif
	
	if (IsExtraToughArea (x, y, rlm))
		lvl := lvl + 2;
	endif
	
	SetObjProperty (chest, "level", lvl);
	chest.locked := 1;
	var lootgroup := CSTR ("treasuremapchest" + lvl);
	MakeLootInChest (chest, lootgroup);

	chest.usescript := ":treasuremap:treasurechest";
	return chest;
endfunction




///////////////////
//  Some areas of the map are extra-tough, so give more loot and more monsters
///////////////////

function IsExtraToughArea (x, y, rlm)
	//Buc Den
	if (x > 2550 and x < 3000 and y > 1900 and y < 2400 and rlm == _DEFAULT_REALM)
		return 1;
	endif
	return 0;
endfunction
