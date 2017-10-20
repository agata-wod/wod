use uo;
use os;
use util;

include "include/client";
include "include/objtype";
include "include/itemtypes";
include "include/utility";

Const MAX_AUCTIONS_PER_CATEGORY := 3;

Const BUTTON_LAST_ITEM		:= 1000;
Const BUTTON_NEXT_ITEM		:= 1001;
Const BUTTON_EVALUATE_ITEM	:= 1002;
Const BUTTON_MOVE_GUMP_UP	:= 1003;
Const BUTTON_MOVE_GUMP_DOWN	:= 1004;
Const BUTTON_MOVE_GUMP_LEFT	:= 1005;
Const BUTTON_MOVE_GUMP_RIGHT	:= 1006;
Const BUTTON_MOVE_GUMP_SIZE	:= 1007;
Const BUTTON_HELP			:= 1010;

var storage := find_or_create_storage ( "Auction Storage" );
if (!storage)
	exit;
endif



///////////////////
//  figures out how much gold the auctioneer is holding for the player
///////////////////

function DetermineGoldHeld (acctname)
	var gold_array := GetGlobalProperty ("auctioneer_gold");
	if (!gold_array)
		gold_array := dictionary;
	endif

	if (!gold_array.exists (acctname) )
		return 0;
	else
		return CINT (gold_array[acctname]);
	endif
endfunction




///////////////////
//  adds or subtracts gold from a player's account (pass a negative number to subtract).  This
//  function doesn't check the numbers, so its possible to go below 0.
///////////////////

function AddAmountToGoldHeld (acctname, new_amount)
	set_critical (1);
	new_amount := CINT (new_amount);
	var gold_array := GetGlobalProperty ("auctioneer_gold");
	if (!gold_array)
		gold_array := dictionary;
	endif

	var gold_amount;
	if (!gold_array.exists (acctname) )
		gold_amount := 0;
	else
		gold_amount := CINT (gold_array[acctname]);
	endif

	gold_array [acctname] := gold_amount + new_amount;
	SetGlobalProperty ("auctioneer_gold", gold_array);
	set_critical (0);
endfunction




///////////////////
//  determines the amount of gold the player has available to use (When a player bids on an item, that gold
//  is marked as unavailable)
///////////////////

function DetermineGoldAvailable (acctname)
	var gold_held := DetermineGoldHeld (acctname);
	var gold_reserved := 0;
	foreach category in ListCategoryNames ()
		var category_box := find_or_create_item (storage, category, UOBJ_BANKBOX);
		foreach item in ListRootItemsInContainer (category_box)
			var current_high_bidder := GetObjProperty (item, "current_high_bidder");
			if (current_high_bidder and current_high_bidder == acctname)
				gold_reserved := gold_reserved + CINT (GetObjProperty (item, "current_high_bid"));
			endif
		endforeach
	endforeach

	return (gold_held - gold_reserved);
endfunction




///////////////////
//  builds the gump for the category selection menu
///////////////////

function SelectCategory (player, message := "What kind of items do you want to look at?")
	var categories := CreateMenu (message);
	foreach category_description in ListCategoryDescriptions ()
		AddMenuItem (categories, 0, category_description);
	endforeach
	var cat_select := SelectMenuItem2 (player, categories);
	if (!cat_select)
		return 0;
	endif
	
	var category_names := ListCategoryNames ();
	
	return category_names[cat_select.index];
endfunction




///////////////////
//  returns a listing of the auction category names (behind the scenes names)
///////////////////

function ListCategoryNames ()
	return array {
		"auc_weapons_general",
		"auc_armor_general",
		"auc_clothing_general",
		"auc_decorative_general",
		"auc_other_general"
	};
endfunction




///////////////////
//  returns a listing of the auction category descriptions (names the player sees)
///////////////////

function ListCategoryDescriptions ()
	return array {
		"Weapons",
		"Armor",
		"Clothing",
		"Decorative",
		"Misc."
	};
endfunction




///////////////////
//  Checks all the auctions to see if which ones have been completed
///////////////////

function CheckCompletedAuctions ()
	start_script (":auctioneer:completeauctions", 0);
endfunction




///////////////////
//  Displays the pages for the auction items in the list provided
///////////////////

