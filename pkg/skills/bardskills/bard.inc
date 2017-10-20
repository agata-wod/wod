include "../pkg/character/skillcheck/skillcheck";

const SNDTYPE_PROVOCATION := 1;
const SNDTYPE_DISCORDANCE := 2;
const SNDTYPE_PEACEMAKING := 3;


function findinstrument (character)
	if (GetObjProperty (character, "instrument"))
		var instrument_serial := GetObjProperty (character, "instrument");
		foreach item in EnumerateItemsInContainer (character.backpack)
			if (item.serial == instrument_serial)
				if (ReserveItem (item))
					return item;
				endif
			endif
		endforeach
	endif

	foreach item in EnumerateItemsInContainer (character.backpack)
		case (item.objtype)
			0x17067:
			0x0e9d:
			0x0e9e:
			0x0eb3:
			0x0eb4:
			0x0e9c:
			0x0eb2:
			0x0eb1:
			0x2805:
			0x2807:
			0x403b:
			0x403c:
				if (ReserveItem (item))
					SetObjProperty (character, "instrument", item.serial);
					return item;
				endif
		endcase
	endforeach

	return 0;
endfunction

function can_sing (me, circle)
	var cfg := ReadConfigFile (":*:itemdesc");
	foreach item in ListEquippedItems (me)
		if (cfg[item.objtype].blockcircle && cfg[item.objtype].blockcircle <= circle) 
			SendSysmessage(me, "Your equipment prevents you from using this instrument!");
			return 0;
		endif
	endforeach

	return 1;
endfunction

function PlayNoise (me, instrument, difficulty, soundtype := 0)
	var cfg := ReadConfigFile (":musicianship:itemdesc");
	var elem := FindConfigElem (cfg, instrument.objtype);
	var sound := 0;
	case (soundtype)
		SNDTYPE_PROVOCATION: sound := elem.provocation;
		SNDTYPE_DISCORDANCE: sound := elem.discordance;
		SNDTYPE_PEACEMAKING: sound := elem.peacemaking;
	endcase
	if (!sound)
		sound := elem.succeed;
	endif

	if (!difficulty)
		MakeSound (me, sound);
		return 1;
	elseif (CheckSkillWithItem (me, instrument, SKILLID_MUSICIANSHIP, difficulty))
		MakeSound (me, sound);
		return 1;
	else
		MakeSound (me, elem.fail);
		return 0;
	endif
endfunction




///////////////////
//  does the sound effect for the musical instrument, dependant on if people want to hear it or not
///////////////////

function MakeSound (character, soundeffect)
	foreach mobile in ListMobilesNearLocationEx (character.x, character.y, character.z, 15, 
			LISTEX_FLAG_NORMAL+LISTEX_FLAG_HIDDEN+LISTEX_FLAG_GHOST, character.realm)
		if (mobile.acct)
			var sound_options := GetObjProperty (mobile, "sound_options");
			if (!sound_options or !sound_options[1])
				PlaySoundEffectPrivate (character, soundeffect, mobile);
			endif
		endif
	endforeach
endfunction

function GetDiscordanceMod (who)
	var mods := GetObjProperty (who, "#mods");
	foreach onemod in mods
		if (onemod[1] == "discordance")
		return onemod[2];
	endif
	endforeach
	return 0;
endfunction
