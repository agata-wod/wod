include "include/utility";
include ":virtue:virtue";
var speech := 0;

///////////////////
//  needs to be called for NPCs that speak
///////////////////

function SetUpSpeech ()
	var MyTemplateElem := GetNpcTemplateElem (me.npctemplate);
	speech := MyTemplateElem.speech;
	if (!speech)
		speech := 0;
	endif
endfunction




///////////////////
//  The main function that processes speech events
///////////////////

function check_speech (text, speaker)
	//Lowercase the text and strip out punctuation that might get in the way
	text := Lower (text);
	while (text["."])
		text["."] := "";
	endwhile
	while (text["?"])
		text["?"] := "";
	endwhile
	while (text["!"])
		text["!"] := "";
	endwhile
	while (text[","])
		text[","] := "";
	endwhile
	
	//Check for seer-set keywords
	if (RespondToScriptedKeyword (speaker, text))
		return;
	endif
	
	//Check for NPC clone speech
	if (DefaultHello (text, speaker))
		return;
	endif

	//See if they have a speech group set.  If not, give up.
	//Also, ignore GMs
	if (!speech)
		return;
	endif
	
	//Quit at this point most of the time, to prevent spam
	var myname := Lower (me.name);
	if (!text[myname])
		if (GetObjProperty (me, "#nextgreet"))
			if (GetObjProperty (me, "#nextgreet") > ReadGameClock())
				return;
			endif
		endif
		SetObjProperty (me, "#nextgreet", ReadGameClock() + 30);
	endif

	//Check to see if they've mentioned any key topics
	if (CheckForKeyTopicWords (text, speaker))
		return;
	endif
	
	//Again, spam reduction
	if (RandomInt (6) != 1)
		return;
	endif

	//Otherwise, chat about friends, or meet a new one
	//Some NPCs aren't very friendly
	if (GetObjProperty (me, "nofriends"))
		return;
	endif

	var myfriends := array { };
	for i := 1 to 3
		var thefriend := GetObjProperty (me, "friend"+i);
		if (!thefriend)
			break;
		elseif (thefriend == speaker.name)
			TurnToward (speaker);
			if (i == 1 or RandomInt (2))
				GreetFriend (text, speaker.name, i);
				return;
			endif
			ChatAboutFriends (text, speaker, i-1);
			return;
		else
			myfriends[i] := thefriend;
		endif
	endfor

	//the speaker is not a friend yet, so talk about another player, if we have any friends
	if (!len (myfriends))
		SetObjProperty (me, "friend1", speaker.name);
		SetObjProperty (me, "loyalty1", 1);
		GreetFriend (text, speaker.name, 1);
		return;
	endif

	var friendnum := 1;
	if (len (myfriends) > 1)
		friendnum :=  RandomInt (len (myfriends)) + 1;
	endif	
	TurnToward (speaker);
	ChatAboutFriends (text, speaker, friendnum);

	var loyalty := CINT (GetObjProperty (me, "loyalty"+friendnum));
	//(Maybe) replace the old friend with the new one
	if (RandomInt(20) + 1 > (loyalty+10))
		SetObjProperty (me, "friend"+friendnum, speaker.name);
		SetObjProperty (me, "loyalty"+friendnum, 1);
	endif
	return;
endfunction




///////////////////
//  NPCs can respond to key words set up by seers for quests
///////////////////

function RespondToScriptedKeyword (speaker, text)
	var keyword_dict := GetObjProperty (me, "keywords");
	if (!keyword_dict)
		return 0;
	endif

	var dict_keys := keyword_dict.keys ();
	if (!dict_keys or len (dict_keys) <= 0)
		return 0;
	endif
	
	var thekeyword := "";
	foreach keyword in dict_keys
		if (text[keyword])
			thekeyword := keyword;
			break;
		endif
	endforeach

	if (!thekeyword)
		return 0;
	endif

	var speech_lines := keyword_dict[thekeyword];
	if (!speech_lines)
		return 0;
	endif

	for i := 1 to len (speech_lines)
		Say (speech_lines[i]);
		if (speech_lines[i+1])
			if (len (speech_lines[i]) > 50)
				sleep (2);
			elseif (len (speech_lines[i]) > 35)
				sleepms (1500);
			else
				sleep (2);
			endif
		endif
	endfor
	
	var gavething := 0;
	var givenitem_response;
	foreach item in ListRootItemsInContainer (me.backpack)
		var returnfor := GetObjProperty (item, "returnfor");
		if (returnfor and returnfor == thekeyword)
			if (item.isa (POLCLASS_CONTAINER))
				if (GetObjProperty (item, "givecontainer"))
					gavething := 1;
					givenitem_response := GetObjProperty (item, "finished_reply");
					EraseObjProperty (item, "returnfor");
					EraseObjProperty (item, "finished_reply");
					EraseObjProperty (item, "givecontainer");
					MoveItemToContainer (item, speaker.backpack);
				else
					var stuff := ListRootItemsInContainer (item);
					if (stuff.size() > 1)
						MoveItemToContainer (stuff[1], speaker.backpack);
						gavething := 0;
					elseif (stuff.size() == 1)
						MoveItemToContainer (stuff[1], speaker.backpack);
						gavething := 1;
						givenitem_response := GetObjProperty (item, "finished_reply");
						DestroyItem (item);
					else
						gavething := 1;
						DestroyItem (item);
					endif
				endif
			else
				gavething := 1;
				givenitem_response := GetObjProperty (item, "finished_reply");
				EraseObjProperty (item, "returnfor");
				EraseObjProperty (item, "finished_reply");
				MoveItemToContainer (item, speaker.backpack);
			endif
		endif
	endforeach 

	if (!gavething)
		return 1;
	endif
	
	if (givenitem_response)
		keyword_dict[thekeyword] := givenitem_response;
		SetObjProperty (me, "keywords", keyword_dict);
	endif
	
	return 1;
