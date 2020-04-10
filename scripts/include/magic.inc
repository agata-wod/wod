use uo;
use os;
use cfgfile;
use util;

include "include/dotempmods";
include "include/eventid";
//include "include/math";
include "include/objtype";
include "include/utility";
include "include/attributes";
include "include/poison";
include "include/effects";
include "include/wodinc";
include "../pkg/npcs/npcinfo";
include "../pkg/npcs/npc_util";
include "../pkg/character/virtue/virtue";
include "include/newperformaction";

const CSSC_NOREAGENTS       := 0x01;
const CSSC_HANDSFREE         := 0x02;
const CSSC_NOSKILLCHECK     := 0x04;
const CSSC_NODELAY            := 0x08;
const CSSC_NOUNHIDE           := 0x10;

Const STATMOD_STR			:= 1;
Const STATMOD_DEX			:= 2;
Const STATMOD_INT			:= 3;
Const STATMOD_ALL			:= 4;
Const STATMOD_AR			:= 5;
Const STATMOD_PARALYZE		:= 6;
Const STATMOD_INCOGNITO		:= 7;

var caster_magery;
var caster_evalint;

///////////////////
//  Determines the caster of the spell based on the parameters passed
///////////////////

function GetSpellCaster (byref parms)
	var caster;
	if (parms[1] == "#MOB")
		caster := parms[2];
		caster_magery := GetAttribute (caster, ATTRIBUTEID_MAGERY);
		if (!caster_magery)
			caster_magery := 100;
		endif
		caster_evalint := GetAttribute (caster, ATTRIBUTEID_INVOCATION);
		if (!caster_evalint)
			caster_evalint := 100;
		endif
		return caster;

	elseif (parms[1] == "#WAND")
		caster := parms[2];
		caster_magery := 100;
		caster_evalint := 100;
		return caster;

	elseif (parms[1] == "#SCROLL")
		caster := parms[2];
		caster_magery := GetAttribute (caster, ATTRIBUTEID_MAGERY);
		caster_evalint := GetAttribute (caster, ATTRIBUTEID_INVOCATION);
		return caster;

	elseif (parms[1] == "#SPECIALNONE");
		caster_magery := 100;
		caster_evalint := 100;
		return "nocaster";

	else
		caster := parms;
		caster_magery := GetAttribute (caster, ATTRIBUTEID_MAGERY);
		caster_evalint := GetAttribute (caster, ATTRIBUTEID_INVOCATION);
	endif
	return caster;
endfunction




///////////////////
//  Determines the success of the spell from the parameters passed and other checks
///////////////////

function CheckSuccessfulSpellCast (byref parms, spellid := 0, flags := 0)
	if (parms[1] == "#MOB")
		return 1;
	elseif (parms[1] == "#WAND")
		var thecaster := parms [2];
		if (!CanCastSpell (thecaster, spell_circle))
			return 0;
		endif
		if(!(flags & CSSC_HANDSFREE))
			if (!FreeHands (thecaster))
				return 0;
			endif
		endif
		if (!(flags & CSSC_NOUNHIDE))
			thecaster.hidden := 0;
		endif
		return 1;

	elseif (parms[1] == "#SCROLL")
		var thecaster := parms[2];
		if (!CanCastSpell (thecaster, spell_circle))
			return 0;
		endif
		if(!(flags & CSSC_HANDSFREE))
			if (!FreeHands (thecaster))
				return 0;
			endif
		endif
		if (!(flags & CSSC_NOUNHIDE))
			thecaster.hidden := 0;
		endif
		return 1;

	elseif (parms[1] == "#SPECIALNONE");
		return 1;

	else
		var thecaster := parms;
		if (GetObjProperty (thecaster, "#useskill"))
			if (GetProcess (GetObjProperty (thecaster, "#useskill")))
				PrintTextAbovePrivate (thecaster, "I am already performing another action.", thecaster);
				return 0;
			endif
		endif
		if (!CanCastSpell (thecaster, spell_circle))
			return 0;
		endif
		if (!(flags & CSSC_HANDSFREE))
			if (!FreeHands (thecaster))
				return 0;
			endif
		endif
		if (!(flags & CSSC_NOUNHIDE))
			thecaster.hidden := 0;
		endif
		if (!spellid)
			if(!ConsumeSpellManaCost (thecaster, spell_circle))
				return 0;
			endif
		else
			if (!GetObjProperty(thecaster, "#scroll"))
				SpeakPowerWords (thecaster, spellid);
				NewPerformAction (thecaster, NPA_ANIM_SPELL, NPA_SPELL_DIRSPELL, NPA_DIRSPELL_SUB);
				if(!(flags & CSSC_NOREAGENTS))
					if (!ConsumeSpellReagents (thecaster, spellid))
						return 0;
					endif
				endif
				if (!(flags & CSSC_NODELAY))
					var orig_hp := GetVital( thecaster, "Hits" ),
					delay := GetTimeDelayBySpellID (spellid);
