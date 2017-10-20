use os;
use uo;
use cfgfile;
use polsys;

include "include/client";
include "include/string";
include "include/attributes";

const LASTPAGE := 4;
const skillvalcolor := " 2400 ";
const skillnamecolor := " 0 ";

var selectskill_gump_layout := array {
	"nodispose",
	"page 0",
	"gumppic 0 50 500"
};
var selectskill_gump_data := array {
	""
};

var skillgroups := array {
	array { //Mage
		SKILLID_ALCHEMY,
		SKILLID_ANIMALLORE,
		SKILLID_INVOCATION,
		SKILLID_INSCRIPTION,
		SKILLID_MAGERY,
		SKILLID_MAGICRESISTANCE,
		SKILLID_NECROMANCY,
		SKILLID_MACEFIGHTING,
		SKILLID_WRESTLING,
		SKILLID_MEDITATION }, 
		
	array { //Bard
		SKILLID_PEACEMAKING,
		SKILLID_DISCORDANCE,
		SKILLID_PROVOCATION,
		SKILLID_MUSICIANSHIP,
		SKILLID_WRESTLING,
		SKILLID_MEDITATION }, 
		
	array { //Warrior
		SKILLID_ANATOMY,
		SKILLID_PARRY,
		SKILLID_LEADERSHIP,
		SKILLID_HEALING,
		SKILLID_TACTICS ,
		SKILLID_ARCHERY,
		SKILLID_SWORDSMANSHIP,
		SKILLID_MACEFIGHTING,
		SKILLID_FENCING,
		SKILLID_WRESTLING,
		SKILLID_FOCUS, 
		SKILLID_THROWING }, 
		
	array { //Ranger
		SKILLID_ANIMALLORE,
		SKILLID_CAMPING,
		SKILLID_CARTOGRAPHY,
		SKILLID_DETECTHIDDEN,
		SKILLID_HEALING,
		SKILLID_HIDING,
		SKILLID_TAMING,
		SKILLID_TRACKING,
		SKILLID_STEALTH }, 
		
	array { //Rogue
		SKILLID_DETECTHIDDEN,
		SKILLID_HIDING,
		SKILLID_LOCKPICKING,
		SKILLID_POISONING,
		SKILLID_STEALING, 
		SKILLID_STEALTH },
		
	array { //Tradesman
		SKILLID_ALCHEMY,
		SKILLID_BLACKSMITHY, 
		SKILLID_CARPENTRY,
		SKILLID_CARTOGRAPHY,
		SKILLID_COOKING,
		SKILLID_FISHING,
		SKILLID_INSCRIPTION,
		SKILLID_TAILORING,
		SKILLID_TINKERING,
		SKILLID_LUMBERJACKING,
		SKILLID_MINING }
};


///////////////////
//  Allows the character to select a new skill
///////////////////

function PickASkill (me)
	selectskill_gump_layout := array {
		"nodispose",
		"page 0",
		"gumppic 0 50 500"
	};
	selectskill_gump_data := array {
		""
	};

	var skills := array {};
	var skill := struct {};
	skill.+ value;
	skill.+ index;

	//organize them by skill value:
	for i := 0 to 57
		skill.value := SortValue (me, i);
		skill.index := i;
		skills[i+1] := skill;
	endfor

	//sort from high to low (thanks Myrathi!  nice function!)
	skills := SortMultiArrayByIndex (skills, "value");
	skills.reverse();

	foreach skill in skills
		selectskill_gump_data.append (GetSkillValDisplay (me, skill.index));
		selectskill_gump_data.append (GetSkillName (skill.index));
	endforeach

	//finish loading skillwinlayout
	var button;
	for pagenum := 1 to LASTPAGE
		LoadPageArray (pagenum);
		for i := 1 to 6
			button := ( (pagenum - 1) * 12 ) + i;
			LoadLeftPageArray (button, i, skills[button].index);
		endfor
		for i := 7 to 12
			button := ( (pagenum - 1) * 12 ) + i;
			LoadRightPageArray (button, i-6, skills[button].index);
		endfor
	endfor

	var sreturn := SendDialogGump( me, selectskill_gump_layout, selectskill_gump_data );
	var snum := sreturn[0];
	if ( (snum) or (snum == 0) )
		return skills[snum].index;
	else
		return;
	endif
