/*
        Created by *Edwards

        2009-11-06
        
        Last Modifications 2010-02-16
             * Added : Checks for LFucker items so they cannot be used to create new objects
*/
use uo;

include "include/facings";
include "include/string";
include ":crafting:craftGumps";
include ":crafting:craftSkill";
include ":crafting:repairItem";

const CRAFT_DELAY := 1;

var itemConfig := ReadConfigFile( ":*:itemdesc" ),
    craft_skill,
    display_skill,
    craftL := 0,
    rcolor := 1,
    quality := 0,
    makermark := 1;
    
function Craft_CheckParts( who, elem )

	var parts_list := SplitWords( elem.parts );
	if( TypeOf( parts_list ) != "Array" || !parts_list )
		SendSysMessage( who, "This item does not require parts to be made! Please, contact a staff member." );
		return 0;
	endif

	var parts := dictionary;
	for i := 1 to ( parts_list.Size() / 2 )
		parts[CInt( parts_list[i] )] := CInt( parts_list[i+( parts_list.Size() / 2 )] );
                SleepMS(5);
	endfor

	var material_list := dictionary;

	foreach thing in ( EnumerateItemsInContainer( who.backpack ))
		// If thing.objtype is the same as the objtype in the part list array position....
		var thing_objtype := CInt( thing.objtype );
		if( thing_objtype in parts.Keys() )
			if( CInt( material_list[thing_objtype] ) < CInt( parts[thing_objtype] ))
				material_list[thing_objtype] := CInt( material_list[thing_objtype] ) + thing.amount;
			endif
		endif
		SleepMS(5);
	endforeach

	foreach part in ( parts.Keys() )
		if( material_list[part] < parts[part] )
			SendSysMessage( who, "You do not have the required materials to make that." );
			return 0;
		endif
		SleepMS(5);
	endforeach

	return parts;
endfunction

function MaterialSelection( who, craft_skill, type )

	SendSysMessage( who, "Select the "+Lower( type )+" you would like to work with." );
	var material := Target( who );
	if( !material )
		return 0;
	endif

	if( !canUseMaterial( who, craft_skill, itemConfig[material.objtype].Difficulty ))
		SendSysMessage( who, "You are not skilled enough to make anything out of that." );
		return 0;
	endif
 
        var material_objtype := 0;
	if( CInt( GetObjProperty( material, type )) > 0 )
		material_objtype := GetObjProperty( material, type );
	else
		material_objtype := material.objtype;
	endif

	var rtype := Lower( itemConfig[material_objtype].Type );
	if( rtype != type )
		SendSysMessage( who, "That's not the proper material." );
                return 0;
	endif
	
	if (material.objtype >= UOBJ_BOLT_START and material.objtype <= UOBJ_BOLT_END)
		material := TurnBoltsIntoCloth (who, material);
	endif
	if (!material)
		return 0;
	endif


//	var ret_val := array{};
//	    ret_val.Append( material );
//	    ret_val.Append( material_objtype );
//	    ret_val.Append( itemConfig[material_objtype].Ptmod );
 
//	return ret_val;
	return material;
endfunction

function canUseMaterial( who, craft_skill, difficulty )

	if( GetAttribute( who, craft_skill ) >= CInt( difficulty ))
		return 1;
        endif

	return 0;
endfunction

function Craft_PlayEffects( who, elem, item:=0 )

	var craft_sound	:= elem.CraftSound,
	    craft_loops := CInt( elem.CraftLoops ),
	    craft_action := CInt( elem.CraftAnim );
     
	var craft_delay := CInt( elem.CraftDelay );
        if( !craft_delay  )
                craft_delay := 2;
        endif
 
	if( craft_loops )
                var i;
                for( i:=1; i<=craft_loops; i+=1 )
        		if( item )
                		TurnObjectToward( who, item.x, item.y );
        		endif
          
			if( craft_action )
				PerformAction( who, craft_action );
			endif
   
			PlaySoundEffect( who, craft_sound );
                        Sleep( craft_delay );
		endfor
	else
		PlaySoundEffect( who, craft_sound );
                Sleep( craft_delay );
	endif
 
	return 1;
endfunction

function Craft_ConsumeParts( who, parts_list )

	foreach material_objtype in ( parts_list.Keys() )
		var amount := parts_list[material_objtype];
		if( amount > 0 )
                        var consume := ConsumeSubstance( who.backpack, material_objtype, amount );
			if( consume.errortext )
				SendSysMessage( who, "You do not have the required materials to make that." );
				return 0;
			endif
		endif
  
		SleepMS(5);
	endforeach
 
	return 1;
endfunction

function Craft_ConsumeMaterial( who, elem, use_objtype )

	var amount := CInt( elem.Material );
	if( amount )
		var consume := ConsumeSubstance( who.backpack, use_objtype, amount );
		if( consume.errortext )
			SendSysMessage( who, "You do not have the required materials to make that." );
			return 0;
		endif
        else
                SendSysMessage( who, "This item does not require materials to be made! Please, contact a staff member." );
		return 0;
	endif

        return 1;
endfunction

///////////////////
//  Turns bolts of cloth into usable cloth
///////////////////

function TurnBoltsIntoCloth (character, bolts)
	PlaySoundEffect (character, SFX_TAILORING);
	sleep (3);
	var cloth := 0;
	if (bolts.amount > 100)
		cloth := CreateItemInBackpack (character, UOBJ_CLOTH, 5000);
		if (cloth)
			SubtractAmount (bolts, 100);
		else
			SendSysMessage (character, "Your backpack is full!");
			return 0;
		endif
	else
		var amount_of_cloth := bolts.amount * 50;
		cloth := CreateItemInbackPack (character, UOBJ_CLOTH, amount_of_cloth);
		if (cloth)
			DestroyItem (bolts);
		else
			SendSysMessage (character, "Your backpack is full!");
			return 0;
		endif
	endif
	return cloth;
endfunction

