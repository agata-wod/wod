use cfgfile;
use os;
use util;

include "../pkg/npcs/npcinfo";

//
//  See starteqp.cfg for a description of what we expect in the config file.
//

var unique_items_created := array{};
var starteqp_cfg;
var template;
var magicspawnchance := 0;
var making_starting_equipment := 0;

function MakeLoot( corpse , lootgroup )

	var elem := GetNpcTemplateElem( GetObjProperty( corpse, "npctemplate" ) );

	var mprop;
	if (elem)
		mprop := GetConfigInt( elem , "Magicitemchance" );
	endif

	if( mprop )
		magicspawnchance := mprop;
	else
		magicspawnchance := 0;
	endif

	if ( (lootgroup >= 5) and (lootgroup <= 9) )
		magicspawnchance := randomint(5+lootgroup)+1;
		magicspawnchance := cint(magicspawnchance/2);
	endif

	if ( (lootgroup >= 109) and (lootgroup <= 119) )
		magicspawnchance := randomint(1+(lootgroup-110))+1;
		magicspawnchance := cint(magicspawnchance/2);
	endif

	if ( lootgroup == 122 )
		magicspawnchance := 5;
	endif

	var lootgroups;
	if (elem)
		lootgroups := GetConfigStringArray( elem, "lootgroup" );
	endif
	starteqp_cfg := ReadConfigFile( "::nlootgroup" );

	if( lootgroup )
        	CreateFromGroup( corpse , lootgroup );
	else
		foreach str in lootgroups
			CreateFromGroup( corpse , cint(str) );
		endforeach
	endif

endfunction


function CreateStartingEquipment( who, skillids )
	making_starting_equipment := 1;
	starteqp_cfg := ReadConfigFile( "::starteqp" );
	foreach skillid in skillids
		var skillname := GetSkillName( CInt(skillid) );
		CreateFromGroup (who.backpack, skillname);
	endforeach
	CreateFromGroup(who.backpack, "all");
endfunction


function CreateFromGroup ( who, group )

    var elem := starteqp_cfg[ group ];
    if (!elem)
	return;
    endif

    foreach str in GetConfigStringArray( elem, "Stack" )
        CreateFromStackString( who, str );
    endforeach

    foreach str in GetConfigStringArray( elem, "Item" )
        CreateFromItemString( who, str );
    endforeach

    foreach str in GetConfigStringArray( elem, "Random" )
        CreateFromRandomString( who, str );
    endforeach

    foreach str in GetConfigStringArray( elem, "Group" )
        CreateFromGroup( who, str );
    endforeach

    foreach str in GetConfigStringArray( elem, "Unique" )
        CreateFromUniqueString( who, str );
    endforeach
endfunction

function GetSkillName ( skillid )
    var cfg := ReadConfigFile( "::skills" );
    return cfg[ skillid ].name;
endfunction


/////////////////////////////////////////////////////////////////////////////
//
//  An 'Item' string can have one of the following forms:
//
//  FORM                                    EXAMPLE
//  --------------------------------------------------------------------
//  "Item [ObjTypeName]"                    "Item Mortar"
//  "Item [Count] [ObjTypeName]"            "Item 4 EmptyBottle"
//  "Item [ObjTypeName] [Colornum]"         "Item Robe 0xE9"
//  "Item [Count] [ObjTypeName] [Colornum]"  "Item 2 Robe 0xE9"
//  "item [ObjTypeName] [Colornum] [chancetospawn]  "item Longsword 0 100"
//  "Item [Count] [ObjTypeName] [Colornum] [chancetospawn]  "item 1 Longsword 0 100"
//
//Item 1 level1map 0 1
//
/////////////////////////////////////////////////////////////////////////////

function CreateFromItemString( who, str )
    var params := SplitWords( str );
    var count;
    var objname;
    var color;
    var chance;
    if ( RandomDiceRoll( params[1] ) )
        count := RandomDiceRoll( params[1] );
        params[1] := params[2];
        params[2] := params[3];
        params[3] := params[4];
    else
        count := 1;
    endif
    objname := params[1];

    if ( CInt(params[2]) )
        color := CInt( params[2] );
    else
        color := 0;
    endif

    if ( CInt(params[3]) )
        chance := CInt( params[3] );
    else
        chance := 100;
    endif

    //syslog(   "count=" + count + ", objname=" + objname + ", chance=" + chance );

  if( cint(RandomInt(100)+1) <= cint(chance) )
	var i;
//	var canbemagic;
	for( i := 1; i <= count; i := i + 1 )
		var item := CreateItemInContainer( who, objname );
		if (color)
			item.color := color;
		endif
		if(making_starting_equipment)
			item.newbie := 1;
		endif

		if( RandomInt(100)+1 <= magicspawnchance )
			MakeMagic( objname, item );
		endif
    endfor
  endif