endfunction



function LoadPageArray (pagenum)
	if ( pagenum == 1 )
		selectskill_gump_layout.append ("page 1");
		selectskill_gump_layout.append ("button 356 50 502 502 0 2");//next page
	elseif ( pagenum == LASTPAGE )
		selectskill_gump_layout.append ("page " + cstr(LASTPAGE));
		selectskill_gump_layout.append ("button 0 50 501 501 0 "+ cstr(LASTPAGE-1));//previous page
	else
		selectskill_gump_layout.append ("page " + cstr(pagenum));
		selectskill_gump_layout.append ("button 0 50 501 501 0 " + cstr(pagenum - 1));//next
		selectskill_gump_layout.append ("button 356 50 502 502 0 "+ cstr(pagenum + 1));//previous
	endif
endfunction

function LoadCustomPageArray (pagenum, numpages)
	if ( pagenum == 1 )
		selectskill_gump_layout.append ("page 1");
		if (numpages > 1)
			selectskill_gump_layout.append ("button 356 50 502 502 0 2");//next page
		endif
	elseif ( pagenum == numpages )
		selectskill_gump_layout.append ("page " + cstr(numpages));
		selectskill_gump_layout.append ("button 0 50 501 501 0 "+ cstr(numpages-1));//previous page
	else
		selectskill_gump_layout.append ("page " + cstr(pagenum));
		selectskill_gump_layout.append ("button 0 50 501 501 0 " + cstr(pagenum - 1));//next
		selectskill_gump_layout.append ("button 356 50 502 502 0 "+ cstr(pagenum + 1));//previous
	endif
endfunction

function LoadLeftPageArray (button, i, skillid)
	var skillvaltext := cstr((button*2)-1);
	var skillnametext := cstr(button*2);
	var skillname := GetSkillName(skillid);
	if ( skillname == "None" )
		skillvaltext := " 0";
		skillnametext := " 0";
	endif

	selectskill_gump_layout.append ("text 50 " + cstr(80 + (20*i)) + skillvalcolor + skillvaltext);
	selectskill_gump_layout.append ("text 100 " + cstr(80 + (20*i)) + skillnamecolor + skillnametext);

	if ( skillname == "None" )
		return;
	endif

	var buttoncolor := " 2104 2103 ";
	if (IsTradeSkill(skillid))
		buttoncolor := " " + cstr(cint(0x939)) + " 2103 ";
	endif
	
	selectskill_gump_layout.append ("button 35 " + cstr(85 + (20*i)) + buttoncolor + "1 0 " + cstr( button ));

endfunction

function LoadRightPageArray (button, i, skillid)

	var skillvaltext := cstr((button*2)-1);
	var skillnametext := cstr(button*2);
	var skillname := GetSkillName(skillid);
	if ( skillname == "None" )
		skillvaltext := " 0";
		skillnametext := " 0";
	endif

	selectskill_gump_layout.append ("text 240 " + cstr(80 + (20*i)) + skillvalcolor + skillvaltext);
	selectskill_gump_layout.append ("text 290 " + cstr(80 + (20*i)) + skillnamecolor + skillnametext);

	if ( skillname == "None" )
		return;
	endif

	var buttoncolor := " 2104 2103 ";
	if (IsTradeSkill(skillid))
		buttoncolor := " " + cstr(cint(0x939)) + " 2103 ";
	endif

	selectskill_gump_layout.append ("button 225 " + cstr(85 + (20*i)) + buttoncolor + "1 0 " + cstr( button ));

endfunction




///////////////////
//  Makes sure that primary and secondary skills are listed first
///////////////////

function SortValue (character, skillid)
	if (GetSkillName (skillid) == "None")
		return 0;		//sort disabled skills to the end
	endif

	//The +1 makes sure enabled skills go in front of disabled ones
	return GetAttribute (character, GetAttributeIDBySkillID (skillid)) * 10 + 1;
endfunction

