use attributes;
use util;

function GetFocusManaReduction (byref who, mana)
	var focus := GetAttribute (who, "Focus");
	if (focus <= 60)
		return mana;
	elseif (focus >= 150)
		focus := 45;
	else
		focus := (focus - 60) / 2;
	endif
	return CINT ((mana * (100 - (focus + RandomInt (focus + 1))))/100);
endfunction