function DisplayAuctionItems (player, auctioneer, byref items, current_item_num := 1)
	if (!items.size())
		SendSysMessage (player, "Error:  no items to display");
		return;
	endif
	if (current_item_num > items.size())
		current_item_num := 1;
	endif

	repeat
		players_gold_held := DetermineGoldHeld (player.acctname);
		players_gold_available := DetermineGoldAvailable (player.acctname);
		current_item := items[current_item_num];
		BuildAuctionDisplayGump (current_item, player);

		var gump_return := SendDialogGump (player, gump_layout, gump_data);
		if (gump_return == error)
			if (auctioneer)
				PrintTextAbovePrivate (auctioneer, "Please close other gumps first.", player);
			else
				SendSysMessage (player, "Please close other gumps first");
			endif
			return;
		elseif (!gump_return or !gump_return[0])
			if (auctioneer)
				PrintTextAbovePrivate (auctioneer, "Maybe next time.", player);
			else
				SendSysMessage (player, "Maybe next time.");
			endif
			return;
		endif
		
		if (gump_return[0] == BUTTON_NEXT_ITEM)
			current_item_num := current_item_num + 1;
			if (current_item_num > len (items))
				current_item_num := 1;
			endif
			SetObjProperty (player, "#auction_item_num", current_item_num);
		elseif (gump_return[0] == BUTTON_LAST_ITEM)
			current_item_num := current_item_num - 1;
			if (current_item_num < 1)
				current_item_num := len (items);
			endif
			SetObjProperty (player, "#auction_item_num", current_item_num);
		elseif (gump_return[0] == BUTTON_EVALUATE_ITEM)
			start_script (":auctioneer:evaluate_item", array{player, current_item});
		elseif (gump_return[0] == BUTTON_MOVE_GUMP_UP)
			var auction_gump_location := GetObjProperty (player, "#auction_gump_location");
			if (!auction_gump_location)
				auction_gump_location := array{100, 100};
			endif
			
			auction_gump_location[2] := auction_gump_location[2] - 50;
			if (auction_gump_location[2] < 0)
				auction_gump_location[2] := 0;
			endif
			SetObjProperty (player, "#auction_gump_location", auction_gump_location);
		elseif (gump_return[0] == BUTTON_MOVE_GUMP_DOWN)
			var auction_gump_location := GetObjProperty (player, "#auction_gump_location");
			if (!auction_gump_location)
				auction_gump_location := array{100, 100};
			endif
			
			auction_gump_location[2] := auction_gump_location[2] + 50;
			SetObjProperty (player, "#auction_gump_location", auction_gump_location);
		elseif (gump_return[0] == BUTTON_MOVE_GUMP_LEFT)
			var auction_gump_location := GetObjProperty (player, "#auction_gump_location");
			if (!auction_gump_location)
				auction_gump_location := array{100, 100};
			endif
			
			auction_gump_location[1] := auction_gump_location[1] - 50;
			if (auction_gump_location[1] < 0)
				auction_gump_location[1] := 0;
			endif
			SetObjProperty (player, "#auction_gump_location", auction_gump_location);
		elseif (gump_return[0] == BUTTON_MOVE_GUMP_RIGHT)
			var auction_gump_location := GetObjProperty (player, "#auction_gump_location");
			if (!auction_gump_location)
				auction_gump_location := array{100, 100};
			endif
			
			auction_gump_location[1] := auction_gump_location[1] + 50;
			SetObjProperty (player, "#auction_gump_location", auction_gump_location);
		else
			var item := current_item;

			//check the timer to see if it expired when they were browsing
			if (!GetObjProperty (item, "endtime") or GetObjProperty (item, "endtime") < ReadGameClock())
				if (auctioneer)
					PrintTextAbovePrivate (auctioneer, "Too late!  Bidding is already closed on that item", player);
				else
					SendSysMessage (player, "Too late!  Bidding is already closed on that item");
				endif
				return;
			endif

			if (BidOnItem (player, auctioneer, item))
				return;
			endif
		endif
	until (!player);
endfunction




///////////////////
//  Handles bids on items
///////////////////

