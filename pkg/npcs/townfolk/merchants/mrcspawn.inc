///////////////////
//  restocks all items in the vendors inventory
///////////////////

function RestockInventory (merchanttype, byref inventory)
	UnloadConfigFile (":merchants:mrcspawn");
	var mrcspawn_cfg := ReadConfigFile(":merchants:mrcspawn");

	foreach item in ListRootItemsInContainer (inventory)
		DestroyItem (item);
	endforeach

	foreach productgroup in ProductGroups (mrcspawn_cfg, merchanttype)
		Restock (mrcspawn_cfg, inventory, productgroup);
	endforeach

	foreach item in ListRootItemsInContainer ( inventory )
		if ( item.quality )
			item.quality := 1;
			item.hp := item.maxhp;
		endif
	endforeach
endfunction




///////////////////
//  determines what product groups the merchant sells
///////////////////

function ProductGroups (byref mrcspawn_cfg, mtype)
	var mtype_elem := FindConfigElem ( mrcspawn_cfg, mtype );
	if (!mtype_elem)
		return 0;
	endif

	return GetConfigStringArray (mtype_elem, "Sells");
endfunction




///////////////////
//  restocks the given group of items in the merchant's inventory, if those items need
//  to be restocked
///////////////////

function Restock (byref mrcspawn_cfg, byref inventory, byref productgroup)

	var pg_elem := FindConfigElem (mrcspawn_cfg, productgroup);
	foreach itemstr in GetConfigStringArray (pg_elem, "Item")
		sleepms(100);
		SpawnItem (inventory, itemstr);
	endforeach

	foreach itemstr in GetConfigStringArray( pg_elem, "Multibag" )
		sleepms(100);
		var baglist := splitwords(itemstr);
		var spawnelem := FindConfigElem(mrcspawn_cfg, baglist[1] );
		var desc := spawnelem.desc;
		var stuffarray := GetConfigStringArray( spawnelem, "Item" );
		SpawnMultiBag( inventory, desc, stuffarray, cint(baglist[2]) );
	endforeach

endfunction




///////////////////
//  takes the string passed and creates the item specified in it
///////////////////

function SpawnItem (byref inventory, itemstr)

	var ii := SplitWords( itemstr );
	var objtype := ii[1];
	if (cint(objtype))
		objtype := cint(objtype);
	else
		objtype := GetObjtypeByName (objtype);
	endif

	var count := CInt(ii[2]);
	CreateItemInInventory (inventory, objtype, count);

endfunction




///////////////////
//  creates a bag with several different items in it (used for bags of regs)
///////////////////

function SpawnMultiBag (byref inventory, desc, stuffarray, number)

	var itemdesc := ReadConfigFile (":*:itemdesc");
	if (!itemdesc)
		return;
	endif

	for i := 1 to number
		var regbag := CreateItemInContainer (inventory,	UOBJ_BAG, 1);
		if (!regbag)
			return;
		endif

		regbag.name := desc;
		regbag.sellprice := 10;

		foreach item in stuffarray
			var parms := splitwords(item);

			var elem := FindConfigElem(itemdesc,cint(parms[1]));
			if (!elem)
				return;
			endif
			var itemprice := GetConfigInt(elem,"vendorsellsfor");
			if (!itemprice)
				return;
			endif

			var it := CreateItemInContainer(regbag,cint(parms[1]),cint(parms[2]));
			if (!it)
				return;
			endif
			regbag.sellprice := regbag.sellprice + ( itemprice * it.amount );
		endforeach
	endfor
endfunction
