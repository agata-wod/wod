use uo;
use os;
use util;
use cfgfile;
use math;

include "include/client";
include "include/objtype";
include "../pkg/npcs/npc_util";

var npc_cfg_file := ReadConfigFile (":*:npc_lootgroups");
var chest_cfg_file := ReadConfigFile (":*:chest_lootgroups");
var itemgroup_cfg_file := ReadConfigFile (":loot:itemgroups");

///////////////////
//  used to make loot on corpses.  Probably only called by death.src.  Lootgroups for NPCs
//  can be found in npc_lootgroups.cfg
///////////////////

function MakeLootOnCorpse (corpse, npctemplate)
	var elem := FindConfigElem (npc_cfg_file, npctemplate);
	if (!elem)
		return 0;
	endif
  

	if (elem.gold)
		var gold := RandomDiceRoll (elem.gold);
		CreateItemInContainer (corpse, UOBJ_GOLD_COIN, gold);
	endif

	//Christmas quest thing
	if (elem.christmaspresent)
		CreateChristmasPresent (corpse);
	endif

	if (elem.chest_chance)
		MakeLockedChest (corpse, elem.chest_chance, elem.chest_quality);
	endif

	foreach str in GetConfigStringArray (elem, "item")
		MakeSpecifiedLootItem (corpse, str);
	endforeach

	foreach str in GetConfigStringArray (elem, "random")
		MakeItemsOutOfGroup (corpse, str);
	endforeach

	var quality := elem.magic_quality;
	if (quality)
		foreach str in GetConfigStringArray (elem, "magic_group")
			var temp_str := SplitWords (str);
			var group := temp_str [1];
			var chance := CInt (temp_str [2]);
			if (RandomInt (1000) + 1 <= chance)
				MakeMagicItemOutOfGroup (corpse, group, quality);
			endif
		endforeach
	endif
	return 1;
endfunction




///////////////////
//  used to make loot in chests.  Lootgroups can be found in chest_lootgroups.cfg
///////////////////

function MakeLootInChest (chest, lootgroup)

	var elem := FindConfigElem (chest_cfg_file, lootgroup);
	if (!elem)
		return 0;
	endif

	if (elem.gold)
		var gold := RandomDiceRoll (elem.gold);
		CreateItemInContainer (chest, UOBJ_GOLD_COIN, gold);
	endif

	//Christmas quest thing
	if (elem.christmaspresent)
		CreateChristmasPresent (chest);
	endif

	foreach str in GetConfigStringArray (elem, "item")
		MakeSpecifiedLootItem (chest, str);
	endforeach

	foreach str in GetConfigStringArray (elem, "random")
		MakeItemsOutOfGroup (chest, str);
	endforeach

	var quality := elem.magic_quality;
	if (quality)
		foreach str in GetConfigStringArray (elem, "magic_group")
			var temp_str := SplitWords (str);
			var group := temp_str [1];
			var chance := CInt (temp_str [2]);
			if (RandomInt (1000) + 1 <= chance)
				MakeMagicItemOutOfGroup (chest, group, quality);
			endif
		endforeach
	endif
	return 1;

endfunction




///////////////////
//  creates specific item in container
///////////////////

function MakeSpecifiedLootItem (container, str)
	str := SplitWords (str);
	var itemname := str[1];
	var number := RandomDiceRoll (str[2]);
	var chance := CInt(str[3]);
  
	if (number > 20)
		number := 20;
	endif
  case (GetObjTypeByName(itemname))
      UOBJ_HIDE:
      UOBJ_BROWN_HIDE:
      UOBJ_BLUE_HIDE:
      UOBJ_RED_HIDE:
      UOBJ_GREEN_HIDE:
      UOBJ_SILVER_HIDE:
      var character := FindTopLevelItem(container);
        if (character.acctname)
          if (character.race == RACE_HUMAN)
            number := number + Ceil(number/4);
          endif
      endif
    endcase
  
	for i := 1 to number
		if (RandomInt(100) + 1 <= chance)
			var item := CreateItemInContainer (container, itemname, 1);
			item.movable := 1;
		endif
	endfor
	return 1;

