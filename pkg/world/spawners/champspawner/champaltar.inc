/* Misc Champion Altar functions
 *
 * Functions in the Inc:
 * get_spawnarray(altar)		returns an npc template to be spawned
 * get_subLevel(altar)			returns the current sub-level
 * activateAltar(altar)			activates the altar
 * deactivateAltar(altar)		deactivates the altar
 * getRedSkullLocation(altar, index)	gets the index'th red skull location
 * getWhiteSkullLocation(altar, index)  gets the index'th white skull location
 * createNextWhiteSkull(altar)		creates the next white skull
 * createNextRedSkull(altar)		creates the next red skull
 * destroyLastRedSkull(altar)		destroys the last red skull
 * wipeItemsArray(Items)		destroy all items from an Items serial array
 * getSpawnLocation(altar)		gets an random spawn location 20 tiles around the altar
 *
 */


function get_spawn(altar)
	var cfg := ReadConfigFile(":champspawner:spawns");

	if ( cfg.errortext )
		PrintTextAbove(altar, "Couldnt open spawns.cfg");
		return error;
	endif
	cfg := cfg[GetObjProperty(altar, "spawn")];
	if ( !cfg )
		PrintTextAbove(altar, "Couldnt get the cfg elem ["+GetObjProperty(altar, "spawn")+"]");
		return error;
	endif

	var spawn_name := "level"+get_subLevel(altar);

	var spawns := GetConfigStringArray(cfg, spawn_name);
	if ( !spawns )
		PrintTextAbove(altar, "Could not get ConfigStringArray(cfg, \""+spawn_name+"\"");
		return error;
	endif

	return (spawns[RandomInt(spawns.size())+1]);

endfunction

function get_champ(altar)
	var cfg := ReadConfigFile("spawns");

	if ( cfg.errortext )
		PrintTextAbove(altar, "Couldnt open spawns.cfg");
		return error;
	endif

	cfg := cfg[GetObjProperty(altar, "spawn")];
	if ( !cfg )
		PrintTextAbove(altar, "Couldnt get the cfg elem ["+GetObjProperty(altar, "spawn")+"]");
		return error;
	endif

	var spawn_name := "champion";

	var spawns := GetConfigStringArray(cfg, spawn_name);
	if ( !spawns )
		PrintTextAbove(altar, "Could not get ConfigStringArray(cfg, \""+spawn_name+"\"");
		return error;
	endif

	return (spawns[1]);

endfunction

function spawnChampion(altar)
	var loc:= getSpawnLocation(altar);
	var creature:=CreateNPCFromTemplate(get_champ(altar), loc[1], loc[2], loc[3], 0, altar.realm);
	if (creature)
		SetObjProperty(altar, "champ", creature.serial);
		var pentagram := GetObjProperty(altar, "pentagram");
		foreach piece in pentagram
			piece:=SystemFindObjectBySerial(piece);
			piece.invisible:=0;
			piece.color:=38;
		endforeach
	endif
endfunction

function get_subLevel(altar)
	var redSkulls := CInt(GetObjProperty(altar, "lrskull"));
	
	if(redSkulls<6)
		return 0;
	elseif(redSkulls<10)
		return 1;
	elseif(redSkulls<14)
		return 2;
	else
		return 3;
	endif
endfunction

function get_maxMonsters(altar)
	return (250-get_subLevel(altar)*12);
//	return (75-get_subLevel(altar)*12);
endfunction

function activateAltar(altar)
	var pentagram := GetObjProperty(altar, "pentagram");
	foreach piece in pentagram
		piece:=SystemFindObjectBySerial(piece);
		piece.invisible:=0;
		piece.color:=0;
	endforeach
	SetObjProperty(altar, "active", 1);
endfunction

function deactivateAltar(altar)
	var pentagram := GetObjProperty(altar, "pentagram");
	foreach piece in pentagram
		piece:=SystemFindObjectBySerial(piece);
		piece.color:=980;
//		piece.invisible:=1;
	endforeach
	SetObjProperty(Altar, "NextActiv", ReadGameClock()+ACTIVE_EACH);
	EraseObjProperty(altar, "#completed");
	EraseObjProperty(altar, "active");
	EraseObjProperty(altar, "champ");
endfunction

