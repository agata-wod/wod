use vitals;

///////////////////
//  Random functions and constants dealing with the new attributes system
///////////////

include "include/client";
include "include/math";
include "include/skills";

Const ATTRIBUTEID_ALCHEMY		:= "Alchemy";
Const ATTRIBUTEID_ANATOMY		:= "Anatomy";
Const ATTRIBUTEID_ANIMALLORE		:= "Animallore";
Const ATTRIBUTEID_ARCHERY		:= "Archery";
Const ATTRIBUTEID_BLACKSMITHY		:= "Blacksmithy";
Const ATTRIBUTEID_CAMPING		:= "Camping";
Const ATTRIBUTEID_CARPENTRY		:= "Carpentry";
Const ATTRIBUTEID_STONECRAFTING		:= "Stonecrafting";
Const ATTRIBUTEID_CARTOGRAPHY		:= "Cartography";
Const ATTRIBUTEID_COOKING		:= "Cooking";
Const ATTRIBUTEID_DETECTHIDDEN	:= "DetectingHidden";
Const ATTRIBUTEID_LEADERSHIP		:= "Leadership";
Const ATTRIBUTEID_INVOCATION		:= "Invocation";
Const ATTRIBUTEID_FENCING		:= "Fencing";
Const ATTRIBUTEID_FISHING		:= "Fishing";
Const ATTRIBUTEID_HEALING		:= "Healing";
Const ATTRIBUTEID_HIDING			:= "Hiding";
Const ATTRIBUTEID_INSCRIPTION		:= "Inscription";
Const ATTRIBUTEID_LOCKPICKING		:= "Lockpicking";
Const ATTRIBUTEID_LUMBERJACKING	:= "Lumberjacking";
Const ATTRIBUTEID_MACEFIGHTING	:= "Macefighting";
Const ATTRIBUTEID_MAGERY			:= "Magery";
Const ATTRIBUTEID_MAGICRESISTANCE	:= "Magicresistance";
Const ATTRIBUTEID_MEDITATION		:= "Meditation";
Const ATTRIBUTEID_MINING			:= "Mining";
Const ATTRIBUTEID_MUSICIANSHIP	:= "Musicianship";
Const ATTRIBUTEID_PARRY			:= "Parry";
Const ATTRIBUTEID_PEACEMAKING		:= "Peacemaking";
Const ATTRIBUTEID_POISONING		:= "Poisoning";
Const ATTRIBUTEID_PROVOCATION		:= "Provocation";
Const ATTRIBUTEID_NECROMANCY		:= "Necromancy";
Const ATTRIBUTEID_STEALING		:= "Stealing";
Const ATTRIBUTEID_STEALTH		:= "Stealth";
Const ATTRIBUTEID_SWORDSMANSHIP	:= "Swordsmanship";
Const ATTRIBUTEID_TACTICS		:= "Tactics";
Const ATTRIBUTEID_TAILORING		:= "Tailoring";
Const ATTRIBUTEID_TAMING			:= "Animaltaming";
Const ATTRIBUTEID_TINKERING		:= "Tinkering";
Const ATTRIBUTEID_TRACKING		:= "Tracking";
Const ATTRIBUTEID_WRESTLING		:= "Wrestling";
const ATTRIBUTEID_AOSNECROMANCY     := "AOSNecromancy";
const ATTRIBUTEID_FOCUS             := "Focus";
const ATTRIBUTEID_CHIVALRY          := "Chivalry";
const ATTRIBUTEID_BUSHIDO           := "Bushido";
const ATTRIBUTEID_NINJITSU          := "Ninjitsu";
const ATTRIBUTEID_SPELLWEAVING      := "Spellweaving";
const ATTRIBUTEID_MYSTICISM         := "Mysticism";
const ATTRIBUTEID_IMBUING           := "Imbuing";
const ATTRIBUTEID_THROWING          := "Throwing";
const ATTRIBUTEID_DISCORDANCE := "Discordance";