//					var orig_x := thecaster.x, orig_y := thecaster.y;
             
					// Check for health changes and fail the spell if caster becomes damaged
					while( delay > 0 )
						if( orig_hp > GetVital( thecaster, "Hits"))
							Fizzle(thecaster);
							return 0;
						elseif( thecaster.frozen || thecaster.paralyzed )
							Fizzle(thecaster);
							return 0;
						endif

						delay -= 100;
						SleepMS(100);
					endwhile
//					if( thecaster.x != orig_x || thecaster.y != orig_y )
//						SendSysMessage( thecaster, "Your concentration is disturbed while moving." );
//						return 0;
//					endif
				endif
				if (!(flags & CSSC_NOSKILLCHECK))
					if (!CheckSkill (thecaster, SKILLID_MAGERY, GetSpellDifficulty(spellid), 0))
						Fizzle(thecaster);
						return 0;
					endif
				endif
			endif
			if(!ConsumeSpellManaCostByID (thecaster, spellid))
				return 0;
			endif			
		endif
		return 1;
	endif
	return 1;
endfunction



///////////////////
//  Returns the target of the spell.  If the caster of the spell is an NPC, as determined by the
//  parameters passed, its chosen from the parameters automatically.  Otherwise, it allows the
//  player to chose the target.
///////////////////

function GetSpellCastOn (byref caster, byref parms, targetting_options := 0)
	var cast_on;
	if (parms[1] == "#MOB")
		return parms[3];

	elseif (parms[1] == "#WAND")
		return parms[3];

	elseif (parms[1] == "#SCROLL")
		var cx := caster.x;
		var cy := caster.y;

		if (!targetting_options)
			cast_on := TargetCoordinates (caster);
			if (!cast_on)
				SendSysMessage (caster, "Canceled.");
				return 0;
			endif
		else
			cast_on := Target (caster, targetting_options);
			if(!cast_on)
				SendSysMessage (caster, "Canceled.");
				return 0;
			endif
			if (!GetVital (cast_on, "Life"))
				SendSysMessage (caster, "That is not a valid target!");
				return 0;
			endif
		endif

		if (cx != caster.x or cy != caster.y)
			SendSysMessage (caster,"You've lost your concentration.");
			return 0;
		endif

		if (!CheckLosAt (caster, cast_on.x, cast_on.y, cast_on.z) and !CheckLosAt (caster, cast_on.x, cast_on.y, cast_on.z + cast_on.height))
			SendSysMessage (caster, "You can't see that.");
			return 0;
		endif
		SubtractAmount (parms[3], 1);

	elseif (parms[1] == "#SPECIALNONE");
		return parms[2];

	else
		var cx := caster.x;
		var cy := caster.y;

		if (!targetting_options)
			cast_on := TargetCoordinates (caster);
			if (!cast_on)
				SendSysMessage (caster, "Canceled.");
				GiveBackMana (caster);
				return 0;
			endif
		else
			cast_on := Target (caster, targetting_options);
			if(!cast_on)
				SendSysMessage (caster, "Canceled.");
				GiveBackMana (caster);
				return 0;
			endif
			if (!GetVital (cast_on, "Life"))
				SendSysMessage (caster, "That is not a valid target!");
				GiveBackMana (caster);
				return 0;
			endif
			if (cast_on.hidden)
				SendSysMessage (caster, "You can't see that!");
				GiveBackMana (caster);
				return 0;
			endif
		endif

		if (cx != caster.x or cy != caster.y)
			SendSysMessage (caster, "You've lost your concentration.");
			GiveBackMana (caster);
			return 0;
		endif

		if (!CheckLosAt (caster, cast_on.x, cast_on.y, cast_on.z) and !CheckLosAt (caster, cast_on.x, cast_on.y, cast_on.z + cast_on.height))
			SendSysMessage (caster, "You can't see that.");
			GiveBackMana (caster);
			return 0;
		endif
	endif

	return cast_on;
