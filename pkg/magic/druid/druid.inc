include "include/attributes";

///////////////////
//  tries to cast the specified druid spell
///////////////////

function CastDruidSpell (caster, staff, spellid)
  if (staff)
  endif
	if (GetObjProperty (caster, "#useskill"))
		if (GetProcess (GetObjProperty (caster, "#useskill")))
			PrintTextAbovePrivate (caster, "I am already performing another action.", caster);
			return;
		endif
	endif
	
	//load the config file
	var spell_cfg_file1:= ReadConfigFile (":*:spells");
	var current_spell := FindConfigelem (spell_cfg_file1,spellid);
//	var circle := FindSpellCircle (spellid);
	
	//save the property "#castingdruidspell".  This is done in order to avoid casting more than
	//one spell at a time
	SetObjProperty (caster, "#castingdruidspell", ReadGameClock() + 60);

	//save the player's position
	var casterx := caster.x;
	var castery := caster.y;
	caster.hidden := 0;

	//play the animations
	var animation := GetConfigStringArray (current_spell, "animation");
	foreach motion in animation
    var anim := NPA_ANIM_SPELL, action := 0, subaction := 0;
    case (cint(motion))
      0x10:
        action := NPA_SPELL_AREASPELL;
        subaction := NPA_AREASPELL_SUB;
      0x11:
        action := NPA_SPELL_DIRSPELL;
        subaction := NPA_DIRSPELL_SUB;
      0x1e:
        anim := NPA_ANIM_PARRY;
      0x20:
        anim := NPA_ANIM_EMOTE;
        action := NPA_EMOTE_BOW;
      0x21:
        anim := NPA_ANIM_EMOTE;
        action := NPA_EMOTE_SALUTE;
    endcase
		NewPerformAction( caster, anim, action, subaction);
		sleepms(1500);
		if (casterx != caster.x or castery != caster.y)
			EraseObjProperty (caster, "#castingdruidspell");
			return 0;
		endif
	endforeach

	//do a skill check
	var difficulty := GetSpellDifficulty (spellid);
	if (!CheckSkill (caster, SKILLID_MAGERY, difficulty, 0))
		return 0;
	endif

//	//make sure the caster has enough mana
//	if (!ConsumeMana (caster, spellid))
//		PrintTextAbovePrivate (caster, "*Not enough mana*", caster);
//		return 0;
//	endif

	//consume the reagents
	//the "#noreagents" prop is set if the caster was a GM and declared that they didn't need reagents
	if (!GetObjProperty(caster, "#noreagents"))
		//go through all the reagents
		var reagents := GetConfigStringArray (current_spell, "reagent");
		var reg, amt, regline;
		foreach reagent in reagents
			regline := splitwords(reagent);
			reg := cint(regline[1]);
			amt := cint(regline[2]);
			if (!ConsumeSubstanceIsBroken(caster, reg, amt))
				PrintTextAbovePrivate(caster, "Not enough reagents", caster);
				return 0;
			endif
		endforeach
	endif

	ReduceCastersNecroSkill (caster);
	detach();
	start_script (current_spell.Script, caster);
	return 1;
endfunction




///////////////////
//  Reduce's the caster's necro skill for casting a druid spell
//////////////////

function ReduceCastersNecroSkill (caster)
	var skillval := GetAttributeBaseValue (caster, ATTRIBUTEID_NECROMANCY);
	if (skillval)
		skillval := CINT (skillval*0.99);
		SetAttributeBaseValue (caster, ATTRIBUTEID_NECROMANCY, skillval);
	endif
endfunction




///////////////////
//  temporary function until Consume Substance is fixed
///////////////////

function ConsumeSubstanceIsBroken (caster, what, amt)
	var mystuff := HasReagents (caster, what, amt);
	if (!mystuff)
		return 0;
	else
		SubtractAmount(mystuff,amt);
		return 1;
	endif
endfunction

function HasReagents(caster, what, amt)
	foreach item in EnumerateItemsInContainer (caster.backpack)
		if ( (item.objtype == what)  and (item.amount >= amt) )
			return item;
		endif
	endforeach

	return 0;
endfunction




///////////////////
//  Put the rune ojbtype in the bag specified.  Doesn't check to make sure its a valid rune.
///////////////////

function PutRuneInBag (bag, rune_objtype)
	var rune_array := GetObjProperty (bag, "runes");
	if (!rune_array)
		rune_array := array{};
	endif

	foreach rune in rune_array
		if (cint(rune) == cint(rune_objtype) )
			return 0;
		endif
	endforeach

	rune_array.append (rune_objtype);
	SetObjProperty (bag, "runes", rune_array);

	return 1;
endfunction



///////////////////
//  takes the rune objtype out of the bag.  Doesn't check to make sure its a valid rune
///////////////////

function TakeRuneOutOfBag (bag, rune_objtype)
	var rune_array := GetObjProperty (bag, "runes");
	if (!rune_array)
		return 0;
	endif

	var new_rune_array := array{};

	foreach rune in rune_array
		if (rune and cint(rune) != cint (rune_objtype))
			new_rune_array.append (rune);
		endif
	endforeach

	SetObjProperty (bag, "runes", new_rune_array);
	return 1;
endfunction




///////////////////
//  checks to see if the runeobjtype is in the bag
///////////////////

function CheckForRuneInBag (bag, rune_objtype)
	var rune_array := GetObjProperty (bag, "runes");
	if (!rune_array)
		return 0;
	endif

	foreach rune in rune_array
		if ( cint(rune) == cint (rune_objtype) )
			return 1;
		endif
	endforeach

	return 0;
endfunction