//Unused attributes on the WoD
Const ATTRIBUTEID_ITEMID			:= "ItemID";
Const ATTRIBUTEID_ARMSLORE		:= "Armslore";
Const ATTRIBUTEID_BEGGING		:= "Begging";
Const ATTRIBUTEID_BOWCRAFT		:= "Bowcraft";
Const ATTRIBUTEID_FORENSICS		:= "ForensicEvaluation";
Const ATTRIBUTEID_HERDING		:= "Herding";
Const ATTRIBUTEID_SNOOPING		:= "Snooping";
Const ATTRIBUTEID_TASTEID		:= "TasteIdentification";
Const ATTRIBUTEID_VETERINARY		:= "Veterinary";
Const ATTRIBUTEID_REMOVETRAP		:= "RemoveTrap";

//Attributes used for stats
Const ATTRIBUTEID_STRENGTH		:= "Strength";
Const ATTRIBUTEID_INTELLIGENCE	:= "Intelligence";
Const ATTRIBUTEID_DEXTERITY		:= "Dexterity";

const VITALID_LIFE                 := "Life";
const VITALID_MANA                 := "Mana";
const VITALID_STAMINA              := "Stamina";


///////////////////
//  figures out the attribute associated with the given skillid
///////////////////

function GetAttributeIDBySkillID (skillid)
	Case (skillid)
		SKILLID_ALCHEMY:
			return ATTRIBUTEID_ALCHEMY;
		SKILLID_ANATOMY:
			return ATTRIBUTEID_ANATOMY;
		SKILLID_ANIMALLORE:
			return ATTRIBUTEID_ANIMALLORE;
		SKILLID_PARRY:
			return ATTRIBUTEID_PARRY;
		SKILLID_BLACKSMITHY:
			return ATTRIBUTEID_BLACKSMITHY;
		SKILLID_PEACEMAKING:
			return ATTRIBUTEID_PEACEMAKING;
		SKILLID_CAMPING:
			return ATTRIBUTEID_CAMPING;
		SKILLID_CARPENTRY:
			return ATTRIBUTEID_CARPENTRY;
		SKILLID_CARTOGRAPHY:
			return ATTRIBUTEID_CARTOGRAPHY;
		SKILLID_COOKING:
			return ATTRIBUTEID_COOKING;
		SKILLID_DETECTHIDDEN:
			return ATTRIBUTEID_DETECTHIDDEN;
		SKILLID_LEADERSHIP:
			return ATTRIBUTEID_LEADERSHIP;
		SKILLID_INVOCATION:
			return ATTRIBUTEID_INVOCATION;
		SKILLID_HEALING:
			return ATTRIBUTEID_HEALING;
		SKILLID_FISHING:
			return ATTRIBUTEID_FISHING;
		SKILLID_HIDING:
			return ATTRIBUTEID_HIDING;
		SKILLID_PROVOCATION:
			return ATTRIBUTEID_PROVOCATION;
		SKILLID_INSCRIPTION:
			return ATTRIBUTEID_INSCRIPTION;
		SKILLID_LOCKPICKING:
			return ATTRIBUTEID_LOCKPICKING;
		SKILLID_MAGERY:
			return ATTRIBUTEID_MAGERY;
		SKILLID_MAGICRESISTANCE:
			return ATTRIBUTEID_MAGICRESISTANCE;
		SKILLID_TACTICS:
			return ATTRIBUTEID_TACTICS;
		SKILLID_MUSICIANSHIP:
			return ATTRIBUTEID_MUSICIANSHIP;
		SKILLID_POISONING:
			return ATTRIBUTEID_POISONING;
		SKILLID_ARCHERY:
			return ATTRIBUTEID_ARCHERY;
		SKILLID_NECROMANCY:
			return ATTRIBUTEID_NECROMANCY;
		SKILLID_STEALING:
			return ATTRIBUTEID_STEALING;
		SKILLID_TAILORING:
			return ATTRIBUTEID_TAILORING;
		SKILLID_TAMING:
			return ATTRIBUTEID_TAMING;
		SKILLID_TINKERING:
			return ATTRIBUTEID_TINKERING;
		SKILLID_TRACKING:
			return ATTRIBUTEID_TRACKING;
		SKILLID_SWORDSMANSHIP:
			return ATTRIBUTEID_SWORDSMANSHIP;
		SKILLID_MACEFIGHTING:
			return ATTRIBUTEID_MACEFIGHTING;
		SKILLID_FENCING:
			return ATTRIBUTEID_FENCING;
		SKILLID_WRESTLING:
			return ATTRIBUTEID_WRESTLING;
		SKILLID_LUMBERJACKING:
			return ATTRIBUTEID_LUMBERJACKING;
		SKILLID_MINING:
			return ATTRIBUTEID_MINING;
		SKILLID_MEDITATION:
			return ATTRIBUTEID_MEDITATION;
		SKILLID_STEALTH:
			return ATTRIBUTEID_STEALTH;
		SKILLID_AOSNECROMANCY:
			return ATTRIBUTEID_AOSNECROMANCY;
		SKILLID_FOCUS:
			return ATTRIBUTEID_FOCUS;
		SKILLID_CHIVALRY:
			return ATTRIBUTEID_CHIVALRY;
		SKILLID_BUSHIDO:
			return ATTRIBUTEID_BUSHIDO;
		SKILLID_NINJITSU:
			return ATTRIBUTEID_NINJITSU;
		SKILLID_SPELLWEAVING:
			return ATTRIBUTEID_SPELLWEAVING;
		SKILLID_MYSTICISM:
			return ATTRIBUTEID_MYSTICISM;
		SKILLID_IMBUING:
			return ATTRIBUTEID_IMBUING;
		SKILLID_THROWING:
			return ATTRIBUTEID_THROWING;
		SKILLID_DISCORDANCE:
			return ATTRIBUTEID_DISCORDANCE;

		//Unused skills
		SKILLID_ITEMID:
			return ATTRIBUTEID_ITEMID;
		SKILLID_ARMSLORE:
			return ATTRIBUTEID_ARMSLORE;
		// SKILLID_BEGGING:
			// return ATTRIBUTEID_BEGGING;
		SKILLID_BOWCRAFT:
			return ATTRIBUTEID_BOWCRAFT;
		SKILLID_FORENSICS:
			return ATTRIBUTEID_FORENSICS;
		SKILLID_HERDING:
			return ATTRIBUTEID_HERDING;
		SKILLID_SNOOPING:
			return ATTRIBUTEID_SNOOPING;
		SKILLID_TASTEID:
			return ATTRIBUTEID_TASTEID;
		SKILLID_VETERINARY:
			return ATTRIBUTEID_VETERINARY;
		SKILLID_REMOVETRAP:
			return ATTRIBUTEID_REMOVETRAP;
		default:
			return -1;
	endcase