function BidOnItem (player, auctioneer, item)
	// set up a couple of variables
	var current_high_bid := GetObjProperty (item, "current_high_bid");
	var current_high_bidder := GetObjProperty (item, "current_high_bidder");
	var minbid := GetObjProperty (item, "minbid");
	players_gold_held := DetermineGoldHeld (player.acctname);
	players_gold_available := DetermineGoldAvailable (player.acctname);

	//If its the player's own item and theres no bidders, they can remove it from auction if there's no bidders
	if (GetObjProperty (item, "selleracctname") == player.acctname)
		if (!current_high_bid)
			if (auctioneer)
				PrintTextAbovePrivate (auctioneer, "Well, since no one has bid on that item yet, I guess we can do that.", player);
			else
				SendSysMessage (player, "Well, since no one has bid on that item yet, I guess we can do that.");
			endif
			MoveItemToContainer (item, player.backpack);
			EraseAllProperties (item);
			return 1;
		else
			if (auctioneer)
				PrintTextAbovePrivate (auctioneer, "You can't bid on your own items!", player);
			else
				SendSysMessage (player, "You can't bid on your own items!");
			endif
			return 0;
		endif
	endif

	if (current_high_bid)
		minbid := minbid + GetMinimumBidIncrement (minbid);
	endif

	//Let them know if they're the current high bidder
	if (current_high_bidder and current_high_bidder == player.acctname)
		if (auctioneer)
			PrintTextAbovePrivate (auctioneer, "You're already the highest bidder!", player);
			sleep (1);
			PrintTextAbovePrivate (auctioneer, "Your old bid was " + current_high_bid, player);
		else
			SendSysMessage (player, "You're already the highest bidder!");
			sleep (1);
			SendSysMessage (player, "Your old bid was " + current_high_bid);
		endif
	endif

	//Make sure they have enough gold available to pay for the minimum bid
	if (players_gold_available < minbid)
		if (auctioneer)
			PrintTextAbovePrivate (auctioneer, "The minimum bid is " + minbid + " gold.", player);
			sleep (1);
			PrintTextAbovePrivate (auctioneer, "You don't have that much available!", player);
		else
			SendSysMessage (player, "The minimum bid is " + minbid + " gold.");
			sleep (1);
			SendSysMessage (player, "You don't have that much available!");
		endif
		return;
	endif

	var players_bid := SendTextEntryGump (player, "Enter your bid:", TE_CANCEL_DISABLE, TE_STYLE_NORMAL, 10, "(Must be at least " + minbid + " gold)");
	players_bid := CINT (players_bid);
	if (!players_bid)
		if (auctioneer)
			PrintTextAbovePrivate (auctioneer, "Maybe next time, then.", player);
		else
			SendSysMessage (player, "Maybe next time, then.");
		endif
		return;
	endif

	//check the timer again to see if it expired when they were entering a price, or if it was
	//just about to expire, extend it a bit
	if (!GetObjProperty (item, "endtime") or GetObjProperty (item, "endtime") < ReadGameClock())
		if (auctioneer)
			PrintTextAbovePrivate (auctioneer, "Too late!  Bidding is already closed on that item", player);
		else
			SendSysMessage (player, "Too late!  Bidding is already closed on that item");
		endif
		return;
	elseif (GetObjProperty (item, "endtime") + 600 < ReadGameClock() )
		SetObjProperty (item, "endtime", ReadGameClock() + 600);
	endif

	//Reload some of the data in case someone else bid on the item while we
	// were entering our bid
	current_high_bid := GetObjProperty (item, "current_high_bid");
	current_high_bidder := GetObjProperty (item, "current_high_bidder");
	minbid := GetObjProperty (item, "minbid");
	players_gold_held := DetermineGoldHeld (player.acctname);
	players_gold_available := DetermineGoldAvailable (player.acctname);

	//add them to the list of bidders
	var bidder_list := GetObjProperty (item, "bidder_list");
	if (!bidder_list)
		bidder_list := Array {player.acctname};
		SetObjProperty (item, "bidder_list", bidder_list);
	elseif (!(player.acctname in bidder_list))
		bidder_list.append (player.acctname);
		SetObjProperty (item, "bidder_list", bidder_list);
	endif

	//Make sure they have enough to pay for their bid
	if (players_bid > players_gold_available)
		if (auctioneer)
			PrintTextAbovePrivate (auctioneer, "You don't have that much available!", player);
		else
			SendSysMessage (player, "You don't have that much available!");
		endif
		return;
	endif

	//if no one else bid, they get it for the minimum bid
	if (!current_high_bidder)
		if (players_bid < minbid)
			if (auctioneer)
				PrintTextAbovePrivate (auctioneer, "That's less than the current bid!", player);
			else
				SendSysMessage (player, "That's less than the current bid!");
			endif
			return;
		endif

		SetObjProperty (item, "current_high_bidder", player.acctname);
		SetObjProperty (item, "current_high_bid", players_bid);
		if (auctioneer)
			PrintTextAbovePrivate (auctioneer, "Congratulations!  You're the current high bidder at " + minbid + " gold!", player);
		else
			SendSysMessage (player, "Congratulations!  You're the current high bidder at " + minbid + " gold!");
		endif
		return;
	endif


	//if they're already the highest bidder, make sure they outbid themselves and
	// replace their old bid with the new one
	if (current_high_bidder == player.acctname)
		if (players_bid < GetObjProperty (item, "current_high_bid"))
			if (auctioneer)
				PrintTextAbovePrivate (auctioneer, "You must bid higher than your old bid of " + GetObjProperty (item, "current_high_bid"), player);
			else
				SendSysMessage (player, "You must bid higher than your old bid of " + GetObjProperty (item, "current_high_bid"));
			endif
		else
			SetObjProperty (item, "current_high_bid", players_bid);
			if (auctioneer)
				PrintTextAbovePrivate (auctioneer, "You replace your old bid with the new one of " + players_bid, player);
			else
				SendSysMessage (player, "You replace your old bid with the new one of " + players_bid);
			endif
		endif
		return;
	endif

	//otherwise we already have a bidder, so increment it by the min. bid inc.
	minbid := minbid + GetMinimumBidIncrement (minbid);
	if (players_bid < minbid)
		if (auctioneer)
			PrintTextAbovePrivate (auctioneer, "That's less than the current bid!", player);
		else
			SendSysMessage (player, "That's less than the current bid!");
		endif
		return;
	endif
	
	//if there's already a higher bid, outbid them
	if (current_high_bid >= players_bid)
		minbid := players_bid + GetMinimumBidIncrement (minbid);
		if (minbid > current_high_bid)
			minbid := current_high_bid;
		endif
		SetObjProperty (item, "minbid", minbid);
		if (auctioneer)
			PrintTextAbovePrivate (auctioneer, "I'm sorry, but there's an existing higher bid.", player);
		else
			SendSysMessage (player, "I'm sorry, but there's an existing higher bid.");
		endif
		return;

	//otherwise, increase the min bid to the preview bid
	else
		minbid := current_high_bid + GetMinimumBidIncrement (minbid);
		if (minbid > players_bid)
			minbid := players_bid;
		endif
		SetObjProperty (item, "current_high_bidder", player.acctname);
		SetObjProperty (item, "current_high_bid", players_bid);
		SetObjProperty (item, "minbid", minbid);
		if (auctioneer)
			PrintTextAbovePrivate (auctioneer, "Congratulations!  You're the current high bidder at " + minbid + " gold!", player);
		else
			SendSysMessage (player, "Congratulations!  You're the current high bidder at " + minbid + " gold!");
		endif
	endif