endfunction




///////////////////
//  should be called whenever a spell changes a stat
///////////////////

function DoMagicStatMod (byref cast_on, modtype, modamount, duration, description)
	var parms := array{};
	parms[1] := cast_on;
	parms[2] := modtype;
	parms[3] := modamount;
	parms[4] := duration;
	parms[5] := description;
	start_script (":magiceffects:domagicstatmod", parms);
endfunction




///////////////////
//  figures out the final target of the spell after all the reflections are done
///////////////////

function CheckMagicReflection (caster, cast_on)
	if (GetObjProperty (cast_on, "mr"))
		sleepms (500);
		PlaySoundEffect (cast_on, SFX_SPELL_MAGIC_REFLECTION);
		PlayObjectCenteredEffect (cast_on, FX_CURSE_EFFECT, 10, 10);
		SendSysMessage (cast_on, "Your magic reflect spell saved you!");
		EraseObjProperty (cast_on, "mr");

		if (!caster)
			return 0;
		endif

		if (GetObjProperty (caster, "mr"))
			EraseObjProperty (caster, "mr");
			SendSysMessage (caster, "Your magic reflect spell saved you!");
			return 0;
		else
			return caster;
		endif
	else
		return cast_on;
	endif
endfunction




///////////////////
//  determines if the character can cast a spell of the given circle dependant upon the equipped armor
///////////////////

function CanCastSpell (byref caster, circle)
	if (!GetGlobalProperty ("restrictarmor"))
		return 1;
	endif

	var cfg := ReadConfigFile (":*:itemdesc");
	foreach item in ListEquippedItems (caster)
		if (cfg[item.objtype].blockcircle && cfg[item.objtype].blockcircle <= circle && !cfg[item.objtype].errortext) 
			SendSysmessage (caster, "Your equipment prevents you from casting this spell!");
			return 0;
		endif
	endforeach

	return 1;
endfunction




///////////////////
//  determines if the weapons equipped by the character will interfer with spellcasting
///////////////////

function FreeHands (byref me)
	var cfg := ReadConfigFile (":*:itemdesc");
	var weapon1 := GetEquipmentByLayer (me, LAYER_HAND1);
	if (!FindConfigElem (cfg, weapon1.objtype) )
		weapon1 := 0;
	endif

	var weapon2 := GetEquipmentByLayer (me, LAYER_HAND2);
	if (!FindConfigElem (cfg, weapon2.objtype) )
		weapon2 := 0;
	endif

	if (!weapon1 and !weapon2)
		return 1;
	endif

	if (cfg[weapon1.objtype].blocksCastingIfInHand==0 and !weapon2)
		return 1;
	elseif (cfg[weapon2.objtype].blocksCastingIfInHand==0 and !weapon1)
		return 1;
	endif

	PrintTextAbovePrivate (me, "Your hands must be free to cast that spell!", me);
	return 0;
endfunction




///////////////////
//  calculates spell damage based on the circle of the spell
///////////////////

function CalcSpellDamageByCircle (circle)
	var die_string := CSTR(2*circle)+"d3";
	var dmg := RandomDiceRoll (die_string) + RandomInt (CINT (caster_magery/10));
	return dmg;
endfunction




///////////////////
//  Figures out how strong a buff should be
///////////////////

function CalcModAmount () 
	var mod_amount := RandomInt (5) + CINT(caster_magery/10);
	if (caster_evalint > 95)
		mod_amount := mod_amount + 3;
	elseif (caster_evalint > 75)
		mod_amount := mod_amount + 1;
	endif
	return mod_amount;
endfunction




///////////////////
//  Figure out how long a buff should last
///////////////////

function CalcModDuration ()
	var duration := 300 + caster_magery * 10 + caster_evalint * 10;
	return duration;
endfunction

///////////////////
//  This function is for curses (aka negative buffs)
///////////////////

function CalcCurseModDuration ()
	var duration := 10 + CINT (caster_magery/2) + CINT (caster_evalint/2);
	return duration;
endfunction



///////////////////
//  returns a modifies amount of damage based upon the casters evalint as compared to the
//  targets magic resistance
///////////////////

