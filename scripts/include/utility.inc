use os;
use uo;
use util;
use cfgfile;
use storage;

const SYSFIND_SEARCH_STORAGE_AREAS := 2;

include "include/objtype";
include "../pkg/items/housing/house";

function find_or_create_storage ( areaname )

	var area := FindStorageArea ( areaname );
	if (!area)
		area := CreateStorageArea ( areaname );
		if (area)
			print ( "Storage Area '" + areaname + "' created." );
		endif
	endif
	return area;

endfunction

function find_or_create_item ( storage, name, objtype )

	var item := FindRootItemInStorageArea ( storage, name );
	if (item)
		return item;
	endif

	//didn't find, so create
	item := CreateRootItemInStorageArea ( storage, name, objtype );
	return item;

endfunction

function IsLocationAccessible ( character, x, y, z )
    var xd, yd, zd;
    
    xd := character.x - x;
    if (xd < -1 or xd > 1) 
        return 0;
    endif
    
    yd := character.y - y;
    if (yd < -1 or yd > 1) 
        return 0;
    endif
    
    zd := character.z - z;
    if (zd < -10 or zd > 10)
        return 0;
    endif

    return CheckLosAt( character, x, y, z );

endfunction

function coordist ( x1 , y1 , x2 , y2 )

	var xd := x1 - x2;
	var yd := y1 - y2;
	if (xd < 0)
		xd := -xd;
	endif
	if (yd < 0)
		yd := -yd;
	endif
	if (xd > yd)
		return xd;
	else
		return yd;
	endif

endfunction

function AllocLockId()

	set_critical (1);
	var lockid := GetGlobalProperty( "nextlockid" );
	if (!lockid)
		lockid := 1;
	endif
	SetGlobalProperty ( "nextlockid", lockid+1 );

	set_critical (0);

	return lockid;
endfunction






///////////////////
//  opens the bank storage area
///////////////////

function OpenWorldBank()
	var bank := FindStorageArea ("World Bank");
	if (!bank)
		bank := CreateStorageArea ("World Bank");
	endif
	return bank;
endfunction




///////////////////
// finds the bankbox for the given character
///////////////////

function FindBankBox (acctname, nocreate := 0)

	var worldbank := OpenWorldBank();
	var bank_obj_name := CSTR ("Bankbox of " + acctname);
	var bankbox := FindRootItemInStorageArea (worldbank, bank_obj_name);
	if (!bankbox and !nocreate)
		bankbox := CreateRootItemInStorageArea( worldbank, bank_obj_name, UOBJ_BANKBOX );
	endif
	return bankbox;
endfunction

///////////////////
//returns an array of all the items in container of the given objtype
///////////////////

function EnumerateItemsInContainerOfObjtype (byref container, objtype)
	var ret := array{ };
	foreach item in enumerateItemsIncontainer(container)
		if (item.objtype == objtype)
			ret.append(item);
		endif
	endforeach
	return ret;
endfunction




///////////////////
//returns the first item found in the container of the given objtype
///////////////////

function FindItemInContainerOfObjtype (byref container, objtype)
	foreach item in enumerateitemsincontainer(container)
		if (item.objtype == objtype)
			return item;
		endif
	endforeach
	return 0;
endfunction




///////////////////
//returns all the 'top-level' items in container
///////////////////

function ListRootItemsInContainer (byref container)
	var ret := array { };
        foreach item in enumerateitemsincontainer(container)
		if ( item.container == container )
			ret.append (item);
		endif
	endforeach

	return ret;
endfunction




///////////////////
//returns all the 'top-level' items in container of objtype
///////////////////

function ListRootItemsInContainerOfObjtype (byref container, objtype)
	var ret := array{ };
	foreach item in enumerateitemsincontainer(container)
		if ( (item.container == container) and (item.objtype == objtype) )
			ret.append(item);
		endif
	endforeach

	return ret;
endfunction




///////////////////
//  Figures out if the given item is in the given container
//////////////////

function InInventory (byref inventory, byref it)
	foreach item in EnumerateItemsInContainer(inventory)
		if (item == it)
			return 1;
		endif
	endforeach

	return 0;
endfunction




///////////////////
//  Figures out if the given item is a container (some items that technically
//  are containers don't count as such in most cases)
///////////////////

