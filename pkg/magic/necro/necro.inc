use uo;
use cfgfile;

include "include/client";
include "include/magic";
include "include/attributes";
include "../pkg/character/virtue/virtue";



function CastNecroSpell (caster, spellid)
	if (GetObjProperty (caster, "#useskill"))
		if (GetProcess (GetObjProperty (caster, "#useskill")))
			PrintTextAbovePrivate (caster, "I am already performing another action.", caster);
			return;
		endif
	endif

	//load the config file
	var spell_cfg_file1:= ReadConfigFile(":*:spells");
	var current_spell := FindConfigelem (spell_cfg_file1, spellid);
	if (!current_spell)
		return;
	endif

	if (!GetObjProperty (caster, "#NoNecroWords"))
		SpeakPowerWords (caster, spellid);
	endif
	EraseObjProperty (caster, "#NoNecroWords");

	var circle := FindSpellCircle (spellid);
	if (!CanCastSpell (caster, circle) or !FreeHands (caster))
		return;
	endif

	var i;
	var spelldelay := GetSpellDelay (spellid);
	for (i := 1; i <= spelldelay; i := i + 1)
		NewPerformAction (NPA_ANIM_SPELL, NPA_SPELL_AREASPELL, NPA_AREASPELL_SUB);
		spelldelay := spelldelay - 1;
		sleep(1);
	endfor
	sleepms(500);

	//do a skill check
	var difficulty := GetSpellDifficulty (spellid);
	if (!CheckSkill (caster, SKILLID_MAGERY, difficulty, 0))
		PlayObjectCenteredEffect (caster, FX_SPELL_FAIL, 7, 7);
		PlaySoundEffect (caster, SFX_5B);
		return 0;
	endif

	//consume the reagents
	//the "#noreagents" prop is set if the caster was a GM and declared that they didn't need reagents
	if(!GetObjProperty (caster, "#noreagents"))
		if (!ConsumeReagents(caster, spellid))
			PlayObjectCenteredEffect(caster, FX_SPELL_FAIL, 7, 7);
			PlaySoundEffect(caster, SFX_5B);
			SendSysMessage(caster, "Insufficient Reagents.");
			return 0;
		endif
	endif

	LoseSkill(caster);
	AddVirtue(caster,-1);

	start_script (current_spell.Script, caster);
	return 1;
endfunction




///////////////////
//  Reduces the caster's druid skill just for trying to cast a necro spell
///////////////////

function LoseSkill (caster)
	var skillval := GetAttributeBaseValue (caster, ATTRIBUTEID_ANIMALLORE);
	if (skillval)
		skillval := CINT (skillval*0.99);
		SetAttributeBaseValue (caster, ATTRIBUTEID_ANIMALLORE, skillval);
	endif
endfunction




///////////////////
//  Finds the delay for casting the spell
///////////////////

function GetSpellDelay(spellid)

	var spellcfg := ReadConfigFile("::spells");
	var circle := spellcfg[spellid].circle;
	var circlecfg := ReadConfigFile("::circles");
	var delay := circlecfg[circle].Delay;
	return cint(delay/1000);

endfunction