endfunction



///////////////////
//  Figures out the skillid associated with the given attribute
///////////////////

function GetSkillIDByAttributeID (attributeid)
	Case (attributeid)
		ATTRIBUTEID_ALCHEMY:
			return SKILLID_ALCHEMY;
		ATTRIBUTEID_ANATOMY:
			return SKILLID_ANATOMY;
		ATTRIBUTEID_ANIMALLORE:
			return SKILLID_ANIMALLORE;
		ATTRIBUTEID_PARRY:
			return SKILLID_PARRY;
		ATTRIBUTEID_BLACKSMITHY:
			return SKILLID_BLACKSMITHY;
		ATTRIBUTEID_PEACEMAKING:
			return SKILLID_PEACEMAKING;
		ATTRIBUTEID_CAMPING:
			return SKILLID_CAMPING;
		ATTRIBUTEID_CARPENTRY:
			return SKILLID_CARPENTRY;
		// ATTRIBUTEID_STONECRAFTING:
			// return SKILLID_STONECRAFTING;
		ATTRIBUTEID_CARTOGRAPHY:
			return SKILLID_CARTOGRAPHY;
		ATTRIBUTEID_COOKING:
			return SKILLID_COOKING;
		ATTRIBUTEID_DETECTHIDDEN:
			return SKILLID_DETECTHIDDEN;
		ATTRIBUTEID_LEADERSHIP:
			return SKILLID_LEADERSHIP;
		ATTRIBUTEID_INVOCATION:
			return SKILLID_INVOCATION;
		ATTRIBUTEID_HEALING:
			return SKILLID_HEALING;
		ATTRIBUTEID_FISHING:
			return SKILLID_FISHING;
		ATTRIBUTEID_HIDING:
			return SKILLID_HIDING;
		ATTRIBUTEID_PROVOCATION:
			return SKILLID_PROVOCATION;
		ATTRIBUTEID_INSCRIPTION:
			return SKILLID_INSCRIPTION;
		ATTRIBUTEID_LOCKPICKING:
			return SKILLID_LOCKPICKING;
		ATTRIBUTEID_MAGERY:
			return SKILLID_MAGERY;
		ATTRIBUTEID_MAGICRESISTANCE:
			return SKILLID_MAGICRESISTANCE;
		ATTRIBUTEID_TACTICS:
			return SKILLID_TACTICS;
		ATTRIBUTEID_MUSICIANSHIP:
			return SKILLID_MUSICIANSHIP;
		ATTRIBUTEID_POISONING:
			return SKILLID_POISONING;
		ATTRIBUTEID_ARCHERY:
			return SKILLID_ARCHERY;
		ATTRIBUTEID_NECROMANCY:
			return SKILLID_NECROMANCY;
		ATTRIBUTEID_STEALING:
			return SKILLID_STEALING;
		ATTRIBUTEID_TAILORING:
			return SKILLID_TAILORING;
		ATTRIBUTEID_TAMING:
			return SKILLID_TAMING;
		ATTRIBUTEID_TINKERING:
			return SKILLID_TINKERING;
		ATTRIBUTEID_TRACKING:
			return SKILLID_TRACKING;
		ATTRIBUTEID_SWORDSMANSHIP:
			return SKILLID_SWORDSMANSHIP;
		ATTRIBUTEID_MACEFIGHTING:
			return SKILLID_MACEFIGHTING;
		ATTRIBUTEID_FENCING:
			return SKILLID_FENCING;
		ATTRIBUTEID_WRESTLING:
			return SKILLID_WRESTLING;
		ATTRIBUTEID_LUMBERJACKING:
			return SKILLID_LUMBERJACKING;
		ATTRIBUTEID_MINING:
			return SKILLID_MINING;
		ATTRIBUTEID_MEDITATION:
			return SKILLID_MEDITATION;
		ATTRIBUTEID_STEALTH:
			return SKILLID_STEALTH;
		ATTRIBUTEID_AOSNECROMANCY:
			return SKILLID_AOSNECROMANCY;
		ATTRIBUTEID_FOCUS:
			return SKILLID_FOCUS;
		ATTRIBUTEID_CHIVALRY:
			return SKILLID_CHIVALRY;
		ATTRIBUTEID_BUSHIDO:
			return SKILLID_BUSHIDO;
		ATTRIBUTEID_NINJITSU:
			return SKILLID_NINJITSU;
		ATTRIBUTEID_SPELLWEAVING:
			return SKILLID_SPELLWEAVING;
		ATTRIBUTEID_MYSTICISM:
			return SKILLID_MYSTICISM;
		ATTRIBUTEID_IMBUING:
			return SKILLID_IMBUING;
		ATTRIBUTEID_THROWING:
			return SKILLID_THROWING;
		ATTRIBUTEID_DISCORDANCE:
			return SKILLID_DISCORDANCE;

		//Unused skills
		ATTRIBUTEID_ITEMID:
			return SKILLID_ITEMID;
		ATTRIBUTEID_ARMSLORE:
			return SKILLID_ARMSLORE;
		ATTRIBUTEID_BEGGING:
			return SKILLID_BEGGING;
		ATTRIBUTEID_BOWCRAFT:
			return SKILLID_BOWCRAFT;
		ATTRIBUTEID_FORENSICS:
			return SKILLID_FORENSICS;
		ATTRIBUTEID_HERDING:
			return SKILLID_HERDING;
		ATTRIBUTEID_SNOOPING:
			return SKILLID_SNOOPING;
		ATTRIBUTEID_TASTEID:
			return SKILLID_TASTEID;
		ATTRIBUTEID_VETERINARY:
			return SKILLID_VETERINARY;
		ATTRIBUTEID_REMOVETRAP:
			return SKILLID_REMOVETRAP;
		default:
			return -1;
	endcase