function Resisted (byref caster, byref cast_on, amt)
  if (caster)
  endif
	var cast_on_resist := GetAttribute (cast_on, ATTRIBUTEID_MAGICRESISTANCE);
	if (cast_on_resist > 90)
		cast_on_resist := CINT (cast_on_resist * 1.4);
	elseif (cast_on_resist > 70)
		cast_on_resist := CINT (cast_on_resist * 1.2);
	endif

	var dmg := CINT((caster_evalint * amt)/(cast_on_resist + 20));
	if (dmg > CINT (amt*1.5))
		dmg := CINT (amt * 1.5);
	elseif (dmg < CINT (amt/2))
		dmg := CINT (amt/2);
	endif

	if (dmg < amt)
		SendSysMessage (cast_on, "You feel yourself resisting magical energy!");
		PlaySoundEffect (cast_on, SFX_SPELL_WEAKEN);
		PlayObjectCenteredEffect (cast_on, FX_CURSE_EFFECT, 5, 5);
	endif
	if (caster.race == RACE_GARGOYLE)
		var berserk_mod := 0.03 * CINT(5.0 - ((GetVital (caster, "Hits") * 5.0) / GetVitalMaximumValue (caster, "Hits")));
		dmg := CINT(dmg * (1.0 + berserk_mod));
	endif

	return dmg;
endfunction




///////////////////
//  check to see if the source of the spell resists
///////////////////

function DruidResist (byref caster, byref cast_on, amt)
	var cast_on_resist := GetAttribute (cast_on, ATTRIBUTEID_MAGICRESISTANCE);
	var caster_animallore := GetAttribute (caster, ATTRIBUTEID_ANIMALLORE);

	var dmg := CINT((caster_animallore * amt)/(cast_on_resist + 20));
	if (dmg > CINT (amt*1.5))
		dmg := CINT (amt * 1.5);
	elseif (dmg < CINT (amt/2))
		dmg := CINT (amt/2);
	endif

	if (dmg < amt)
		SendSysMessage (cast_on, "You feel yourself resisting magical energy!");
		PlaySoundEffect (cast_on, SFX_SPELL_WEAKEN);
		PlayObjectCenteredEffect (cast_on, FX_CURSE_EFFECT, 5, 5);
	endif
	return dmg;

endfunction




///////////////////
//  Summons the passed creature at the passed location, then controls it
///////////////////

function SummonCreature (byref caster, npctemplate, cast_loc, duration, controllable := 1, npc_name := "", npc_color := 0, savedprop := 0)
	var parms := array{};
	parms[1] := caster;
	parms[2] := npctemplate;
	parms[3] := cast_loc;
	parms[4] := duration;
	parms[5] := controllable;
	parms[6] := npc_name;
	parms[7] := npc_color;
	parms[8] := savedprop;

	start_script (":magiceffects:summoning", parms);
endfunction




///////////////////
// creates a field of length 5 centered on the target
///////////////////