function getRedSkullLocation(altar, index)
	var x, y;
	if(index<5)
		x := index - 2;
		y := -2;
	elseif(index<9)
		x := 2;
		y := index-6;
	elseif(index<13)
		x := 10-index;
		y := 2;
	else
		x := -2;
		y := 14-index;
	endif

	return array {altar.x+x, altar.y+y, altar.z};
endfunction

function getWhiteSkullLocation(altar, index)
	var x:=0, y:=0;
	case ( index )
		default:
		0: x := -1; y := -1; break;
		1: x :=  1; y := -1; break;
		2: x :=  1; y :=  1; break;
		3: x := -1; y :=  1; break;
	endcase
	return array {altar.x+x, altar.y+y, altar.z};
endfunction

function createNextWhiteSkull(altar)
	var lwskull := CInt(GetObjProperty(altar, "lwskull"));
	var wskulls:=array;
	wskulls:=GetObjProperty(altar, "wskulls");
	if(lwskull==4)
		WipeItemsArray(wskulls);
		wskulls:= array {};
		SetObjProperty(altar, "wskulls", wskulls);
		EraseObjProperty(altar, "lwskull");
		CreateNextRedSkull(altar);
		return;
	endif
	var location := GetWhiteSkullLocation(altar, lwskull+1);
	var wskull := CreateItemAtLocation(location[1], location[2], location[3], 0x1854, 1, altar.realm);

	wskull.movable := 0;

	if(!wskulls)
		wskulls:= array {wskull.serial};
	else
		wskulls.append(wskull.serial);
	endif
    
        PlaySoundEffect(wskull, 0x29);
	PlayStationaryEffect( wskull.x,wskull.y,wskull.z, 0x3728, 7, 0x10,0,wskull.realm );
	//PlayStationaryEffect( x, y, z, effect, speed, loop := 0, explode := 0, realm := _DEFAULT_REALM );

	SetObjProperty(altar, "lwskull", lwskull+1);
	SetObjProperty(altar, "wskulls", wskulls);

	return;
endfunction

function createNextRedSkull(altar)
	var lskull := CInt(GetObjProperty(altar, "lrskull"));
	var skulls:=array;
	skulls:=GetObjProperty(altar, "rskulls");

	var wskulls:=array;
	wskulls:=GetObjProperty(altar, "wskulls");
	WipeItemsArray(wskulls);
	wskulls:= array {};
	SetObjProperty(altar, "wskulls", wskulls);
	EraseObjProperty(altar, "lwskull");

	if(lskull==16)
		WipeItemsArray(skulls);
		skulls:= {};
		SetObjProperty(altar, "rskulls", skulls);
		SetObjProperty(altar, "#completed", 1);
		EraseObjProperty(altar, "lrskull");
		return;
	endif
	var location := GetRedSkullLocation(altar, lskull+1);
	var skull := CreateItemAtLocation(location[1], location[2], location[3], 0x1854, 1, altar.realm);
	
	skull.color:=0x26;
	skull.movable := 0;

	if(!skulls)
		skulls:= array {skull.serial};
	else
		skulls.append(skull.serial);
	endif
        PlaySoundEffect(skull, 0x29);
	PlayStationaryEffect( skull.x,skull.y,skull.z, 0x3728, 7, 0x10,0,skull.realm );

	SetObjProperty(altar, "lrskull", lskull+1);
	SetObjProperty(altar, "rskulls", skulls);

	return;
endfunction

function destroyLastRedSkull(altar)
	var lskull := CInt(GetObjProperty(altar, "lrskull"));
	var skulls:=array;
	skulls:=GetObjProperty(altar, "rskulls");
	if(!lskull)
		return;
	endif
	var skull := skulls[lskull];
	DestroyItem(SystemFindObjectBySerial(skull));
	skulls.erase(lskull);

	SetObjProperty(altar, "lrskull", lskull-1);
	SetObjProperty(altar, "rskulls", skulls);

	return;
endfunction

function wipeItemsArray(Items)
	foreach item in Items
		item:=SystemFindObjectBySerial(item);
		if(!DestroyItem(item))
			DestroyItem(item);
		endif
	endforeach

	return;
endfunction

function getSpawnLocation(altar)
	var x := altar.x + ((RandomInt(40)+1)-20);
	var y := altar.y + ((RandomInt(40)+1)-20);
        var z := GetMapInfo(x,y, altar.realm).z;

	return array {x, y, z};
endfunction