endfunction




///////////////////
//  Returns an array of all the skill attributes
///////////////////

function ListSkillAttributes ()
	var attributes := array{
		ATTRIBUTEID_ALCHEMY,
		ATTRIBUTEID_ANATOMY,
		ATTRIBUTEID_ANIMALLORE,
		ATTRIBUTEID_ARCHERY,
		ATTRIBUTEID_LEADERSHIP,
		ATTRIBUTEID_BLACKSMITHY,
		ATTRIBUTEID_CAMPING,
		ATTRIBUTEID_CARPENTRY,
		ATTRIBUTEID_CARTOGRAPHY,
		ATTRIBUTEID_COOKING,
		ATTRIBUTEID_DETECTHIDDEN,
		ATTRIBUTEID_DISCORDANCE,
		ATTRIBUTEID_INVOCATION,
		ATTRIBUTEID_FENCING,
		ATTRIBUTEID_FISHING,
		ATTRIBUTEID_HEALING,
		ATTRIBUTEID_HIDING,
		ATTRIBUTEID_INSCRIPTION,
		ATTRIBUTEID_LOCKPICKING,
		ATTRIBUTEID_LUMBERJACKING,
		ATTRIBUTEID_MACEFIGHTING,
		ATTRIBUTEID_MAGERY,
		ATTRIBUTEID_MAGICRESISTANCE,
		ATTRIBUTEID_MEDITATION,
		ATTRIBUTEID_MINING,
		ATTRIBUTEID_MUSICIANSHIP,
		ATTRIBUTEID_PARRY,
		ATTRIBUTEID_PEACEMAKING,
		ATTRIBUTEID_POISONING,
		ATTRIBUTEID_PROVOCATION,
		ATTRIBUTEID_NECROMANCY,
		ATTRIBUTEID_STEALING,
		ATTRIBUTEID_STEALTH,
		ATTRIBUTEID_SWORDSMANSHIP,
		ATTRIBUTEID_TACTICS,
		ATTRIBUTEID_TAILORING,
		ATTRIBUTEID_TAMING,
		ATTRIBUTEID_TINKERING,
		ATTRIBUTEID_TRACKING,
		ATTRIBUTEID_WRESTLING,
		ATTRIBUTEID_AOSNECROMANCY,
		ATTRIBUTEID_FOCUS,
		ATTRIBUTEID_CHIVALRY,
		ATTRIBUTEID_BUSHIDO,
		ATTRIBUTEID_NINJITSU,
		ATTRIBUTEID_SPELLWEAVING,
		ATTRIBUTEID_MYSTICISM,
		ATTRIBUTEID_IMBUING,
		ATTRIBUTEID_THROWING
	};
	return attributes;