endfunction




///////////////////
//  does thw grunt work of building the gump
///////////////////

function BuildAuctionDisplayGump (byref item, byref player)
	var auction_gump_location := GetObjProperty (player, "#auction_gump_location");
	if (!auction_gump_location)
		auction_gump_location := array{100, 100};
	endif

	gump_layout := array {
		"nodispose",
		"nomove",
		"page 0",
		"resizepic " + auction_gump_location[1] + " " + auction_gump_location[2] + " 2600 500 380",
		"resizepic " + (auction_gump_location[1]+380) + " " + (auction_gump_location[2]+50) + " 5120 60 60",
		"tilepic " + (auction_gump_location[1]+380) + " " + (auction_gump_location[2]+55) + " " + item.graphic,

		"text " + (auction_gump_location[1]+60) + " " + (auction_gump_location[2]+30) + " 1327 0",			//"Item:"
		"text " + (auction_gump_location[1]+140) + " " + (auction_gump_location[2]+30) + " 0 1",				//First line of item name
		"text " + (auction_gump_location[1]+140) + " " + (auction_gump_location[2]+50) + " 0 2",				//Second line of item name
		
		"text " + (auction_gump_location[1]+60) + " " + (auction_gump_location[2]+75) + " 1327 3",			//"Seller:"
		"text " + (auction_gump_location[1]+140) + " " + (auction_gump_location[2]+75) + " 0 4",				//Seller name

		"text " + (auction_gump_location[1]+60) + " " + (auction_gump_location[2]+100) + " 1327 5",			//"Current Bid:"
		"text " + (auction_gump_location[1]+140) + " " + (auction_gump_location[2]+100) + " 0 6",				//the current bid

		"text " + (auction_gump_location[1]+60) + " " + (auction_gump_location[2]+125) + " 1327 7",			//"Ends in:"
		"text " + (auction_gump_location[1]+140) + " " + (auction_gump_location[2]+125) + " 0 8",				//Time left

		"croppedtext " + (auction_gump_location[1]+90) + " " + (auction_gump_location[2]+170) + " 350 20 0 9",	//First line of description
		"croppedtext " + (auction_gump_location[1]+90) + " " + (auction_gump_location[2]+190) + " 350 20 0 10",	//Second line of description
		"croppedtext " + (auction_gump_location[1]+90) + " " + (auction_gump_location[2]+210) + " 350 20 0 11",	//Third line of description
		"croppedtext " + (auction_gump_location[1]+90) + " " + (auction_gump_location[2]+230) + " 350 20 0 12",	//Fourth line of description

		"text " + (auction_gump_location[1]+70) + " " + (auction_gump_location[2]+310) + " 1327 13",			//Gold held string
		"text " + (auction_gump_location[1]+180) + " " + (auction_gump_location[2]+310) + " 0 14",			//Gold held amount
		"text " + (auction_gump_location[1]+70) + " " + (auction_gump_location[2]+335) + " 1327 15",			//Gold available string
		"text " + (auction_gump_location[1]+180) + " " + (auction_gump_location[2]+335) + " 0 16",			//Gold available amount

		"text " + (auction_gump_location[1]+100) + " " + (auction_gump_location[2]+260) + " 0 17",			//"Bid on item"
		"text " + (auction_gump_location[1]+300) + " " + (auction_gump_location[2]+260) + " 0 18",			//"Evalute item"

		"button " + (auction_gump_location[1]+400) + " " + (auction_gump_location[2]+302) + " 5600 5604 1 0 " + BUTTON_MOVE_GUMP_UP,
		"button " + (auction_gump_location[1]+400) + " " + (auction_gump_location[2]+340) + " 5602 5606 1 0 " + BUTTON_MOVE_GUMP_DOWN,
		"button " + (auction_gump_location[1]+420) + " " + (auction_gump_location[2]+320) + " 5601 5605 1 0 " + BUTTON_MOVE_GUMP_RIGHT,
		"button " + (auction_gump_location[1]+380) + " " + (auction_gump_location[2]+320) + " 5603 5607 1 0 " + BUTTON_MOVE_GUMP_LEFT,

		"button " + (auction_gump_location[1]+20) + " " + (auction_gump_location[2]+180) + " 5603 5607 1 0 " + BUTTON_LAST_ITEM,
		"button " + (auction_gump_location[1]+460) + " " + (auction_gump_location[2]+180) + " 5601 5605 1 0 " + BUTTON_NEXT_ITEM,
		"button " + (auction_gump_location[1]+280) + " " + (auction_gump_location[2]+265) + " 2104 2103 1 0 " + BUTTON_EVALUATE_ITEM
	};

	gump_data := array {
		"Item:",						//0
		item.desc,
		"",
		"Seller:",
		GetObjProperty (item, "sellername"),

		"Current bid:",				//5
		GetObjProperty (item, "minbid"),
		"Ends in:",
		"",
		"",
		"",							//10
		"",
		"",
		
		"Gold Held:",
		players_gold_held,
		"Gold Available:",				//15
		players_gold_available,
		"Bid on Item",
		"Evaluate Item"
	};
	
	//Split the name into 2 parts if its too long for one line
	var itemname := item.desc;
	if (len (itemname) > 40)
		var splitted := SplitWords (itemname);
		var itemname1 := "";
		var itemname2 := "";
		var done := 0;
		foreach word in splitted
			if (!done)
				if (len (itemname1) + len (word) < 40)
					if (len (itemname1))
						itemname1 := itemname1 + " " + word;
					else
						itemname1 := word;
					endif
				else
					itemname2 := word;
					done := 1;
				endif
			else
				itemname2 := itemname2 + " " + word;
			endif
		endforeach
		gump_data[2] := itemname1;
		gump_data[3] := itemname2;
	endif

	//Calculate the time left
	var time := CINT (GetObjProperty (item, "endtime")) - ReadGameClock();
	var days := CINT (time/86400);
	var hours := CINT ((time - days*86400)/3600);
	var minutes := CINT ( (time - days*86400 - hours*3600)/60);

	gump_data[9] := (days + " days, " + hours + " hours, " + minutes + " minutes");
	
	//Put in the item's description, if it has one
	var description := GetObjProperty (item, "auctiondescription");
	if (!description)
		description := array{"", "", "", ""};
	endif
	for i := 1 to 4
		gump_data[9+i] := description[i];	
	endfor
	
	//Add the buy button
	if (player.acctname != GetObjProperty (item, "selleracctname"))
		gump_layout.append ("button " + (auction_gump_location[1]+80) + " " + (auction_gump_location[2]+265) + " 2104 2103 1 0 " + (current_item_num));
	elseif (!GetObjProperty (item, "current_high_bidder"))
		gump_layout.append ("button " + (auction_gump_location[1]+80) + " " + (auction_gump_location[2]+265) + " 2104 2103 1 0 " + (current_item_num));
		gump_data[18] := "Remove item from auction";
	else
		gump_data[18] := "This item has already been bid on.";
	endif
