///////////////////
//
//  Contains assorted functions related to crafting
//
///////////////////




///////////////////
//  Tries to find the bag that crafted items will go into, if not the character's main backpack.
//  Set with .options
///////////////////

function FindCraftingBag (character)
	//Make absolutely certain that they have a backpack
	if (!character.backpack)
		var newbackpack := CreateItemAtLocation (5288, 1176, 0, UOBJ_BACKPACK, 1, REALM_BRITANNIA);
		if (!EquipItem (character, newbackpack))
			DestroyItem (newbackpack);
		endif
	endif

	var craftingbagserial := GetObjProperty (character, "craftingbagserial");
	if (!craftingbagserial)
		return character.backpack;
	endif
	
	foreach item in EnumerateItemsInContainer (character.backpack)
		if (item.serial == craftingbagserial)
			if (item.isa (POLCLASS_CONTAINER))
				return item;
			endif
		endif
	endforeach

	//Still can't find it
	EraseObjProperty (character, "craftingbagserial");
	return character.backpack;
endfunction




///////////////////
//  Allows the user to select what to make, based on the passed menu.cfg file and
//  crafting cfg file (so that material and skill requirements can be displayed)
///////////////////

function SelectWhatToMakeFromMenu ( character, main_menu, crafting_cfg_file, character_skill, character_material, menuname)
	var menu_cfg_file := ReadConfigFile ("menu");
	var elem := FindConfigElem (menu_cfg_file, main_menu);
	if (!elem)
		SendSysMessage (character, "Unable to create menu.");
		return 0;
	endif

	var submenu_array := GetConfigStringArray (elem, "Submenu");
	var entry_array := GetConfigStringArray (elem, "Entry");
	var menu_items_array := array { };
	var select_menu := CreateMenu (menuname);
	var i := 0;

	foreach submenu in submenu_array
		submenu := SplitWords (submenu);
		//if there's more than 3 entries, we need to patch the last one together again because its a multi-word string
		if ( len(submenu) > 3 )
			for ( i := 4 ; i <= len(submenu); i := i + 1 )
				submenu[3] := submenu[3] + " " + submenu[i];
			endfor
		endif
		menu_items_array.append ( submenu [2] );
		AddMenuItem (select_menu, cint(submenu[1]), submenu[3]);
	endforeach

	foreach entry in entry_array
		entry := SplitWords (entry);
		//if there's more than 2 entries, we need to patch the last one back together
		if (len (entry) > 2)
			for ( i := 3 ; i <= len(entry); i := i + 1 )
				entry[2] := entry[2] + " " + entry[i];
			endfor
		endif
		var material_needed := cint (crafting_cfg_file [entry[1]].material);
		var skill_needed :=  cint (crafting_cfg_file [entry[1]].skill);
		if ( (character_skill >= (skill_needed-20) ) and (character_material >= material_needed) )
			menu_items_array.append ("object");
			AddMenuItem (select_menu, cint(entry[1]), entry[2] + " ( " + skill_needed + " / " + material_needed +" )");
		endif
	endforeach

	//if there's nothing on the menu, end
	if (!len (menu_items_array))
		SendSysMessage (character, "You can't make anything on that menu.");
		return 0;
	endif

	//display the menu
	var selected_item := SelectMenuItem2 (character, select_menu);
	if (!selected_item)
		SendSysMessage (character, "Canceled.");
		return 0;
	endif

	//if they picked something that's a submenu, recurse this function to allow them to select what they
	//want from that menu.  otherwise return the the objtype of their selection
	if (menu_items_array [selected_item.index] != "object")
		var new_menu := menu_items_array [selected_item.index];
		return SelectWhatToMakeFromMenu (character, new_menu, crafting_cfg_file, character_skill, character_material, menuname);
	else
		return selected_item.objtype;
	endif
endfunction





