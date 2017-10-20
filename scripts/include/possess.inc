use vitals;

include "include/client";
include "include/objtype";
include "include/utility";
include "include/attributes";

function possess (me, npc)
	if (GetObjProperty (me, "possess"))
		var oldnpc := SystemFindObjectBySerial(GetObjProperty (me, "possess"));
		if (oldnpc)
			unpossess (me);
		endif
	endif

	set_critical (1);

	if (npc.enable ("invul") )
		GrantPrivilege (me, "invul");
		me.enable ("invul");
	else
		RevokePrivilege (me, "invul");
	endif

	me.concealed := 1;
	var oldpack := MoveMyPack (me);

	SetObjProperty (me, "oldname", me.name);
	SetObjProperty (me, "oldprefix", me.title_prefix);
	SetObjProperty (me, "oldsuffix", me.title_suffix);
	me.name := npc.name;
	me.title_prefix := npc.title_prefix;
	me.title_suffix := npc.title_suffix;

	Clone (me, npc);
	EquipItem (npc, oldpack);
	SetObjProperty (me, "possess", npc.serial);

	foreach item in ListRootItemsInContainer (npc.backpack)
		if (item.newbie)
			MoveItemToContainer (item, me.backpack);
		endif
	endforeach

	set_critical (0);

endfunction




///////////////////
//  Moves all the character's equipped items to their backpack, and unequips their backpack
///////////////////

function MoveMyPack (character)
	if (!character.backpack)
		var newbackpack := CreateItemAtLocation (5123, 1779, 0, UOBJ_BACKPACK, 1, REALM_BRITANNIA);
		EquipItem (character, newbackpack);
	endif

	foreach item in listequippeditems (character)
		if (item != character.backpack )
			MoveItemToContainer (item, character.backpack);
		endif
	endforeach

	var oldbackpack := character.backpack;
	MoveObjectToLocation( oldbackpack, 5123, 1779, 0, _DEFAULT_REALM, MOVEOBJECT_FORCELOCATION );
	return oldbackpack;
endfunction




///////////////////
//  turns the seer into a duplicate of the NPC
///////////////////

function clone (character, npc)
	if (!npc.backpack)
		var newbackpack := CreateItemAtLocation (5123, 1779, 0, UOBJ_BACKPACK, 1, REALM_BRITANNIA);
		EquipItem (npc, newbackpack);
	endif

	if (npc.color == CINT (0x4631))
		npc.color := 1002;
	endif

	character.graphic := npc.graphic;
	character.color := npc.color;
	character.facing := npc.facing;
	character.hidden := npc.hidden;
	
	SetAttributeBaseValue (character, "Strength", GetAttributeBaseValue (npc, "Strength"));
	SetAttributeBaseValue (character, "Intelligence", GetAttributeBaseValue (npc, "Intelligence"));
	SetAttributeBaseValue (character, "Dexterity", GetAttributeBaseValue (npc, "Dexterity"));
	SetAttributeTemporaryMod (character, "Strength", GetAttributeTemporaryMod (npc, "Strength"));
	SetAttributeTemporaryMod (character, "Intelligence", GetAttributeTemporaryMod (npc, "Intelligence"));
	SetAttributeTemporaryMod (character, "Dexterity", GetAttributeTemporaryMod (npc, "Dexterity"));
	RecalcVitals (character);
	
	SetVital (character, "Life", GetVital (npc, "Life"));
	SetVital (character, "Mana", GetVital (npc, "Mana"));
	SetVital (character, "Stamina", GetVital (npc, "Stamina"));

	foreach it in ListEquippedItems (npc)
		MoveObjectToLocation( it, 5123, 1779, 0, _DEFAULT_REALM, MOVEOBJECT_FORCELOCATION );
		equipitem (character, it);
	endforeach

	for i := 0 to 48
		SetAttributeBaseValue (character, GetAttributeIDBySkillID (i), GetAttribute (npc, GetAttributeIDBySkillID (i))*10);
	endfor

	MoveObjectToLocation( character, npc.x, npc.y, npc.z, npc.realm, MOVEOBJECT_FORCELOCATION );
	MoveObjectToLocation( npc, 5276, 1164, 0, _DEFAULT_REALM, MOVEOBJECT_FORCELOCATION );

	character.concealed := 0;

	npc.hidden := 1;
	npc.frozen := 1;
endfunction




///////////////////
//  tries to unpossess the given NPC
///////////////////

