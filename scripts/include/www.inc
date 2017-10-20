///////////////////
//
//  Functions used in POL www pages
//
///////////////////
use datafile;

include "include/clock";
include "include/wodinc";

function WriteHTMLPageOpen (page_title)
	WriteHTML ("<html><head>");
	WriteHTML ("<title>" + page_title + "</title>");
	WriteHTML ("</head>");
	WriteHTML ("<body bgcolor=\"#dcdeed\">");
endfunction

function WriteHTMLPageClose ()
	WriteHTML ("</body></html>");
endfunction




///////////////////
//  Try to match a webpage hit with an online player, based on IP address
///////////////////

function FindOnlineAccountByIP (ipaddress)
	foreach character in EnumerateOnlineCharacters()
		if (character.ip == ipaddress)
			return character;
		endif
	endforeach
	return 0;
endfunction




///////////////////
//  Records when someone logs into the WWW page (for security reasons)
//  This records the IP address and the timestamp
///////////////////

function LogWWWPageAccess (ipaddress)
	//Try to find an online character that matches
	var character := 0;
	foreach player in EnumerateOnlineCharacters()
		if (player.ip == ipaddress)
			character := player;
			break;
		endif
	endforeach
	
	var this_access := Array {};
	this_access[1] := ipaddress;
	this_access[2] := Systime_GetReadableTimeString ();
	
	var thisscript := GetProcess(GetPID());
	this_access[3] := thisscript.name;
	
	if (character)
		this_access[4] := character.name;
	else
		this_access[4] := "";
	endif
	
	set_critical (1);
	var www_datafile := OpenDataFile ("www_logs");
	if (!www_datafile)
		www_datafile := CreateDataFile ("www_logs");
		if (!www_datafile)
			set_critical (0);
			return CreateError ("Unable to create www datafile!");
		endif
	endif

	var entry := www_datafile.FindElement ("access_attempts");
	if (!entry)
		www_datafile.CreateElement ("access_attempts");
		entry := www_datafile.FindElement ("access_attempts");
	endif
	
	entry.SetProp (CSTR (ReadGameClock()), this_access);
	UnloadDataFile ("www_logs");
	set_critical (0);
	return 1;
endfunction





