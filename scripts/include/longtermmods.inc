use uo;
use os;
use util;



///////////////////
//  For long-term mods that persist through server restarts, but values need to be saved...
///////////////////

function DoLongTermMod (byref who, stat, amt, duration, description)
	var allmods := GetObjProperty (who, "longtermmods");
	if (!allmods)
		allmods := array {};
	endif
	var mmod := array {};
	mmod[1] := stat;
	mmod[2] := amt;
	mmod[3] := ReadGameClock()+duration;
	mmod[4] := description;

	AddToLongTermMods (allmods, mmod);
	SetObjProperty (who, "longtermmods", allmods);

	case (stat)
		"BonusMaxHP":
		"cBonusMaxHP":
		"BonusMaxStam":
		"cBonusMaxStam":
		"BonusMaxMana":
		"cBonusMaxMana":
			RecalcVitals (who);
		default:
			Syslog ("Error:  Invalid Statmod");
	endcase

	set_critical(0);
endfunction




///////////////////
//  Remove mods that have expired

function CheckLongTermMods (byref who)
	set_critical (1);
	var now := ReadGameClock();

	var currentmods := GetObjProperty (who, "longtermmods");
	if (!currentmods)
		set_critical (0);
		return;
	endif

	var newmods := array {};
	foreach mmod in currentmods
		if (now < mmod[3])
			newmods.append(mmod);
		endif
	endforeach

	if (len(newmods))
		SetObjProperty (who, "longtermmods", newmods);
	else
		EraseObjProperty (who, "longtermmods");
	endif

	RecalcVitals (who);

	set_critical(0);
endfunction




function AddToLongTermMods (byref allmods, byref mmod)

//For Reference:
//	mmod[1] := stat;
//	mmod[2] := amt;
//	mmod[3] := ReadGameClock()+duration;

	foreach oldmod in allmods
		if (oldmod[1] == mmod[1])
			if ( (mmod[2] > 0 and oldmod[2] > 0) or (mmod[2] < 0 and oldmod[2] < 0) )
				mmod[2] := 0;
			else
				oldmod[2] := oldmod[2] + mmod[2];
			endif
			oldmod[3] := mmod[3];
			return 0;
		endif
	endforeach

	allmods.append (mmod);
	return 1;

endfunction
