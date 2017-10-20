include "include/string";

///////////////////
//  This function returns a dictionary of all foodstuffs used in cooking.  the dictionary is used to look
//  up the objtypes of names given to food.  it's used to make defining recipes easier.
///////////////////

function define_food_dictionary ()
	var food := dictionary;

	//some tools used in cooking
	food [ "ITEM_MIXING_BOWL" ]			:= 0x15f8;
	food [ "ITEM_ROLLING_PIN" ]			:= 0x1043;
	food [ "ITEM_FRYING_PAN" ]			:= 0x097f;
	food [ "ITEM_EMPTY_PITCHER" ]			:= 0x09a7;
	food [ "ITEM_EMPTY_GLASS" ]			:= 0x1f81;
	food [ "ITEM_EMPTY_BOTTLE" ]			:= 0x0F0E;

	//bottles, pitchers and glasses of liquid
	food [ "ITEM_BOTTLE_OF_LIQUOR" ]		:= 0x099B;
	food [ "ITEM_BOTTLE_OF_ALE" ]			:= 0x099F;
	food [ "ITEM_BOTTLE_OF_WINE" ]		:= 0x09C7;
	food [ "ITEM_JUG_OF_CIDER" ]			:= 0x09c8;
	
	food [ "ITEM_BOTTLE_MELON_LIQUOR" ]	:= 0xC960;
	food [ "ITEM_BOTTLE_PEACH_LIQUOR" ]	:= 0xC961;
	food [ "ITEM_BOTTLE_OF_BRANDY" ]		:= 0xC962;
	food [ "ITEM_BOTTLE_OF_EGGNOG" ]		:= 0xC963;

	food [ "ITEM_PITCHER_OF_LIQUOR" ]		:= 0x1f9a;
	food [ "ITEM_PITCHER_OF_ALE" ]		:= 0x1f96;
	food [ "ITEM_PITCHER_OF_WINE" ]		:= 0x1f9c;
	food [ "ITEM_PITCHER_OF_CIDER" ]		:= 0x1f98;
	food [ "ITEM_PITCHER_OF_WATER" ]		:= 0x1f9e;
	food [ "ITEM_PITCHER_OF_MILK" ]		:= 0x09ad;

	food [ "ITEM_GLASS_OF_LIQUOR" ]		:= 0x1F85;
	food [ "ITEM_GLASS_OF_ALE" ]			:= 0x09EE;
	food [ "ITEM_GLASS_OF_WINE" ]			:= 0x1F8D;
	food [ "ITEM_GLASS_OF_CIDER" ]		:= 0x1F7D;
	food [ "ITEM_GLASS_OF_WATER" ]		:= 0x1F91;
	food [ "ITEM_GLASS_OF_MILK" ]			:= 0x1F89;

	//These items are special.  If they're called for in a recipe, it will call a special function that will
	//permit multiple forms of that item
	food [ "ITEM_GENERAL_WATER" ]		:= "water";
	food [ "ITEM_GENERAL_MILK" ]		:= "milk";
	food [ "ITEM_GENERAL_CHEESE" ]	:= "cheese";

	//primary food ingredients
	food [ "ITEM_FLOUR" ]			:= 0x1045;
	food [ "ITEM_CORNMEAL" ]			:= 0xc800;
	food [ "ITEM_RAW_RIB" ]			:= 0x09f1;
	food [ "ITEM_RAW_FISHSTEAK" ]		:= 0x097a;
	food [ "ITEM_RAW_BIRD" ]			:= 0x09b9;
	food [ "ITEM_RAW_HAM" ]			:= 0x09c9;
	food [ "ITEM_BACON_SLAB" ]		:= 0x0976;
	food [ "ITEM_RAW_SAUSAGE" ]		:= 0x09c0;
	food [ "ITEM_GREEN_FISH" ]		:= 0x09cc;
	food [ "ITEM_BROWN_FISH" ]		:= 0x09cd;
	food [ "ITEM_BLUE_FISH" ]		:= 0x09ce;
	food [ "ITEM_YELLOW_FISH" ]		:= 0x09cf;
	food [ "ITEM_LARD" ]			:= 0x1e8a;
	food [ "ITEM_EGG" ]				:= 0x09b5;
	food [ "ITEM_GARLIC" ]			:= 0x0f84;
	food [ "ITEM_HONEY" ]			:= 0x09ec;
	food [ "ITEM_CARROT" ]			:= 0x0c77;
	food [ "ITEM_APPLE" ]			:= 0x09d0;
	food [ "ITEM_ORANGE" ]			:= 0xc807;
	food [ "ITEM_PEACH" ]			:= 0x172c;
	food [ "ITEM_LEMON" ]			:= 0x1728;
	food [ "ITEM_LIME" ]			:= 0x172a;
	food [ "ITEM_BANANA" ]			:= 0x171f;
	food [ "ITEM_PEAR" ]			:= 0x172d;
	food [ "ITEM_GRAPES" ]			:= 0x0d1a;
	food [ "ITEM_DATES" ]			:= 0x1727;
	food [ "ITEM_PUMPKIN" ]			:= 0x0c6a;
	food [ "ITEM_YELLOW_GOURD" ]		:= 0x0c65;
	food [ "ITEM_GREEN_GOURD" ]		:= 0x0c66;
	food [ "ITEM_SQUASH" ]			:= 0x0c72;
	food [ "ITEM_WATERMELON" ]		:= 0x0c5c;
	food [ "ITEM_HONEYDEW_MELON" ]	:= 0x0c75;
	food [ "ITEM_CANTELOUPE" ]		:= 0x0c7a;
	food [ "ITEM_LETTUCE" ]			:= 0x0c70;
	food [ "ITEM_CABBAGE" ]			:= 0x0c7c;
	food [ "ITEM_ONION" ]			:= 0x0c6d;
	food [ "ITEM_TURNIP" ]			:= 0x0d3a;
	food [ "ITEM_COCONUT" ]			:= 0x1726;
	food [ "ITEM_RAW_CHICKEN_LEG" ]	:= 0x1607;
	food [ "ITEM_RAW_LEG_OF_LAMB" ]	:= 0x1609;
	food [ "ITEM_CORN_COB" ]			:= 0x0c82;
	food [ "ITEM_WHEAT_SHEAF" ]		:= 0x1ebd;
	food [ "ITEM_HOPS_CONES" ]		:= 0x1aa2;
	food [ "ITEM_POTATO" ]			:= 0xc806;

	//products that are used as ingredients in other recipes
	food [ "ITEM_BALL_OF_DOUGH" ]		:= 0xc805;
	food [ "ITEM_SWEET_DOUGH" ]		:= 0xc801;
	food [ "ITEM_CAKE_BATTER" ]		:= 0xc802;
	food [ "ITEM_PIZZA_CRUST" ]		:= 0xc803;
	food [ "ITEM_PIE_CRUST" ]		:= 0xc804;
	food [ "ITEM_BATTER" ]			:= 0xc806;

	return food;
