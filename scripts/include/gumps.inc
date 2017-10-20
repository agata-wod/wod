use os;
use uo;




///////////////////
//  Sends the given string to the character as an HTML gump.  The gump is scrollable
// and supports some basic HTML commands such as <b>...</b> and <i>...</i>
///////////////////

function SendStringAsHtmlWindow (who, text, title := "")
	var data := array {
		"page 0",    
		"nodispose", 
		"resizepic 40 40 5054 500 400",
		"tilepic 56 52 5357",
		"tilepic 65 49 4032",
		"text 109 50 62 0",
		"htmlgump 52 79 476 348 1 1 1"
	};
	var lines := array {
		title,
		text
	};
  
	return SendDialogGump (who, data, lines);
endfunction




///////////////////
//  A very simple, small gump that simply gives the player the choice between yes and no
///////////////////

function YesNo(player, Prompt)
	var ynlayout := array {
		"page 0",
		"gumppic 0 0 2070",
		"button 30 75 2130 2129 1 0 1",
		"button 90 75 2119 2120 1 0 0",
		"text 40 30 1355 0"
	};

	var yndata := array {
		Prompt
	};
           
	var res := SendDialogGump (player, ynlayout, yndata);

	if (res)
		return 1;
	else
		return 0;
	endif
endfunction




///////////////////
//  Currently an outdated function
///////////////////

function YesNoRes (me, Prompt)
	var layout := array {
		"page 0",
		"gumppic 0 0 2070",
		"button 30 75 2130 2129 1 0 1",
		"button 90 75 2119 2120 1 0 0",
		"text 40 30 1355 0"
	};

	var data := array {
		Prompt
	};

	me.frozen := 1;
	var res := SendDialogGump (me, layout, data);
	me.frozen := 0;

	if (res)
		return 1;
	else
		return 0;
	endif
endfunction




///////////////////
//  similar to above, but the gump closes after a set time (in seconds)
///////////////////

function TimedYesNo (player, prompt, time)
	var layout := array {
		"page 0",
		"gumppic 0 0 2070",
		"button 30 75 2130 2129 1 0 1",
		"button 90 75 2119 2120 1 0 0",
		"text 40 30 1355 0"
	};

	var data := array {
		prompt
	};
	
	var parms := Array {
		player,
		GetPid(),
		ReadGameClock() + time
	};
	start_script (":modifiers:popup_gump_timer", parms);

	var res := SendDialogGump (player, layout, data);

	if (res)
		return 1;
	else
		return 0;
	endif
endfunction




///////////////////
//  This function displays possible options with a button beside each one.
//  returns the index of the button pressed
///////////////////

function DisplayMenuSelectionGump (character, title, selections)
	var selections_count := selections.size();
	if (!selections_count)
		return 0;
	elseif (selections_count == 1)
		return 1;
	endif
	
	var selection_columns := 1;
	if (selections_count > 45)
		SendSysMessage (character, "Error:  Too many options to display.");
		return 0;
	elseif (selections_count > 20)
		selection_columns := 3;
	elseif (selections_count > 10)
		selection_columns := 2;
	endif

	var selection_rows := CINT (selections_count/selection_columns) + 1;
	var menu_height := (selection_rows * 25) + 120;
	var menu_width := ((selection_columns-1) * 200) + 300;

	var MenuSelection_gump_layout := array {
		"page 0",
		"nodispose",
		"resizepic 50 50 2620 " + menu_width + " " + menu_height,
		"text 70 59 40 0",
		"page 1"
	};
	
	var MenuSelection_gump_data := array {
		title
	};

	var current_y := 100;
	var current_x := 80;
	for i := 1 to selections_count
		MenuSelection_gump_layout.append ("button " + current_x + " " + (current_y+5) + " 2118 2117 1 1 " + i);
		MenuSelection_gump_layout.append ("text " + (current_x+20) + " " + current_y + " 40 " + i);
		MenuSelection_gump_data.append (selections[i]);
		current_y := current_y + 25;
		if (i == selection_rows)
			current_y := 100;
			current_x := 280;
		elseif (i == (selection_rows*2))
			current_y := 100;
			current_x := 480;
		endif
	endfor

	var gump_return := SendDialogGump (character, MenuSelection_gump_layout, MenuSelection_gump_data);
	if (!gump_return or !gump_return[0])
		return 0;
	endif
	return gump_return[0];
endfunction




///////////////////
//  This function (tries to) return the length of the the string in pixels (or whatever unit of measurement UO gumps use...)
//  Probably not very accurate
///////////////////

function GetStringGumpPixelLength (thestring)
	var string_len := 0;
	foreach letter in thestring
		var charlen := 0;
		case (letter)
			"|":
				charlen := 2;
			"`":
			"'":
			",":
			".":
			";":
			":":
				charlen := 3;
			"!":
				charlen := 4;
			"i":
			"l":
			"1":
			"[":
			"]":
				charlen := 3;
	
	
			"b":
			"c":
			"e":
			"j":
			"o":
			"t":
			"î":
			"ˆ":
			"(":
			")":
			"{":
			"}":
			"=":
				charlen := 6;
			"a":
			"d":
			"f":
			"p":
			"q":
			"r":
			"s":
			"z":
			"I":
			"Ñ":
			"‰":
			"~":
				charlen := 7;
			"g":
			"h":
			"n":
			"u":
			"v":
			"y":
			"Å":
			"¸":
			" ":
			"<":
			">":
			"-":
				charlen := 8;
			"x":
			"J":
			"5":
			"7":
			"8":
			"$":
			"?":
			"ﬂ":
			"+":
				charlen := 9;
			"k":
			"C":
			"E":
			"2":
			"3":
			"6":
			"9":
			"^":
			"/":
			"*":
				charlen := 10;
			"m":
			"w":
			"A":
			"B":
			"D":
			"F":
			"O":
			"P":
			"é":
			"ƒ":
			"ô":
			"÷":
			"4":
			"0":
			"%":
				charlen := 11;
			"G":
			"L":
			"N":
			"Q":
			"S":
			"Z":
			"#":
				charlen := 12;
			"H":
			"R":
			"T":
			"&":
			"_":
			"":
			"":
			"":
			"":
				charlen := 13;
			"K":
			"ö":
			"‹":
			"U":
			"Y":
				charlen := 14;
			"X":
				charlen := 15;
			"V":
				charlen := 16;
			"M":
				charlen := 18;
			"W":
				charlen := 21;
			default:
				SysLog ("Unable to find character " + letter);
				charlen := 12;
		endcase
		string_len := string_len + charlen;
	endforeach
	return string_len;
endfunction




