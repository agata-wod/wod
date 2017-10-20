use uo;
use os;

function MakeBlankScrolls (character, hides)
	if (!ReserveItem (hides))
		SendSysMessage (character, "You cannot use that item!");
		return;
	endif

	//Get the difficulty for working with this type of hide
	var item_skill := GetSkilRequirements (hides);
	if (!item_skill)
		SendSysMessage (character, "You don't know how to use those together.");
		return;
	endif

	//Check minimum skill
	if (item_skill > GetAttribute (character, ATTRIBUTEID_LUMBERJACKING) + 15)
		SendSysMessage (character, "Your skill is too low to do that.");
		return;
	endif

	var crafting_options := GetObjProperty (character, "crafting_options");
	if (!crafting_options or len (crafting_options) < 2)
		crafting_options := {1, 0};
	endif

	var sx := character.x;
	var sy := character.y;
	var craftingbag := FindCraftingBag (character);

	repeat
		for i := 1 to GetStrokesRequired (hides)
			PlaySoundEffect (character, 0x5A);
			PerformAction (character, ANIM_SALUTE);
			sleep (1);
		endfor

		if (!SubtractAmount (hides, 1))
			PrintTextAbovePrivate (character, "*You cannot use those hides!*", character);
			return;
		endif

		if (CheckSkill (character, SKILLID_LUMBERJACKING, item_skill, 0) )

			if (!CreateItemInContainer (craftingbag, UOBJ_BLANK_SCROLL, 1))
				PrintTextAbovePrivate (character, "*Your backpack is full!*", character);
				return;
			endif

			if (!crafting_options[2])
				SendSysMessage (character, "You succeed in crafting the scroll.");
			endif
		else
			SendSysMessage (character, "You destroy some material.");
		endif

		if (!hides)
			SendSysMessage (character, "You run out of hides");
			return;
		endif
	until (character.x != sx or character.y != sy);

	SendSysMessage (character, "You stop crafting.");
endfunction





///////////////////
//  determines if the given item is a type of hide
///////////////////

function IsTypeOfHide (item)

	case (item.objtype)
		UOBJ_HIDE:
		UOBJ_BROWN_HIDE:
		UOBJ_BLUE_HIDE:
		UOBJ_RED_HIDE:
		UOBJ_GREEN_HIDE:
		UOBJ_SILVER_HIDE:
			return 1;
	endcase

	return 0;
endfunction




///////////////////
//  determines the skill level required to work with a type of hide
///////////////////

function GetSkilRequirements (item)
	case (item.objtype)
		UOBJ_HIDE:
			return 120;
		UOBJ_BROWN_HIDE:
			return 90;
		UOBJ_BLUE_HIDE:
			return 80;
		UOBJ_GREEN_HIDE:
			return 70;
		UOBJ_RED_HIDE:
			return 60;
		UOBJ_SILVER_HIDE:
			return 40;
	endcase

	return 0;
endfunction




///////////////////
//  determines the time delay involved with each type of hides
///////////////////

function GetStrokesRequired (item)
	case (item.objtype)
		UOBJ_HIDE:
			return 6;
		UOBJ_BROWN_HIDE:
			return 5;
		UOBJ_BLUE_HIDE:
			return 4;
		UOBJ_GREEN_HIDE:
			return 3;
		UOBJ_RED_HIDE:
			return 2;
		UOBJ_SILVER_HIDE:
			return 1;
	endcase

	return 0;
endfunction