endfunction

function FindTopLevelItem( item )
    var res := item.container;
    while(res.container)
        res := res.container;        
    endwhile
    return res;
endfunction


///////////////////
//  creates random items out of a group of possible items
///////////////////

function MakeItemsOutOfGroup (container, str)
	str := SplitWords (str);
	var groupname := str[1];
	var number := RandomDiceRoll (str[2]);
	var chance := CInt(str[3]);

	if (number > 100)
		number := 100;
	endif

	var elem := FindConfigElem (itemgroup_cfg_file, groupname);
	if (!elem)
		return 0;
	endif

	var possible_items_array := GetConfigStringArray (elem, "item");
	var total_items := len (possible_items_array);
	var item, itemtype, itemnumber;
	for i := 1 to number
		if (RandomInt (100) + 1 <= chance)
			itemnumber := RandomInt (total_items) + 1;
			itemtype := possible_items_array [itemnumber];
			if (CINT (itemtype))
				itemtype := CINT (itemtype);
			endif
			item := CreateItemInContainer (container, itemtype, 1);
			item.movable := 1;
		endif
	endfor
	return item;

endfunction




///////////////////
//  makes a magic item out of the group specified
///////////////////

function MakeMagicItemOutOfGroup (container, str, quality)

	var elem := FindConfigElem (itemgroup_cfg_file, str);
	if (!elem)
		return 0;
	endif

	var possible_items_array := GetConfigStringArray (elem, "item");
	var total_items := len (possible_items_array);
	var itemnumber := RandomInt (total_items) + 1;
	var itemtype := possible_items_array [itemnumber];

	var item := CreateItemInContainer (container, itemtype, 1);
	item.movable := 1;

	var parms := array {};
	parms[1] := item;
	parms[2] := CInt (quality);
	parms[3] := 1;				//whether the created magic item starts out damaged

	run_script_to_completion (":loot:makeitemmagic", parms);
	return item;

endfunction




///////////////////
//  makes a locked chest that, when picked, spawns loot
///////////////////

function MakeLockedChest (container, chest_chance, chest_quality)

	if (chest_chance < RandomInt (100))
		return 0;
	endif

	var base_quality := CINT(chest_quality/2) + RandomInt(CINT(chest_quality/2));
	var random_quality := CINT (((RandomInt (10)+1) * (RandomInt (10)+1))/10) + 1;
	if (random_quality > base_quality)
		base_quality := random_quality;
	endif

	if (base_quality > 10)
		base_quality := 10;
	endif

	var chest := CreateItemInContainer (container, 0x17083, 1);
	if (!chest)
		return 0;
	endif

	SetObjProperty (chest, "mobchest_spawntime", ReadGameClock ());
	SetObjProperty (chest, "lockpicking", base_quality);
	SetObjProperty (chest, "questitem", 1);
	chest.locked := 1;
	chest.movable := 1;
	chest.name := "a locked chest";

	//make traps
	if (RandomInt (3) == 1)
		case (RandomInt (3))
			0:
				SetObjProperty (chest, "trap", "explosion");
			1:
				SetObjProperty (chest, "trap", "poison");
			2:
				SetObjProperty (chest, "trap", "djinni");
		endcase
	endif

	return 1;

endfunction