endfunction




///////////////////
//  Erases all the properties set on the item when its put up for auction
///////////////////

function EraseAllProperties (item)
	EraseObjProperty (item, "endtime");
	EraseObjProperty (item, "minbid");
	EraseObjProperty (item, "sellerserial");
	EraseObjProperty (item, "selleracctname");
	EraseObjProperty (item, "sellername");
	EraseObjProperty (item, "current_high_bidder");
	EraseObjProperty (item, "current_high_bid");
	EraseObjProperty (item, "auctiondescription");
	EraseObjProperty (item, "bidder_list");
endfunction




///////////////////
//  Maintains a global list of transaction messages for each account
///////////////////

function AddTransactionMessage (acctname, message)
	set_critical (1);
	var allmessages := GetGlobalProperty ("auctioneer_transaction_messages");
	if (!allmessages)
		allmessages := dictionary;
	endif
	
	var acctmessages := array{};
	if (allmessages.exists (acctname))
		acctmessages := allmessages[acctname];
	endif
	
	acctmessages.append (message);
	allmessages [acctname] := acctmessages;
	SetGlobalProperty ("auctioneer_transaction_messages", allmessages);
	set_critical (0);
endfunction




///////////////////
//  Erase the message log for an account
///////////////////

function EraseTransactionMessages (acctname)
	set_critical (1);
	var allmessages := GetGlobalProperty ("auctioneer_transaction_messages");
	if (!allmessages)
		set_critical (0);
		return;
	endif

	if (!allmessages.exists (acctname))
		set_critical (0);
		return;
	endif

	allmessages.erase (acctname);
	SetGlobalProperty ("auctioneer_transaction_messages", allmessages);
	set_critical (0);