function CreateField (byref caster, targetx, targety, targetz, field_type, duration, direction := 0, targetr)
	if (!direction)
		direction := GetFieldDirection (caster, targetx, targety);
	endif

	var field_objtype := 0;
	var field_graphic := 0;
	if (field_type == "fire" and direction == "ns")
		field_objtype := 0x3996;
	elseif (field_type == "fire" and direction == "ew")
		field_objtype := 0x398c;
	elseif (field_type == "energy" and direction == "ns")
		field_objtype := 0x3956;
		field_graphic := 0x3979;
	elseif (field_type == "energy" and direction == "ew")
		field_objtype := 0x3946;
		field_graphic := 0x3967;
	elseif (field_type == "poison" and direction == "ns")
		field_objtype := 0x3920;
	elseif (field_type == "poison" and direction == "ew")
		field_objtype := 0x3915;
	elseif (field_type == "para" and direction == "ns")
		field_objtype := 0x3979;
	elseif (field_type == "para" and direction == "ew")
		field_objtype := 0x3967;
	elseif ( field_type == "stone" and direction == "ns")
		field_objtype := 0x1f7;
		field_graphic := 0x57;
	elseif ( field_type == "stone" and direction == "ew")
		field_objtype := 0x1f7;
		field_graphic := 0x58;
	elseif (field_type == "frost" and direction == "ns")
		field_objtype := 0x17086;
	elseif (field_type == "frost" and direction == "ew")
		field_objtype := 0x17085;
	else
		return;
	endif

	var field := array{ };

	var i, it;
	for ( i := -2; i <= 2; i := i + 1 )
		if (direction == "ew")
			if (caster)
				if (CheckLosAt (caster, targetx + i, targety, targetz) )
					it := CreateItemAtLocation (targetx + i, targety, targetz, field_objtype, 1, targetr);
					if (it)
						it.movable := 0;
						if (field_graphic)
							it.graphic := field_graphic;
						endif
						SetObjProperty (it, "creator", caster.serial);
						SetObjProperty (it, "summoned_field", 1);
						SetObjProperty (it, "caster_evalint", caster_evalint);
						field.append( it );
					endif
				endif

			else	//if there is no caster, we don't need to worry about LOS
				it := CreateItemAtLocation (targetx + i, targety, targetz, field_objtype, 1, targetr);
				if (it)
					it.movable := 0;
					if (field_graphic)
						it.graphic := field_graphic;
					endif
					SetObjProperty (it, "creator", caster.serial);
					SetObjProperty (it, "summoned_field", 1);
					field.append( it );
				endif
			endif
		else
			if (caster)
				if (CheckLosAt (caster, targetx, targety + i, targetz) )
					it := CreateItemAtLocation (targetx, targety + i, targetz, field_objtype, 1, targetr);
					if (it)
						it.movable := 0;
						if (field_graphic)
							it.graphic := field_graphic;
						endif
						SetObjProperty (it, "creator", caster.serial);
						SetObjProperty (it, "summoned_field", 1);
						field.append( it );
					endif
				endif
			else	//again, if there's no caster, don't check LOS
				it := CreateItemAtLocation (targetx, targety + i, targetz, field_objtype, 1, targetr);
				if (it)
					it.movable := 0;
					if (field_graphic)
						it.graphic := field_graphic;
					endif
					SetObjProperty (it, "creator", caster.serial);
					SetObjProperty (it, "summoned_field", 1);
					field.append( it );
				endif
			endif
		endif
		if (i != 0 and it)
			SetObjProperty (it, "#suppresssound", 1);
		endif
	endfor

	detach();
	sleep (duration);

	foreach item in (field)
		DestroyItem (item);
	endforeach
endfunction




///////////////////
// returns the direction that the fields should face
///////////////////

function GetFieldDirection (byref caster, targetx, targety)
	var xdiff;
	var ydiff;
	if (caster.x > targetx)
		xdiff := caster.x - targetx;
	else
		xdiff := targetx - caster.x;
	endif

	if (caster.y > targety)
		ydiff := caster.y - targety;
	else
		ydiff := targety - caster.y;
	endif

	if (!xdiff and !ydiff)
		return "ew";
	elseif (xdiff == ydiff)
		return "ns";
	elseif (xdiff > ydiff)
		return "ns";
	else
		return "ew";
	endif
endfunction




///////////////////
//  Give back half the mana if the fail can't be cast for some reason
///////////////////

function GiveBackMana (byref caster)
	var mana_required := GetManaCostBySpellCircle (spell_circle);
	if (mana_required)
		SetVital (caster, "Mana", GetVital (caster, "Mana") + mana_required * 50);
	endif
	if (GetVital (caster, "Mana") > GetVitalMaximumValue (caster, "Mana"))
		SetVital (caster, "Mana", GetVitalmaximumValue (caster, "Mana"));
	endif
endfunction




///////////////////
//  Finds the circle for the given spell
///////////////////

function FindSpellCircle (spellid)
	var spellcfg := ReadConfigFile(":*:spells");
	var circle := spellcfg[spellid].circle;
	return circle;
endfunction




///////////////////
//  Does the gate spell effect...
///////////////////