function unpossess (character)

	set_critical (1);

	var npc;
	if (GetObjProperty (character, "possess") )
		npc := SystemFindObjectBySerial (GetObjProperty (character, "possess"));
	endif

	if (!npc)
		EraseObjProperty (character, "possess");
		if (GetObjProperty (character, "oldname") )
			character.name := GetObjProperty (character, "oldname");
			character.title_prefix := GetObjProperty (character, "oldprefix");
			character.title_suffix := GetObjProperty (character, "oldsuffix");
		endif
		SendSysMessage (character, "NPC not found!");
		return;
	endif	

	character.concealed := 1;
	SwapClothes (character, npc);

	character.name := GetObjProperty (character, "oldname");
	character.title_prefix := GetObjProperty (character, "oldprefix");
	character.title_suffix := GetObjProperty (character, "oldsuffix");

	EraseObjProperty (character, "oldname");
	EraseObjProperty (character, "oldprefix");
	EraseObjProperty (character, "oldsuffix");
	EraseObjProperty (character, "possess");

	npc.hidden := character.hidden;
	npc.facing := character.facing;
	npc.frozen := 0;	
	SetVital (npc, "Life", GetVital (character, "Life"));
	SetVital (npc, "Mana", GetVital (character, "Mana"));
	SetVital (npc, "Stamina", GetVital (character, "Stamina"));	
	MoveObjectToLocation( npc, character.x, character.y, character.z, character.realm, MOVEOBJECT_FORCELOCATION );

	for i := 0 to 48
		SetAttributeBaseValue (character, GetAttributeIDBySkillID (i), 1000);
	endfor

	SetAttributeBaseValue (character, "Strength", 1000);
	SetAttributeBaseValue (character, "Intelligence", 1000);
	SetAttributeBaseValue (character, "Dexterity", 1000);
	SetAttributeTemporaryMod (character, "Strength", 0);
	SetAttributeTemporaryMod (character, "Intelligence", 0);
	SetAttributeTemporaryMod (character, "Dexterity", 0);
	RecalcVitals (character);
	
	SetVital (character, "Life", GetVitalMaximumValue (character, "Life"));
	SetVital (character, "Mana", GetVitalMaximumValue (character, "Mana"));
	SetVital (character, "Stamina", GetVitalMaximumValue (character, "Stamina"));

	character.graphic := character.trueobjtype;
	character.color := character.truecolor;

	foreach thing in ListRootItemsInContainer (character.backpack)
		EquipItem (character, thing);
	endforeach

	foreach item in ListRootItemsInContainer (npc.backpack)
		if (item.newbie)
			MoveItemToContainer (item, character.backpack);
		endif
	endforeach

	set_critical (0);

endfunction

function SwapClothes (character, npc)
	if (!character.backpack)
		var newbackpack := CreateItemAtLocation (5123, 1779, 0, UOBJ_BACKPACK, 1, REALM_BRITANNIA);
		EquipItem (character, newbackpack);
	endif

	var hispack := npc.backpack;
	if (hispack)
		MoveObjectToLocation( hispack, 5123, 1779, 0, REALM_BRITANNIA, MOVEOBJECT_FORCELOCATION );
	else
		hispack := CreateItemAtLocation (5123, 1779, 0, UOBJ_BACKPACK, 1, REALM_BRITANNIA);
	endif

	foreach item in ListEquippedItems (character)
		MoveObjectToLocation( item, 5123, 1779, 0, REALM_BRITANNIA, MOVEOBJECT_FORCELOCATION );
		EquipItem (npc, item);
	endforeach

	EquipItem (character, hispack);
	foreach item in ListRootItemsInContainer (npc.backpack)
		if (item.newbie)
			MoveItemToContainer (item, character.backpack);
		endif
	endforeach
endfunction




///////////////////
//  called when the seer dies while possessing an NPC
///////////////////

function UnpossessCorpse (character, corpse )

	corpse := "not used now";

	var npc;
	if ( getobjproperty (character, "possess") )
		npc := SystemFindObjectBySerial(getobjproperty (character,"possess"));
	endif
	if (!npc)
		eraseobjproperty (character, "possess");
		if ( getobjproperty (character, "oldname") )
			character.name := getobjproperty (character, "oldname");
			character.title_prefix := getobjproperty (character, "oldprefix");
			character.title_suffix := getobjproperty (character, "oldsuffix");
		endif
		return;
	endif	

	character.concealed := 1;

	Resurrect (character);
	character.graphic := character.trueobjtype;
	character.color := character.truecolor;

	SwapClothes (character, npc);

	eraseobjproperty (character, "possess");

	character.name := getobjproperty (character, "oldname");
	character.title_prefix := getobjproperty (character, "oldprefix");
	character.title_suffix := getobjproperty (character, "oldsuffix");
	eraseobjproperty (character, "oldname");
	eraseobjproperty (character, "oldprefix");
	eraseobjproperty (character, "oldsuffix");

	MoveObjectToLocation( npc, 5288, 1176, 0, REALM_BRITANNIA, MOVEOBJECT_FORCELOCATION );
	setobjproperty (npc, "guardkill", 1);
	RevokePrivilege (npc, "invul");
	DoDamageByType (0, npc, CINT (GetVital (npc, "Life")/100) + 100, DAMAGETYPE_PHYSICAL);

	for i := 0 to 48
		SetAttributeBaseValue (character, GetAttributeIDBySkillID (i), 1000);
	endfor

	SetAttributeBaseValue (character, "Strength", 1000);
	SetAttributeBaseValue (character, "Intelligence", 1000);
	SetAttributeBaseValue (character, "Dexterity", 1000);
	SetAttributeTemporaryMod (character, "Strength", 0);
	SetAttributeTemporaryMod (character, "Intelligence", 0);
	SetAttributeTemporaryMod (character, "Dexterity", 0);
	RecalcVitals (character);
	
	SetVital (character, "Life", GetVitalMaximumValue (character, "Life"));
	SetVital (character, "Mana", GetVitalMaximumValue (character, "Mana"));
	SetVital (character, "Stamina", GetVitalMaximumValue (character, "Stamina"));

	foreach thing in ListRootItemsInContainer (character.backpack)
		EquipItem (character, thing);
	endforeach

	var weaponone := GetEquipmentByLayer (character, LAYER_HAND1);
	var weapontwo := GetEquipmentByLayer (character, LAYER_HAND2);
	if (weaponone)
		MoveItemToContainer (weaponone, character.backpack);
	endif
	if (weapontwo)
		MoveItemToContainer (weapontwo, character.backpack);
	endif

endfunction