endfunction




///////////////////
//  Used for NPC clones of players
///////////////////

function DefaultHello (text, speaker)

	text := "not used";

	TurnToward (speaker);
	if (!GetObjProperty (me, "logoff_clone_options"))
		return 0;
	endif
	
	if (GetObjProperty (me, "#nextgreet"))
		if (GetObjProperty (me, "#nextgreet") > ReadGameClock())
			return 1;
		endif
	endif
	SetObjProperty (me, "#nextgreet", ReadGameClock() + 30);

	var logoffcloneoptions := GetObjProperty (me, "logoff_clone_options");
	var messages := array {};
	
	for i := 2 to 11
		if (logoffcloneoptions[i])
			messages.append (logoffcloneoptions[i]);
		endif
	endfor
	
	if (!len(messages))
		return 1;
	endif
	
	PrintTextAbove (me, messages[RandomInt (len(messages) )+ 1]);
	return 1;
endfunction




///////////////////
//  Key topic words are words that get special responses, and can be set
//  in /config/speechtopics.cfg
///////////////////

function CheckForKeyTopicWords (text, speaker)
	var speechcfg := ReadConfigFile ("::speechtopics");
	if (!speechcfg)
		Syslog ("Error opening speechtopics.cfg!");
		return 0;
	endif
	
	//Check each word in the text to see if it matches a set keyword
	var cfgentry;
	foreach word in SplitWords (text)
		cfgentry := FindConfigElem (speechcfg, word);
		if (cfgentry)
			var replies := GetConfigStringArray (cfgentry, "reply");
			if (len (replies))
				var thereply := replies [RandomInt (len (replies))+1];
				TurnToward (speaker);
				say (thereply);
			endif
			return 1;
		endif
	endforeach
	return 0;
endfunction




///////////////////
//  Talk to someone we already know
///////////////////

function GreetFriend (text, friendname, friendnum)

	text := "not used";

	var loyalty := GetObjProperty (me, "loyalty"+friendnum);
	if (!loyalty)
		loyalty := 0;
	elseif (loyalty >= 3)
		friendname := "My "+ GetFriendTitle(loyalty) + friendname;
	endif

	if (loyalty >= 3)
		friendname := "My "+ GetFriendTitle(loyalty) + friendname;
	endif

	var chatcfg := ReadConfigFile("::friendchat");
	var elem := FindConfigElem (chatcfg, "talktofriend");
	var spamlist := GetConfigStringArray (elem, "say");
	var spamline := spamlist [randomint(len(spamlist))+1 ];
	spamline["friendname"] := friendname;
	say (spamline);

	if ( RandomInt(8) == 1 )
		loyalty := loyalty + 1;
		if (loyalty > 9)
			loyalty := 9;
		endif
		SetObjProperty (me, "loyalty"+friendnum, loyalty);
	endif
endfunction




///////////////////
//  Talk about another player
///////////////////

function ChatAboutFriends (text, speaker, friendnum)
  if (text)
  endif
	var loyalty := GetObjProperty (me, "loyalty"+friendnum);
	if (!loyalty)
		loyalty := 0;
	endif
	var friend := GetObjProperty (me, "friend"+friendnum);
	if (friend or RandomInt (2))
		CheckForKeyTopicWords ("hello", speaker);
		return;
	endif

	if (loyalty >= 3)
		friend := "My "+ GetFriendTitle(loyalty) + friend;
	endif

	var chatcfg := ReadConfigFile("::friendchat");
	var elem := FindConfigElem (chatcfg, "talkaboutfriends");
	var spamlist := GetConfigStringArray( elem, "say" );
	var spamline := spamlist[ randomint(len(spamlist))+1 ];

	spamline["charname"] := speaker.name;
	spamline["friendname"] := friend;

	say (spamline);
endfunction




///////////////////
//  Nice titles for people the NPC likes
///////////////////

function GetFriendTitle(loyalty)
	case (loyalty)
	3:
	4:
		return "Friend, ";
	5: 
		return "Good Friend, ";
	6: 
		return "Great Friend, ";
	7: 
		return "Old Friend, ";
	8: 
		return "Dear Friend, ";
	9: 
		return "Dearest Friend, ";
	default: 
		return "";
	endcase
endfunction




///////////////////
//  Called when item is given to (some) NPCs
///////////////////

function TakeItem (you, item)
	TurnToward (you);
	if (!getobjproperty (me, "serial"))
		setobjproperty (me, "serial", me.serial);
	endif
	if (!me.backpack)
		var newbackpack := CreateItemAtLocation (me.x, me.y, me.z, UOBJ_BACKPACK, 1, me.realm);
		if (!EquipItem (me, newbackpack))
			DestroyItem (newbackpack);
			return;
		endif
	endif
	MoveItemToContainer (item, me.backpack);

	//launch an external script
	var itemparms := array {};
	itemparms[1] := you;
	itemparms[2] := me;
	itemparms[3] := item;
	TurnToward (you);
	start_script (":npcs:giveitem_npc", itemparms);
	return;
endfunction

