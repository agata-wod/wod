///////////////////
//  These functions relate to the menu that allows players to select
//  what blacksmithy item to make.  This was put in its own
//  include file for neatness sake
///////////////////

function BuildTailoringCraftingMenu (character, menu_cfg_file_name, crafting_cfg_file_name, start_menu_elem, menu_name)
	var menu_cfg_file := ReadConfigFile (menu_cfg_file_name);
	if (!menu_cfg_file)
		SendSysMessage (character, "Unable to open " + menu_cfg_file + " config file!");
		return 0;
	endif

	var elem := FindConfigElem (menu_cfg_file, start_menu_elem);
	if (!elem)
		SendSysMessage (character, "Unable to find " + start_menu_elem + " in menu file!");
		return 0;
	endif
	
	var crafting_cfg_file := ReadConfigFile (crafting_cfg_file_name);
	if (!crafting_cfg_file)
		SendSysMessage (character, "Unable to open " + crafting_cfg_file_name + " config file!");
		return 0;
	endif
	
	var itemdesc_cfg := ReadConfigFile (":*:itemdesc");
	if (!crafting_cfg_file)
		SendSysMessage (character, "Unable to open main itemdesc.cfg file!");
		return 0;
	endif
	
	var base_x := 50;
	var base_y := 20;

	//To start out with, figure out the menus
	var blank_gump_layout := Array {
		"nodispose",
		"nomove",
		"page 0",
		"resizepic " + (base_x) + " " + (base_y) + " 2600 200 400",
		"resizepic " + (base_x+200) + " " + (base_y) + " 2600 450 400",
		"resizepic " + (base_x) + " " + (base_y+400) + " 2600 650 125",
		"text " + (base_x+480) + " " + (base_y+415) + " 1322 9",
		"text " + (base_x+510) + " " + (base_y+440) + " 0 10",
		"gumppic " + (base_x+502) + " " + (base_y+465) + " 2444",
		"textentry " + (base_x+510) + " " + (base_y+467) + " 50 20 0 11 11"
	};

	var blank_gump_data := Array {
		menu_name,
		"Auto-Upgrade",
		"Don't",
		"Quality",
		"Exceptional",
		"Upgrade Material",
		"Same",
		"Any plain hides found",
		"Select",
		"Number to Make",
		"(1-5)",
		"-"				//This is the number to make
	};

	var submenu_array := GetConfigStringArray (elem, "Submenu");
	var gump_y_pos := base_y + 40;
	var string_counter := 12;
	
	var menu_array := array {}; //stores the list of names of the menus

	foreach submenu in submenu_array
		submenu := SplitWords (submenu);

		//if there's more than 3 entries, we need to patch the last one together again because its a multi-word string
		if ((submenu.size()) > 3)
			for i := 4 to (submenu.size())
				submenu[3] := submenu[3] + " " + submenu[i];
			endfor
		endif

		menu_array.append (submenu[2]);
		blank_gump_layout.append ("text " + (base_x+50) + " " + gump_y_pos + " 0 " + string_counter);
		blank_gump_data.append (submenu[3]);
		
		//check to see if there's any items in this category.  If there is, then add the button
		var sub_elem := FindConfigElem (menu_cfg_file, submenu[2]);
		var menu_entry_array := GetConfigStringArray (sub_elem, "Entry");
		if (menu_entry_array.size())
			blank_gump_layout.append ("button " + (base_x+35) + " " + (gump_y_pos+4) + " 2104 2103 1 0 " + (100+menu_array.size()));
		endif
		
		gump_y_pos := gump_y_pos + 22;
		string_counter := string_counter + 1;

		//check to see if the submenu has a submenu
		var sub_submenu_array := GetConfigStringArray (sub_elem, "Submenu");
		if (sub_submenu_array)
			foreach sub_submenu in sub_submenu_array
				sub_submenu := SplitWords (sub_submenu);
		
				//if there's more than 3 entries, we need to patch the last one together again because its a multi-word string
				if ((sub_submenu.size()) > 3)
					for i := 4 to (sub_submenu.size())
						sub_submenu[3] := sub_submenu[3] + " " + sub_submenu[i];
					endfor
				endif
		
				menu_array.append (sub_submenu[2]);
				blank_gump_layout.append ("text " + (base_x+70) + " " + gump_y_pos + " 0 " + string_counter);
				blank_gump_layout.append ("button " + (base_x+55) + " " + (gump_y_pos+4) + " 2104 2103 1 0 " + (100+menu_array.size()));
				blank_gump_data.append (sub_submenu[3]);
				gump_y_pos := gump_y_pos + 20;
				string_counter := string_counter + 1;
			endforeach
			gump_y_pos := gump_y_pos + 7;
		endif
	endforeach
	
	var current_menu := GetObjProperty (character, CSTR ("#last_" + menu_cfg_file_name));
	if (!current_menu)
		current_menu := menu_array[1];
	endif

	var number_to_make := GetObjProperty (character, "#tailoring_number_to_make");
	if (!number_to_make or number_to_make < 1)
		number_to_make := 1;
	elseif (number_to_make > 5)
		number_to_make := 5;
	endif

	var auto_upgrade_options := GetObjProperty (character, "#tailoring_autoupgrade_options");
	if (!auto_upgrade_options)
		auto_upgrade_options := array {1, 0, 0, 0, 1, 0};
	endif

	repeat
		//add the basic layout stuff
		var gump_layout := blank_gump_layout;
		var gump_data := blank_gump_data;

		var menu_objtype_array := array {}; //stores the objtypes of the items on the current_menu

		var current_elem := FindConfigElem (menu_cfg_file, current_menu);
		var entry_array := GetConfigStringArray (current_elem, "Entry");
		if (entry_array)
			gump_y_pos := base_y + 40;
			var current_column_number := 1;
			var gump_x_pos := base_x + 250;
			
			string_counter := gump_data.size();

			foreach entry in entry_array
				entry := SplitWords (entry);
				menu_objtype_array.append (CINT (entry[1]));

				//if there's more than 2 entries, we need to patch the last one together again because its a multi-word string
				if ((entry.size()) > 2)
					for i := 3 to (entry.size())
						entry[2] := entry[2] + " " + entry[i];
					endfor
				endif
				
				//fancy box (hiding the button) with the item's picture in it
				gump_layout.append ("button " + (gump_x_pos+8) + " " + (gump_y_pos+8) + " 2240 2240 1 0 " + (200+(menu_objtype_array.size())));
				gump_layout.append ("resizepic " + gump_x_pos + " " + gump_y_pos + " 5120 60 60");

				var item_graphic := CINT (entry[1]);

				var item_itemdesc_entry := FindConfigElem (itemdesc_cfg, item_graphic);
				if (item_itemdesc_entry)
					if (item_itemdesc_entry.graphic)
						item_graphic := CINT (item_itemdesc_entry.graphic);
					endif
				endif
				
				gump_layout.append ("tilepic " + (gump_x_pos+10) + " " + (gump_y_pos+10) + " " + item_graphic);

				//Open the item's crafting.cfg elem
				var crafting_elem := FindConfigElem (crafting_cfg_file, entry[1]);
				if (!crafting_elem)
					SendSysMessage (character, "Error: unable to find objtype " + entry[1] + "'s entry in crafting file");
				else
					gump_layout.append ("text " + (gump_x_pos+70) + " " + gump_y_pos + " 1322 " + string_counter);
					gump_data.append (entry[2]);
					string_counter := string_counter + 1;

					gump_layout.append ("text " + (gump_x_pos+75) + " " + (gump_y_pos+20) + " 0 " + string_counter);
					gump_data.append ("Skill: " + crafting_elem.skill);
					string_counter := string_counter + 1;

					gump_layout.append ("text " + (gump_x_pos+75) + " " + (gump_y_pos+40) + " 0 " + string_counter);
					gump_data.append ("Mat: " + crafting_elem.material);
					string_counter := string_counter + 1;
				endif
				
				current_column_number := current_column_number + 1;
				if (current_column_number > 2)
					gump_x_pos := base_x + 250;
					gump_y_pos := gump_y_pos + 80;
					current_column_number := 1;
				else
					gump_x_pos := gump_x_pos + 200;
				endif
			endforeach
		endif
		
		//See if we need to add the upgrade options
		var show_upgrade_options := current_elem.show_upgrade_options;
		if (show_upgrade_options)
			gump_layout.append ("text " + (base_x+66) + " " + (base_y+20) + " 0 0");
			gump_layout.append ("text " + (base_x+80) + " " + (base_y+415) + " 1322 1");
			gump_layout.append ("text " + (base_x+100) + " " + (base_y+440) + " 0 2");
			gump_layout.append ("text " + (base_x+100) + " " + (base_y+462) + " 0 3");
			gump_layout.append ("text " + (base_x+100) + " " + (base_y+484) + " 0 4");
			gump_layout.append ("text " + (base_x+280) + " " + (base_y+415) + " 1322 5");
			gump_layout.append ("text " + (base_x+300) + " " + (base_y+440) + " 0 6");
			gump_layout.append ("text " + (base_x+300) + " " + (base_y+462) + " 0 7");
			gump_layout.append ("text " + (base_x+300) + " " + (base_y+484) + " 0 8");
		
			//add the checkboxes
			gump_layout.append ("group 0");
			gump_layout.append ("radio " + (base_x+80) + " " + (base_y+440) + " 208 209 " + auto_upgrade_options[1] + " 301");
			gump_layout.append ("radio " + (base_x+80) + " " + (base_y+462) + " 208 209 " + auto_upgrade_options[2] + " 302");
			gump_layout.append ("radio " + (base_x+80) + " " + (base_y+484) + " 208 209 " + auto_upgrade_options[3] + " 303");
	
			gump_layout.append ("group 1");
			gump_layout.append ("radio " + (base_x+280) + " " + (base_y+440) + " 208 209 " + auto_upgrade_options[4] + " 304");
			gump_layout.append ("radio " + (base_x+280) + " " + (base_y+462) + " 208 209 " + auto_upgrade_options[5] + " 305");
			gump_layout.append ("radio " + (base_x+280) + " " + (base_y+484) + " 208 209 " + auto_upgrade_options[6] + " 306");
		
		endif

		//number to make
		gump_data[12] := CSTR (number_to_make);

		//send the gump
		var gump_return := SendDialogGump (character, gump_layout, gump_data);
		if (gump_return == error)
			SendSysMessage (character, "Error:  Close other gumps first");
			return 0;
		elseif (!gump_return[0] or gump_return[0] == 1)
			return 0;
		endif
		
		//check the check boxes
		if (show_upgrade_options)
			for i := 301 to 306
				if (gump_return[i])
					auto_upgrade_options[i-300] := 1;
				else
					auto_upgrade_options[i-300] := 0;
				endif
			endfor
		endif
		
		//figure out the number to make
		var tempnumber := CSTR (gump_return[11]);
		if (tempnumber)
			tempnumber["11: "] := "";
			tempnumber := CINT (tempnumber);
			if (!tempnumber or tempnumber < 1)
				number_to_make := 1;
			elseif (tempnumber > 5)
				number_to_make := 5;
			else
				number_to_make := tempnumber;
			endif
		else
			number_to_make := 1;
		endif

		//figure out what button was pressed
		if (gump_return[0] > 200)
			SetObjProperty (character, CSTR ("#last_" + menu_cfg_file_name), current_menu);
			SetObjProperty (character, "#tailoring_autoupgrade_options", auto_upgrade_options);
			SetObjProperty (character, "#tailoring_number_to_make", number_to_make);
			var selected_item := menu_objtype_array[(gump_return[0]-200)];
			if (selected_item)
				var upgrade_level := 0;
				if (auto_upgrade_options[2])
					upgrade_level := 1;
				elseif (auto_upgrade_options[3])
					upgrade_level := 2;
				endif

				var upgrade_material_selection := 0;
				if (auto_upgrade_options[5])
					upgrade_material_selection := 1;
				elseif (auto_upgrade_options[6])
					upgrade_material_selection := 2;
				endif

				return array {selected_item, upgrade_level, upgrade_material_selection, number_to_make};
			else
				SendSysMessage (character, "Error:  Invalid gump return, no item");
				return 0;
			endif
		elseif (gump_return[0] > 100)
			current_menu := menu_array[gump_return[0] - 100];
			if (!current_menu)
				SendSysMessage (character, "Error: Invalid gump_return");
				return 0;
			endif
		else
			SendSysMessage (character, "Error: Invalid gump_return");
			return 0;
		endif
		

	until (!character);
endfunction