endfunction




///////////////////
//  Horrible, horrible, horrible cludge that's used to be able to slowly increase combat skills
//  through usage.  I've stolem bits and pieces of this out of the POL script distribution, but
//  stripped out quite a few parts that WoD doesn't use in order to increase efficiency.  One
//  big change I made was to switch from floats to ints.  I hate decimal points.
///////////////////

function AwardRawSkillPoints (character, skillid, rawpoints)
	var attributeid := GetAttributeIDBySkillID (skillid);
	if (!attributeid or attributeid == -1)
		Syslog ("Invalid skillid: " + skillid);
		return;
	endif
	AwardRawAttributePoints (character, attributeid, rawpoints);
endfunction

function AwardRawAttributePoints (character, attributeid, rawpoints)
	rawpoints := Cint (rawpoints);
	if (!rawpoints)
		return;
	endif

	var initial := GetAttributeBaseValue (character, attributeid);
	var current := initial;

	if (rawpoints > 0)
		var to_increase := GetNeededRawPointsToIncrease (current);
		while (rawpoints >= to_increase)
			rawpoints		:= rawpoints - to_increase;
			current		:= current + 1;
			to_increase	:= GetNeededRawPointsToIncrease (current);
		endwhile

		if (RandomInt (to_increase) < rawpoints)
			current := current + 1;
		endif

	elseif (rawpoints < 0)
		var to_decrease	:= GetNeededRawPointsToIncrease (current-1);
		while (rawpoints >= to_decrease and current > 0 )
			rawpoints		:= rawpoints - to_decrease;
			current		:= current - 1;
			to_decrease	:= GetNeededRawPointsToIncrease (current-1);
		endwhile

		if (RandomInt (to_decrease) < rawpoints )
			current := current - 1;
		endif

		if (current < 0)
			current := 0;
		endif
	endif

	if (current != initial)
		var skillcap := GetBaseSkillCap (character, GetSkillIDByAttributeID (attributeid));
		if (skillcap)
			if (current > skillcap)
				current := skillcap;
			endif
		endif

		SetAttributeBaseValue (character, attributeid, current);
		case (attributeid)
			ATTRIBUTEID_STRENGTH:
			ATTRIBUTEID_DEXTERITY:
			ATTRIBUTEID_INTELLIGENCE:
				RecalcVitals (character);
		endcase
	endif

	set_critical(0);
	return 1;