endfunction




///////////////////
//  This function returns a dictionary of all foodstuffs used in cooking relating them to their names.  its used in
//  the cookbook script to translate names
///////////////////

function define_food_name_dictionary ()
	var food := dictionary;

	//some tools used in cooking
	food [ "ITEM_MIXING_BOWL" ]		:= "mixing bowl";
	food [ "ITEM_ROLLING_PIN" ]		:= "rolling pin";
	food [ "ITEM_FRYING_PAN" ]		:= "frying pan";
	food [ "ITEM_EMPTY_PITCHER" ]		:= "empty pitcher";
	food [ "ITEM_EMPTY_GLASS" ]		:= "empty glass";
	food [ "ITEM_EMPTY_BOTTLE" ]		:= "empty bottle";

	//bottles, pitchers and glasses of liquid
	food [ "ITEM_BOTTLE_OF_LIQUOR" ]		:= "bottle of liquor";
	food [ "ITEM_BOTTLE_OF_ALE" ]			:= "bottle of ale";
	food [ "ITEM_BOTTLE_OF_WINE" ]		:= "bottle of wine";
	food [ "ITEM_JUG_OF_CIDER" ]			:= "jug of cider";
	food [ "ITEM_BOTTLE_MELON_LIQUOR" ]	:= "bottle of melon liquor";
	food [ "ITEM_BOTTLE_PEACH_LIQUOR" ]	:= "bottle of peach liquor";
	food [ "ITEM_BOTTLE_OF_BRANDY" ]		:= "bottle of brandy";
	food [ "ITEM_BOTTLE_OF_EGGNOG" ]		:= "bottle of eggnog";

	food [ "ITEM_PITCHER_OF_LIQUOR" ]	:= "pitcher of liquor";
	food [ "ITEM_PITCHER_OF_ALE" ]	:= "pitcher of ale";
	food [ "ITEM_PITCHER_OF_WINE" ]	:= "pitcher of wine";
	food [ "ITEM_PITCHER_OF_CIDER" ]	:= "pitcher of cider";
	food [ "ITEM_PITCHER_OF_WATER" ]	:= "pitcher of water";
	food [ "ITEM_PITCHER_OF_MILK" ]	:= "pitcher of milk";

	food [ "ITEM_GLASS_OF_LIQUOR" ]	:= "glass of liquor";
	food [ "ITEM_GLASS_OF_ALE" ]		:= "glass of ale";
	food [ "ITEM_GLASS_OF_WINE" ]		:= "glass of wine";
	food [ "ITEM_GLASS_OF_CIDER" ]	:= "glass of cider";
	food [ "ITEM_GLASS_OF_WATER" ]	:= "glass of water";
	food [ "ITEM_GLASS_OF_MILK" ]		:= "glass of milk";

	//These items are special.  If they're called for in a recipe, it will call a special function that will
	//permit multiple forms of that item
	food [ "ITEM_GENERAL_WATER" ]		:= "cup of water";
	food [ "ITEM_GENERAL_MILK" ]		:= "cup of milk";
	food [ "ITEM_GENERAL_CHEESE" ]	:= "wedge of cheese";

	//primary food ingredients
	food [ "ITEM_FLOUR" ]			:= "cup of flour";
	food [ "ITEM_CORNMEAL" ]			:= "cup of cornmeal";
	food [ "ITEM_RAW_RIB" ]			:= "raw rib";
	food [ "ITEM_RAW_FISHSTEAK" ]		:= "raw fishsteak";
	food [ "ITEM_RAW_BIRD" ]			:= "raw bird";
	food [ "ITEM_RAW_HAM" ]			:= "raw ham";
	food [ "ITEM_BACON_SLAB" ]		:= "bacon slab";
	food [ "ITEM_RAW_SAUSAGE" ]		:= "raw sausage";
	food [ "ITEM_GREEN_FISH" ]		:= "green fish";
	food [ "ITEM_BROWN_FISH" ]		:= "brown fish";
	food [ "ITEM_BLUE_FISH" ]		:= "blue fish";
	food [ "ITEM_YELLOW_FISH" ]		:= "yellow fish";
	food [ "ITEM_LARD" ]			:= "glob of lard";
	food [ "ITEM_EGG" ]				:= "egg";
	food [ "ITEM_GARLIC" ]			:= "clove of garlic";
	food [ "ITEM_HONEY" ]			:= "honey";
	food [ "ITEM_CARROT" ]			:= "carrot";
	food [ "ITEM_APPLE" ]			:= "apple";
	food [ "ITEM_ORANGE" ]			:= "orange";
	food [ "ITEM_PEACH" ]			:= "peach";
	food [ "ITEM_LEMON" ]			:= "lemon";
	food [ "ITEM_LIME" ]			:= "lime";
	food [ "ITEM_BANANA" ]			:= "banana";
	food [ "ITEM_PEAR" ]			:= "pear";
	food [ "ITEM_GRAPES" ]			:= "bunch of grapes";
	food [ "ITEM_DATES" ]			:= "dates";
	food [ "ITEM_PUMPKIN" ]			:= "pumpkin";
	food [ "ITEM_YELLOW_GOURD" ]		:= "yellow gourd";
	food [ "ITEM_GREEN_GOURD" ]		:= "green gourd";
	food [ "ITEM_SQUASH" ]			:= "squash";
	food [ "ITEM_WATERMELON" ]		:= "watermelon";
	food [ "ITEM_HONEYDEW_MELON" ]	:= "honeydew melon";
	food [ "ITEM_CANTELOUPE" ]		:= "canteloupe";
	food [ "ITEM_LETTUCE" ]			:= "head of lettuce";
	food [ "ITEM_CABBAGE" ]			:= "head of cabbage";
	food [ "ITEM_ONION" ]			:= "onion";
	food [ "ITEM_TURNIP" ]			:= "turnip";
	food [ "ITEM_COCONUT" ]			:= "coconut";
	food [ "ITEM_RAW_CHICKEN_LEG" ]	:= "raw chicken leg";
	food [ "ITEM_RAW_LEG_OF_LAMB" ]	:= "raw leg of lamb";
	food [ "ITEM_CORN_COB" ]			:= "corn cob";
	food [ "ITEM_WHEAT_SHEAF" ]		:= "wheat sheaf";
	food [ "ITEM_HOPS_CONES" ]		:= "hops cone";
	food [ "ITEM_POTATO" ]			:= "potato";

	//products that are used as ingredients in other recipes
	food [ "ITEM_BALL_OF_DOUGH" ]		:= "ball of dough";
	food [ "ITEM_SWEET_DOUGH" ]		:= "sweet dough";
	food [ "ITEM_CAKE_BATTER" ]		:= "cake batter";
	food [ "ITEM_PIZZA_CRUST" ]		:= "pizza crust";
	food [ "ITEM_PIE_CRUST" ]		:= "pie crust";
	food [ "ITEM_BATTER" ]			:= "batter";

	return food;
