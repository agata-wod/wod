use os;
use uo;

include "include/skills";
include "include/attributes";

function DoSkillCap (who)
	for skillid := 0 to 57
		SetSkillToCap (who, skillid);
	endfor
endfunction

function SetSkillToCap (character, skillid)
	var attributeid := GetAttributeIDBySkillID (skillid);
	if (GetAttributeBaseValue (character, attributeid) > 600)
		var skillcap := GetBaseSkillCap (character, skillid);
		if (GetAttributeBaseValue (character, attributeid) > skillcap)
			SetAttributeBaseValue (character, attributeid, skillcap);
		endif
	endif
endfunction