function DoGateSpellEffect (byref caster, destinationx, destinationy, destinationz, destinationrealm)
	if (!caster.cmdlevel)
		if (destinationrealm == REALM_BRITANNIA)
			if (destinationx > 5118)
				if (destinationx < 6143 and destinationy > 2303)
					// T2A
				elseif (destinationx > 6913 and destinationx < 7160 and destinationy > 255 and destinationy < 512)
					//Heartwood
				else
					//Dungeons
					sendsysmessage (caster, "Blocked!");
					return 0;
				endif
			//seal off Buccaneer's Den from outside gates
			elseif (destinationx > 2560 and destinationx < 2960 and destinationy > 1940 and destinationy < 2340)
				SendSysMessage (caster, "Your gate spell has somehow been interrupted.");
				return;
			//seal off Hidden Valley from outside gates
			elseif (destinationx > 1655 and destinationx < 1695 and destinationy > 2935 and destinationy < 3005)
				SendSysMessage (caster, "Your gate spell has somehow been interrupted.");
				return;
			//seal off the valleys connecting Covetous from gates...
			elseif (destinationx > 2440 and destinationx < 2560 and destinationy > 850 and destinationy < 910)
				SendSysMessage (caster, "Your gate spell has somehow been interrupted.");
				return;
			elseif (destinationx > 2350 and destinationx < 2430 and destinationy > 830 and destinationy < 910)
				SendSysMessage (caster, "Your gate spell has somehow been interrupted.");
				return;
			endif
			
			//No gating into or out of jail
			if (caster.x > 5268 and caster.x < 5315 and caster.y > 1154 and caster.y < 1197)
				SendSysMessage (caster, "Leaving so soon?  I don't think so.");
				return;
			endif
			
			//completely block off Famine (you can't even gate out of famine)
			if (caster.x > 5260 and caster.x < 5340 and caster.y > 1870 and caster.y < 1940)
				SendSysMessage (caster, "Your gate spell has somehow been interrupted.");
				return;
			elseif (caster.x > 5670 and caster.x < 5870 and caster.y > 780 and caster.y < 1010)
				SendSysMessage (caster, "Your gate spell has somehow been interrupted.");
				return;
			elseif (caster.x > 5240 and caster.x < 5280 and caster.y > 1740 and caster.y < 1780)
				SendSysMessage (caster, "Your gate spell has somehow been interrupted.");
				return;
			//elseif (caster.x > 5720 and caster.y > 2250 and caster.y < 2600)
			//	SendSysMessage (caster, "Your gate spell has somehow been interrupted.");
			//	return;
			////no gating into the snowball fight area
			elseif (caster.x > 5260 and caster.x < 5340 and caster.y > 1870 and caster.y < 1940)
				SendSysMessage (caster, "Your gate spell has somehow been interrupted.");
				return;
			endif


			//Can't gate directly into or out of Britain Bank
			if (destinationx > 1435 and destinationx < 1475 and destinationy > 1545 and destinationy < 1560)
				SendSysMessage (caster, "Gating directly into the bank would be tacky");
				return;
			elseif (caster.x > 1435 and caster.x < 1475 and caster.y > 1545 and caster.y < 1560)
				SendSysMessage (caster, "Gating directly into the bank would be tacky");
				return;
			elseif (destinationx > 1435 and destinationx < 1448 and destinationy > 1545 and destinationy < 1568)
				SendSysMessage (caster, "Gating directly into the bank would be tacky");
				return;
			elseif (caster.x > 1435 and caster.x < 1448 and caster.y > 1545 and caster.y < 1568)
				SendSysMessage (caster, "Gating directly into the bank would be tacky");
				return;
			elseif (destinationx > 1462 and destinationx < 1475 and destinationy > 1545 and destinationy < 1568)
				SendSysMessage (caster, "Gating directly into the bank would be tacky");
				return;
			elseif (caster.x > 1462 and caster.x < 1475 and caster.y > 1545 and caster.y < 1568)
				SendSysMessage (caster, "Gating directly into the bank would be tacky");
				return;
			endif
		elseif (destinationrealm == REALM_BRITANNIA_ALT)
			sendsysmessage (caster, "Blocked!");
			return 0;		
		elseif (destinationrealm == REALM_ILSHENAR)
			sendsysmessage (caster, "Blocked!");
			return 0;	
		elseif (destinationrealm == REALM_MALAS)
			if (destinationx < 513)
				sendsysmessage (caster, "Blocked!");
				return 0;
			endif
		elseif (destinationrealm == REALM_TERMUR)
			if (destinationy < 2840)
				sendsysmessage (caster, "Blocked!");
				return 0;
			endif
		endif
	endif
	
	var casterx := caster.x;
	var castery := caster.y;
	var casterz := caster.z;
	var casterrlm := caster.realm;

	set_critical (1);
	var pregate := CreateItemAtLocation (casterx, castery, casterz, 0x1af3, 1, casterrlm);
	pregate.movable := 0;
	pregate.facing := 1;
	PlaySoundEffect (caster, SFX_SPELL_GATE_TRAVEL);
	sleepms (1500);
	DestroyItem (pregate);

	var gate1 := CreateItemAtLocation (casterx, castery, casterz, UOBJ_BLUE_MOONGATE, 1, casterrlm);
	gate1.movable := 0;
	gate1.facing := 1;
	var gate2 := CreateItemAtLocation (destinationx, destinationy, destinationz, UOBJ_BLUE_MOONGATE, 1, destinationrealm);
	gate2.movable := 0;
	gate2.facing := 1;
	if (!gate1 or !gate2)
        	return;
	endif
	
	gate1.SetDestination(gate2.x, gate2.y, gate2.z, gate2.realm);
	gate2.SetDestination(gate1.x, gate1.y, gate1.z, gate1.realm);

	SetObjProperty (gate1, "summoned_field", 1);
	SetObjProperty (gate2, "summoned_field", 1);

	Detach();
	set_critical( 0 );

	var timer_counter := 0;
	repeat
		sleep (1);
		if (!gate1 or !gate2)
			DestroyItem (gate1);
			DestroyItem (gate2);
			return;
		endif
		timer_counter := timer_counter + 1;
	until (timer_counter > 30);
	DestroyItem (gate1);
	DestroyItem (gate2);
