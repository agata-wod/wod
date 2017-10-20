/*
        Created by *Edwards
        
        2009-11-06
        
        Last Modifications 2009-11-19
             * Added material difficulty
*/
use uo;
use util;

include "include/string";

const CRAFT_NO_ADV := 0x1;
const CRAFT_ADV_ALL := 0x2;

const ARMOR_QUALITY_DIVIDER := 3.5;
const WEAPON_QUALITY_DIVIDER := 5;

function Craft_SkillCheck( who, elem, advance:=CRAFT_NO_ADV, material_difficulty:=0 )

        var who_skill := GetAttribute( who, craft_skill );
        if( !who_skill )
                 who_skill := 0;
        endif

	var difficulty := CDbl( elem.skill );
        if( material_difficulty > difficulty )
                difficulty := material_difficulty;
        endif
        
        var pass_success;
	if( advance & CRAFT_ADV_ALL )
		pass_success := CheckSkill( who, craft_skill, difficulty, 0 );
        else
		pass_success := CalcSuccessPercent( who, craft_skill, difficulty, who_skill );
	endif
        
	return pass_success;
endfunction

function CalcSuccessPercent (who, craft_skill, difficulty, who_skill)
	if (who or craft_skill)
	endif
	var result := CDbl( (((who_skill - difficulty) + 20)*100)/40);
	if (result > 100)
		result := 100;
	elseif (result < 0)
		result := 0;
	endif
	return result;
endfunction

function Craft_ExpCheck( who, elem )
        var who_skill := GetAttribute( who, craft_skill );
        if( !who_skill )
                 who_skill := 0;
        endif

	var difficulty := CDbl( elem.skill );
	difficulty := difficulty + 30;
	if (difficulty > 110)
		difficulty := 110;
	endif
        
        var pass_success := CalcSuccessPercent( who, craft_skill, difficulty, who_skill );
        
	return pass_success;

endfunction