endfunction




///////////////////
//  This function decreases the amount of liquid in a container, such as a bottle or pitcher.  If the container
//  is empty, it destroys the container.  If the item has the property return_type defined, one of that item is
//  then created
///////////////////

function reduce_container_fullness (character, container, reduce_amount := 1)
	//make sure we got a valid reduce_amount
	if (reduce_amount < 1)
		SendSysMessage (character, "Invalid reduce_amount!");
		return;
	endif

	//get how full the container is.  If its not set, the container is full, which is a 4
	var fullness := GetObjProperty (container, "fullness");
	if (!fullness)
		fullness := 4;
	endif
	
	//reduce the fullness
	fullness := fullness - reduce_amount;
	
	//now rename the item, based on how much is left
	if (fullness == 3)
		var tempstring := container.desc;
		tempstring := TruncateArticle (tempstring);
		container.name := "a mostly full " + tempstring;
		SetObjProperty (container, "fullness", 3);
	elseif (fullness == 2)
		var tempstring := container.desc;
		tempstring := TruncateArticle (tempstring);
		if (tempstring ["mostly full "])
			tempstring ["mostly full "] := "";
		endif
		container.name := "half a " + tempstring;
		SetObjProperty (container, "fullness", 2);
	elseif (fullness == 1)
		var tempstring := container.desc;
		tempstring := TruncateArticle (tempstring);
		if (tempstring ["mostly full "])
			tempstring ["mostly full "] := "";
		endif
		if (tempstring ["half a "])
			tempstring ["half a "] := "";
		endif
		container.name := "an almost empty " + tempstring;
		SetObjProperty (container, "fullness", 1);
	else	//if it has no more uses left, return something,  if applicable, and destroy the item
		//load some data about the container from itemdesc
		var cfgfile := ReadConfigFile( "itemdesc" );
		var elem := FindConfigElem( cfgfile, container.objtype );
		var return_type := GetConfigInt (elem, "return_type");

		if (return_type)
			var empty_container := CreateItemInBackpack (character, return_type );
			//if the full container has a saved name, rename the new container
			if (GetObjProperty (container, "EmptyName") )
				empty_container.name := GetObjProperty (container, "EmptyName");
			endif
		endif
		destroyitem (container);
	endif
endfunction