endfunction




///////////////////
//  Since there seems to be a problem with ConsumeMana and default spellcasting, this is a workaround
//  to consume the spell's required mana while its being cast
///////////////////

function ConsumeSpellManaCost (caster, circle)
	if (!caster)
		return 0;
	endif
	var mana_required := GetManaCostBySpellCircle (circle);
	if (!mana_required)
		return 1;
	endif
	
	set_critical (1);
	if (GetVital (caster, "Mana") >= mana_required * 100)
		SetVital (caster, "Mana", GetVital (caster, "Mana") - mana_required * 100);
		set_critical (0);
		return 1;
	endif

	set_critical (0);
	SendSysMessage (caster, "You do not have enough mana to cast that spell.");
	Fizzle (caster);
	return 0;
endfunction

function ConsumeSpellManaCostByID (caster, spellid)
	if (!caster)
		return 0;
	endif
	
	if (!ConsumeMana(caster, spellid))
		SendSysMessage (caster, "You do not have enough mana to cast that spell.");
		Fizzle (caster);
		return 0;
	endif
	
	return 1;
endfunction

///////////////////
//  Consume the reagents
//////////////////

function ConsumeSpellReagents (caster, spellid)
	if (!caster)
		return 0;
	endif
	if(!ConsumeReagents(caster, spellid))
		PrintTextAbovePrivate (caster, "You don't have enough reagents!", caster);
		Fizzle (caster);
		return 0;
	endif
	return 1;
endfunction


///////////////////
//  Figures out what spell circle a spellid is associated with
///////////////////

function GetSpellCircleBySpellID (spellid)
	var spell_cfg := ReadConfigFile (":*:spells");
	return spell_cfg[spellid].circle;
endfunction




///////////////////
//  what happens when the spell fizzles
///////////////////

function Fizzle (caster)
	PlaySoundEffect (caster, 0x005d);
	PlayObjectCenteredEffect (caster, 0x3735, 0x0a, 0x1e);
	sleep (3);
endfunction




///////////////////
//  Gets the amount of mana that a spell should cost based on its circle
///////////////////

function GetManaCostBySpellCircle (spellcircle)
	var cfg := readconfigfile("::circles");
	var spellmana := cint(cfg[ spellcircle ].mana);
	if (!spellmana)
		Syslog ("Error finding spell circle!");
		return 20;
	endif
	return spellmana;
endfunction




///////////////////
//  Gets the amount of mana that a spell should cost based on its spellid
///////////////////

function GetManaCostBySpellID (spellid)
	var cfg := readconfigfile(":*:spells");
	var spellmana := CINT(cfg[ spellid ].mana);
	if (!spellmana)
		spellmana := cfg[ spellid ].circle;
		cfg := readconfigfile("::circles");
		spellmana := cint(cfg[ spellmana ].mana);
    if (!spellmana)
      Syslog ("Error finding spell circle!");
      return 20;
    endif
	endif
	return spellmana;
endfunction

function GetTimeDelayBySpellID (spellid)
	var cfg := readconfigfile(":*:spells");
	var spelldelay := CINT(cfg[spellid].delay);
	if (!spelldelay)
		spelldelay := cfg[ spellid ].circle;
		cfg := readconfigfile("::circles");
		spelldelay := cint(cfg[spelldelay].delay);
    if (!spelldelay)
      syslog ("Can't find spell circle " + cfg[spellid].circle + " on spellid " + spellid);
      return 2000;
    endif
	endif
	return spelldelay;
endfunction