function PickATradeSkill (me)
	selectskill_gump_layout := array {
		"nodispose",
		"page 0",
		"gumppic 0 50 500"
	};
	selectskill_gump_data := array {
		""
	};

	var skills := array {};
	var skill := struct {};
	skill.+ value;
	skill.+ index;

	//organize them by skill value:
	var j := 1;
	for i := 0 to 57
		if (IsTradeSkill (i))
			skill.value := SortValue (me, i);
			skill.index := i;
			skills[j] := skill;
			j := j + 1;
		endif
	endfor

	//sort from high to low (thanks Myrathi!  nice function!)
	skills := SortMultiArrayByIndex (skills, "value");
	skills.reverse();

	foreach skill in skills
		selectskill_gump_data.append (GetSkillValDisplay (me, skill.index));
		selectskill_gump_data.append (GetSkillName (skill.index));
	endforeach

	//finish loading skillwinlayout
	var button;
	var numpages := Ceil ((skills.size())/12.0);
	for pagenum := 1 to numpages
		LoadCustomPageArray (pagenum, numpages);
		for i := 1 to 6
			button := ( (pagenum - 1) * 12 ) + i;
			LoadLeftPageArray (button, i, skills[button].index);
		endfor
		for i := 7 to 12
			button := ( (pagenum - 1) * 12 ) + i;
			LoadRightPageArray (button, i-6, skills[button].index);
		endfor
	endfor

	var sreturn := SendDialogGump( me, selectskill_gump_layout, selectskill_gump_data );
	var snum := sreturn[0];
	if ( (snum) or (snum == 0) )
		return skills[snum].index;
	else
		return;
	endif
endfunction

function PickACompSkill (me, thespecs)
	selectskill_gump_layout := array {
		"nodispose",
		"page 0",
		"gumppic 0 50 500"
	};
	selectskill_gump_data := array {
		""
	};
	

	var skills := array {};
	var skill := struct {};
	skill.+ value;
	skill.+ index;

	//organize them by skill value:
	var compskills := GetCompSkillList(thespecs);
	foreach skillid in compskills
		skill.value := SortValue (me, skillid);
		skill.index := skillid;
		skills[_skillid_iter] := skill;
	endforeach

	//sort from high to low (thanks Myrathi!  nice function!)
	skills := SortMultiArrayByIndex (skills, "value");
	skills.reverse();

	foreach skill in skills
		selectskill_gump_data.append (GetSkillValDisplay (me, skill.index));
		selectskill_gump_data.append (GetSkillName (skill.index));
	endforeach

	//finish loading skillwinlayout
	var button;
	var numpages := Ceil ((skills.size())/12.0);
	for pagenum := 1 to numpages
		LoadCustomPageArray (pagenum, numpages);
		for i := 1 to 6
			button := ( (pagenum - 1) * 12 ) + i;
			LoadLeftPageArray (button, i, skills[button].index);
		endfor
		for i := 7 to 12
			button := ( (pagenum - 1) * 12 ) + i;
			LoadRightPageArray (button, i-6, skills[button].index);
		endfor
	endfor

	var sreturn := SendDialogGump( me, selectskill_gump_layout, selectskill_gump_data );
	var snum := sreturn[0];
	if ( (snum) or (snum == 0) )
		return skills[snum].index;
	else
		return;
	endif
endfunction

function GetCompSkillList (thespecs)
	var skilllist := array {};
	for i := 1 to 6
		var groupmatches := 0;
		if (IsInSkillGroup (thespecs[1], i))
			groupmatches := groupmatches + 1;
		endif
		if (IsInSkillGroup (thespecs[2], i))
			groupmatches := groupmatches + 1;
		endif
		if (IsInSkillGroup (thespecs[3], i))
			groupmatches := groupmatches + 1;
		endif
		if (groupmatches > 1)
			skilllist := skilllist + skillgroups[i];
			if (i == 2)
				skilllist := skilllist + skillgroups[1];
			endif
		endif
	endfor
	if (skilllist.size() == 0)
		skilllist := skillgroups[3] + skillgroups[6];
	else
		var tempskilllist := skilllist;
		tempskilllist.sort();
		Syslog ("Tempskilllist: " + tempskilllist);
		skilllist := array {};
		var lastskill := -1;
		foreach skill in tempskilllist
			if (lastskill != skill)
				skilllist.append(skill);
				lastskill := skill;
			endif
		endforeach
	endif
	return skilllist;
endfunction

function IsInSkillGroup (skill, group)
	foreach compskill in (skillgroups[group])
		if (skill == compskill)
			return 1;
		endif
	endforeach
	return 0;
endfunction