function IsContainer (byref it)
	case (it.objtype)
		UOBJ_SPELLBOOK:
		UOBJ_RUNEBOOK:
		UOBJ_TRASHBARREL:
		UOBJ_POTION_KEG:
		0x170b0:				//checker board
		0x170b1:				//chess board
		0x170b2:				//backgammon board
			return 0;
	endcase

	if (it.isa (POLCLASS_CONTAINER) and !it.isa (POLCLASS_SPELLBOOK) )
		return 1;
	endif
	return 0;
endfunction





///////////////////
//  Players won't leave behind a clone if this returns a 1
///////////////////

function CheckForNoLogoutArea (character)
	foreach item in ListObjectsInBox (character.x-32, character.y-32, -128, character.x+32, character.y+32, +127, character.realm)
		case (item.objtype)
			0x17060:
			0x0bd2:
			0x0bd0:
				if (!IsAFriend (item, character))
					if (IsInsideTheHouse (character, item) )
						return 1;
					endif
				endif
		endcase
	endforeach

	var logoffcfgfile := ReadConfigFile (":guards:logoff");
	if (!logoffcfgfile)
		Syslog ("Error reading :guards:logout config file!");
		return 0;
	endif

	foreach key in GetConfigStringKeys (logoffcfgfile)
		var elem := FindConfigElem (logoffcfgfile, key);
		if (elem)
			if (character.x > elem.MinX)
				if (character.x < elem.MaxX)
					if (character.y > elem.MinY)
						if (character.y < elem.MaxY)
							return 1;
						endif
					endif
				endif
			endif
		endif
	endforeach

	return 0;
endfunction




///////////////////
//  Teleports henchman or pet left in the no-logout areas to the specified location (an inn?)
///////////////////

function TeleportFromNoLogoutArea (byref character)
	var logoffcfgfile := ReadConfigFile (":guards:logoff");
	if (!logoffcfgfile)
		return 0;
	endif

	foreach key in GetConfigStringKeys (logoffcfgfile)
		var elem := FindConfigElem (logoffcfgfile, key);
		var elemrealm := 0;
		if (elem)
			elemrealm := elem.Realm;
			if (!elemrealm)
				elemrealm := REALM_BRITANNIA;
			endif
			if (character.realm == elemrealm)
				if (character.x > elem.MinX)
					if (character.x < elem.MaxX)
						if (character.y > elem.MinY)
							if (character.y < elem.MaxY)
								MoveObjectToLocation( character,elem.TeletoX,elem.TeletoY, elem.TeletoZ, elemrealm, MOVEOBJECT_FORCELOCATION );
								return 1;
							endif
						endif
					endif
				endif
			endif
		endif
	endforeach

	return 0;
endfunction




function OpenBrowser ( who, url )
/*
0xA5 Packet
Open Web Browser (Variable # of bytes)
* BYTE cmd
* BYTE[2] blockSize
* BYTE[blockSize-3] null terminated full web address
*/

	var packet:="A5"+WordPacket(Len(url)+4);
	ForEach ascii_code in CAscZ(url)
		packet:=packet+BytePacket(ascii_code);
	EndForEach
	SendPacket (who, packet+"00");
endfunction
///////////////////////////////////////////////////
// BytePacket - Liefert ein Byte im Packet-Format
///////////////////////////////////////////////////

Function BytePacket(byte)
  var paket:=Hex(byte);
  paket:="00"+paket[3, Len(paket)-2];
  Return (paket[Len(paket)-1, 2]);
EndFunction

///////////////////////////////////////////////////
// WordPacket - Liefert ein Word im Packet-Format
///////////////////////////////////////////////////

Function WordPacket(word)
  var paket:=Hex(word);
  paket:="0000"+paket[3, Len(paket)-2];
  Return (paket[Len(paket)-3, 4]);
EndFunction

///////////////////////////////////////////////////////////////
// DoubleWordPacket - Liefert ein DoubleWord im Packet-Format
///////////////////////////////////////////////////////////////

Function DoubleWordPacket(doubleword)
  var paket:=Hex(doubleword);
  paket:="00000000"+paket[3, Len(paket)-2];
  Return (paket[Len(paket)-7, 8]);
EndFunction