endfunction




///////////////////
//  Retrieves the messages for an account
///////////////////

function GetTransactionMessages (acctname)
	var allmessages := GetGlobalProperty ("auctioneer_transaction_messages");
	if (!allmessages)
		return array{};
	endif

	if (allmessages.exists (acctname))
		return allmessages [acctname];
	endif

	return array{};
endfunction




///////////////////
//  Figures out the proper commision on a sliding scale
///////////////////

function CalculateCommision (price)
	//10% commission on the first 1000 gold
	if (price <= 1000)
		return CINT (price * 0.1);
	endif
	var commission := 100;
	price := price - 1000;

	//5% commission up to 10,000 gold (next 9,000)
	if (price <= 9000)
		commission := commission + CINT (price * 0.05);
		return commission;
	endif
	commission := commission + 450;
	price := price - 9000;

	//2.5% commission up to 100,000 gold (next 90,000)
	if (price <= 90000)
		commission := commission + CINT (price * 0.025);
		return commission;
	endif
	commission := commission + 2250;
	price := price - 90000;
	
	//after that, 1% commission
	commission := commission + CINT (price * 0.01);
	return commission;
endfunction




///////////////////
//  The minimum bid increment depends on the going value
///////////////////

function GetMinimumBidIncrement (amount)
	if (amount < 100)
		return 1;
	elseif (amount < 1000)
		return 10;
	elseif (amount < 10000)
		return 100;
	else
		return 1000;
	endif
endfunction







