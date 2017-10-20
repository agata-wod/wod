/*
        Maintened by *Edwards

        2009-10-22
*/
use uo;
use os;

include "include/magic";

function runebookRecall( who, rune_info, book, charges, use_charge:=0 )
	var sign := FindHouseSignAtLocation (rune_info.x, rune_info.y, rune_info.realm);
	if (sign)
		if (!IsAFriend (sign, who))
			SendSysMessage (who, "You can't recall to there!");
			return 0;
		endif
	endif
	if( use_charge )
		charges -= 1;
		SetCharges( book, who, charges );
	else
		SpeakPowerWords (who, 32);
		NewPerformAction (who, NPA_ANIM_SPELL, NPA_SPELL_DIRSPELL, NPA_DIRSPELL_SUB);
		sleep(1);
		if (!CanCastSpell (who, 4))
			Fizzle (who);
			return 0;
		endif
		if (!FreeHands (who))
			Fizzle (who);
			return 0;
		endif

		if (!CheckSkill ( who, SKILLID_MAGERY, GetSpellDifficulty (32), 0 ) )
			Fizzle (who);
			return 0;
		endif

		if(!ConsumeReagents( who, 32 ))
			PrintTextAbovePrivate (who, "*No reagents!*", who);
			Fizzle (who);
			return 0;
		endif

		if(!ConsumeSpellManaCostByID (who, 32))
			return 0;
		endif
	endif

	PlaySoundEffect (who, SFX_210);
	sleepms (250);
	MoveObjectToLocation( who, rune_info.x, rune_info.y, rune_info.z, rune_info.realm );
	PlaySoundEffect (who, SFX_SPELL_RECALL);
	
	return 1;
endfunction

function runebookGate( caster, rune_info, book )
	if (book)
	endif
	//make sure its not someone elses house
	var sign := FindHouseSignAtLocation (rune_info.x, rune_info.y, rune_info.realm);
	if (sign)
		if (!IsAFriend (sign, caster))
			SendSysMessage (caster, "You can't gate to there!");
			return 0;
		endif
	endif

	//first we try to find a gate scroll in the character's main backpack
	var gate_scroll := FindObjtypeInContainer (caster.backpack, 0x1f60);
	if (!gate_scroll)
		return TryToCastGateSpell (caster, rune_info);
	endif

	var gatemenu := CreateMenu ("How do you want to gate?");
	AddMenuItem (gatemenu, 0, "Cast Gate Spell");
	AddMenuItem (gatemenu, 0, "Use Gate Scroll");

	var menu_return := SelectMenuItem2 (caster, gatemenu);
	if (!menu_return)
		SendSysMessage (caster, "Canceled.");
		return 1;
	endif

	if (menu_return.index == 1)
		return TryToCastGateSpell (caster, rune_info);
	else
		return TryToUseGateScroll (caster, rune_info, gate_scroll);
	endif

endfunction

function TryToCastGateSpell (caster, rune_info)
	var target_x := rune_info.x;
	var target_y := rune_info.y;
	var target_z := rune_info.z;
	var target_rlm := rune_info.realm;
	if (!target_rlm)
		target_rlm := _DEFAULT_REALM;
	endif

	SpeakPowerWords (caster, 52);
	NewPerformAction (caster, NPA_ANIM_SPELL, NPA_SPELL_AREASPELL, NPA_AREASPELL_SUB);
	sleep(1);
	if (!CanCastSpell (caster, 7))
		Fizzle (caster);
		return 0;
	endif
	if (!FreeHands (caster))
		Fizzle (caster);
		return 0;
	endif

	if (!CheckSkill ( caster, SKILLID_MAGERY, GetSpellDifficulty (52), 0 ) )
		Fizzle (caster);
		return 0;
	endif

	if(!ConsumeReagents( caster, 52 ))
		PrintTextAbovePrivate (caster, "*No reagents!*", caster);
		Fizzle (caster);
		return 0;
	endif

	if(!ConsumeSpellManaCostByID (caster, 52))
		return 0;
	endif

	DoGateSpellEffect (caster, target_x, target_y, target_z, target_rlm);
	return 1;

endfunction

function TryToUseGateScroll (caster, rune_info, scroll)

	if (GetAttribute (caster, ATTRIBUTEID_MAGERY) + 20 < GetSpellDifficulty (52))
		SendSysMessage (caster, "You have no chance of using that scroll.");
		return;
	endif

	var target_x := rune_info.x;
	var target_y := rune_info.y;
	var target_z := rune_info.z;
	var target_rlm := rune_info.realm;
	if (!target_rlm)
		target_rlm := _DEFAULT_REALM;
	endif

	SpeakPowerWords (caster, 52);
	NewPerformAction (caster, NPA_ANIM_SPELL, NPA_SPELL_AREASPELL, NPA_AREASPELL_SUB);
	sleep(1);
	if (!CanCastSpell (caster, 6))
		return 0;
	endif
	if (!FreeHands (caster))
		return 0;
	endif

	if (!CheckSkill (caster, SKILLID_MAGERY, GetSpellDifficulty (52) - 20, 0 ) )
		return 0;
	endif

	if(!ConsumeSpellManaCostByID (caster, 52))
		PrintTextAbovePrivate(caster,"*No mana!*", caster);
		return 0;
	endif

	SubtractAmount (scroll, 1);
	DoGateSpellEffect (caster, target_x, target_y, target_z, target_rlm);
	return 1;

endfunction