endfunction




///////////////////
//  Figures out how many raw points are needed to increase base skill by 0.1.  Its a crude
//  approximation of the old rawskill system.  Most of the commonly needed values are pre-calculated
//  for the sake of speed.
///////////////////

function GetNeededRawPointsToIncrease (basevalue)
	basevalue := Cint (basevalue/100);
	case (basevalue)
		0:
		1:
		2:
			return 40;
		3:
			return 81;
		4:
			return 163;
		5:
			return 327;
		6:
			return 655;
		7:
			return 1310;
		8:
			return 2621;
		9:
			return 5242;
		10:
			return 10485;
		11:
			return 20971;
		default:
			//If its above 110 skill (which it shouldn't be...)
			//We have to calculate it
			if (basevalue >= 2800)
				return 0x7fffffff;
			else
				return Cint (Exp (2.0, basevalue) * 10.24);
			endif
	endcase
	return 0x7fffffff;
endfunction




///////////////////
//  Crude approximation of the old function
///////////////////

function BaseToRawSkill (basevalue)
	if (basevalue <= 200)
		var rawvalue := basevalue * 20.48;
		return Cint (rawvalue);
	else
		var scale	:= basevalue / 100.0;
		var remain := scale - Cint (scale);
		return Cint ( Exp ( 2, Cint(scale) ) * (1024 * (1.0 + remain) + 1) );
	endif
endfunction




///////////////////
//  Crude approximation of the old function
///////////////////

function RawToBaseSkill (rawvalue)
	rawvalue := CDBL (rawvalue);
	if (!rawvalue)
		return 0;
	endif

	var scale := Cint (Log10(rawvalue)/Log10(2));
	if (scale <= 11)
		return Cint ((rawvalue+1) / 20.48);
	else
		return Cint (((rawvalue / Exp (2,scale)) + scale - 11 ) * 100);
	endif
endfunction

function GetIntelligence(who)
  return GetAttribute(who, ATTRIBUTEID_INTELLIGENCE);
endfunction

function SetHp(who, hp)
  return SetVital(who, VITALID_LIFE, Cint(hp*100));
endfunction

/***********************************************************************************
**
** function GetEffectiveSkill( who, skillid )
**
** Usage:
**	Get the effective skill of a mobile. Exact equivalent of the old
**	GetSkill(who, skillid). This include the mods.
**
** Parameters:
**	who:	 The mobile. (Mobileref)
**	skillid: The skillid. (integer)
**
** return:
**	The effective skill of the mobilein skillid. (integer ranging [0, 18000])
**
***********************************************************************************/
function GetEffectiveSkill( who, skillid )

	return GetAttribute(who, GetAttributeIdBySkillId(skillid));

endfunction