endfunction

/////////////////////////////////////////////////////////////////////////////
//
//  A 'Random' string follows the following form:
//
//  FORM                                    EXAMPLE
//  --------------------------------------------------------------------
//  "Random [Count] [GroupName]"            "Random 4 Reagents"
//  "Random [Count] [GroupName] [chancetospawn]"   "Random 4 Reagents 50"
//
//  'Count' entries from 'GroupName' are chosen randomly.  The same entry
//  will not be chosen twice.
//
/////////////////////////////////////////////////////////////////////////////


function CreateFromRandomString( who, desc )

    var params := SplitWords( desc );
    var count := RandomDiceRoll( params[1] );
    var group := params[2];
    var chance := params[3];
    //syslog( "count=" + count + ", group=" + group + ", chance=" + chance);
    
    var elem := FindConfigElem( starteqp_cfg , group);
    if (elem == error) 
//        syslog( "Unable to find group " + group );
        return;
    endif
    
    var items := GetConfigStringArray( elem, "Item" );
    if (items == error) 
//        syslog( "Group " + group + " does not contain items!" );
        return; 
    endif

    if ( !chance )
        chance := 100;
    endif

    // print( "count=" + count + ", objname=" + objname + ", color=" + color );
    var rand := RandomInt(100)+1;

    if( cint(rand) <= cint(chance) )

    if( count > len(items))
        count := len(items);
    endif

        while (count)
            var n := RandomInt( len(items) ) + 1;
            if (items[n])
                CreateFromItemString( who, items[n] );

                if(making_starting_equipment)
                   items[n] := "";
                endif

                count := count - 1;
            endif
        endwhile
    endif
endfunction


/////////////////////////////////////////////////////////////////////////////
//
//  A 'Unique' string has the following form:
//
//  FORM                                    EXAMPLE
//  --------------------------------------------------------------------
//  "Unique [ObjTypeName]"                  "Unique Spellbook"
//
//  Only one of a unique item will be created as starting equipment.
//
/////////////////////////////////////////////////////////////////////////////

function CreateFromUniqueString( who, str )
    var objtype := GetObjtypeByName( str );
    if (!objtype)
//        syslog( "Don't know what a " + str + " is" );
        return;
    endif
        
    if (objtype in unique_items_created)
        return;
    endif

    unique_items_created[ len(unique_items_created) + 1 ] := objtype;
    var uitem := CreateItemInContainer( who, objtype );
    if(making_starting_equipment)
           uitem.newbie := 1;
    endif
endfunction


/////////////////////////////////////////////////////////////////////////////
//
//  A 'Stack' string is just like "Item" but stacks the item instead of making
//  count number of individual items.
//
/////////////////////////////////////////////////////////////////////////////

function CreateFromStackString( who, str )
    var params := SplitWords( str );
    var count;
    var objname;
    var color;
    var chance;
    if ( RandomDiceRoll( params[1] ) )
        count := RandomDiceRoll( params[1] );
        params[1] := params[2];
        params[2] := params[3];
        params[3] := params[4];
    else
        count := 1;
    endif
    objname := params[1];
    if ( CInt(params[2]) )
        color := CInt( params[2] );
    else
        color := 0;
    endif

    if ( CInt(params[3]) )
        chance := CInt( params[3] );
    else
        chance := 100;
    endif

    // print( "count=" + count + ", objname=" + objname + ", color=" + color );
    var rand := RandomInt(100)+1;

  if( cint(rand) <= cint(chance) )

        var item := CreateItemInContainer( who, objname, count );
        if (!item)
//            syslog( "Unable to create " + objname );
        endif
        // print( "Created: " + item.desc );
        item.color := color;
        if(making_starting_equipment)
           item.newbie := 1;
        endif
  endif

endfunction

function MagicalEquip(me)

	var npcdesc := ReadConfigFile( "::npcdesc" );
	var elem := FindConfigElem( npcdesc, me.npctemplate );
	if (elem)
		magicspawnchance := GetConfigInt( elem , "Magicitemchance" );
	else
		return;
	endif

	if(!magicspawnchance)
		return;
	endif

	var itemdesc := ReadConfigFile("::itemdesc");
	var objname;

	var canbemagic := 0;
	foreach item in ListEquippedItems(me)
		objname := itemdesc[item.objtype].name;
		canbemagic := 0;
		if( RandomInt(100)+1 <= magicspawnchance )
			MakeMagic( objname, item );
		endif
	endforeach

endfunction

function MakeMagic( objname, item )

	var parms := array { };
	parms[1] := objname;
	parms[2] := item;

	start_script(":magic:createmagicitem", parms);

endfunction