function CreateChristmasPresent (corpse)
	var present := CreateItemInContainer (corpse, 0xBE35, 1);
	if (!present)
		return;
	endif
	
	present.color := RandomInt (500) + 1;
	present.graphic := 10290 + RandomInt (8);
	
        //this variable not used right now
	//var npcnamegroup := 0;
        

	case (RandomInt (20))
		0:
		1:
		2:
		3:
		4:	//player names
			var tries := 0;
			var accountlist := ListAccounts ();
			var listlen := accountlist.size();
			var an_account, a_character;
			repeat
				an_account := accountlist[RandomInt (listlen)+1];
				if (an_account)
					a_character := an_account.GetCharacter (RandomInt(5)+1);
					if (a_character)
						present.name := "a present for " + a_character.name;
						return;
					endif
				endif
				tries := tries + 1;
			until (tries > 10);
			//otherwise, use a random NPC name
			present.name := "a present for " + RandomNPCName (0);
		5:
		6:
		7:
		8:
		9:	//Random NPC names
			present.name := "a present for " + RandomNPCName (0);
		10:
		11:	//Evil NPCs get presents too
			var presentdesc := "a present for " + RandomNPCName (3);
			case (RandomInt (5))
				0:	presentdesc := presentdesc + " the great";
				1:	presentdesc := presentdesc + " the great";
				2:	presentdesc := presentdesc + " the ancient";
				3:	presentdesc := presentdesc + " the royal";
				//4 is no ending
			endcase
			present.name := presentdesc;
		12:
		13:	//lizardmen
			var presentdesc := "a present for " + RandomNPCName (9);
			case (RandomInt (10))
				0:	presentdesc := presentdesc + " the lizardman";
				1:	presentdesc := presentdesc + " the lizardman warrior";
				2:	presentdesc := presentdesc + " the lizardman defender";
				3:	presentdesc := presentdesc + " the lizardman subjugator";
				4:	presentdesc := presentdesc + " the lizardman overseer";
				5:	presentdesc := presentdesc + " the lizardman archer";
				6:	presentdesc := presentdesc + " the lizardman marksman";
				7:	presentdesc := presentdesc + " the lizardman shaman";
				//8 is no ending
			endcase
			present.name := presentdesc;
		14:
		15:	//orcs
			var presentdesc := "a present for " + RandomNPCName (10);
			case (RandomInt (15))
				0:	presentdesc := presentdesc + " the orc warrior";
				1:	presentdesc := presentdesc + " the orc warrior";
				2:	presentdesc := presentdesc + " the orc captain";
				2:	presentdesc := presentdesc + " the Orc Dragon Defender";
				3:	presentdesc := presentdesc + " the Orcish Lord";
				4:	presentdesc := presentdesc + " the Orcish king";
				5:	presentdesc := presentdesc + " the archer";
				6:	presentdesc := presentdesc + " the marksman";
				7:	presentdesc := presentdesc + " the Orcmage";
				8:	presentdesc := presentdesc + " the snoworc";
				9:	presentdesc := presentdesc + " the snoworc";
				10:	presentdesc := presentdesc + " the snoworc warrior";
				//8 is no ending
			endcase
			present.name := presentdesc;
		16:
		17:	//ratmen
			var presentdesc := "a present for " + RandomNPCName (12);
			case (RandomInt (6))
				0:	presentdesc := presentdesc + " the ratman";
				1:	presentdesc := presentdesc + " the ratman";
				2:	presentdesc := presentdesc + " the warrior";
				3:	presentdesc := presentdesc + " the ratman marksman";
				4:	presentdesc := presentdesc + " the shaman";
				//rest are no ending
			endcase
			present.name := presentdesc;
		18:	//Daemons
			present.name := "a present for " + RandomNPCName (2);
		19:	//Evil humans
			case (RandomInt (7))
				0: present.name := "a present for Sister" + RandomNPCName (6);
				1: present.name := "a present for Sister" + RandomNPCName (6);
				2: present.name := "a present for " + RandomNPCName (6) + " the pirate";
				3: present.name := "a present for " + RandomNPCName (6) + " the pirate";
				4: present.name := "a present for " + RandomNPCName (6) + " the pirate brute";
				5: present.name := "a present for " + RandomNPCName (6) + " the pirate captain";
				6: present.name := "a present for " + RandomNPCName (6) + " the brigand";
			endcase
	endcase
	
	

endfunction