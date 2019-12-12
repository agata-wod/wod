include "include/client";
include "include/objtype";
include "include/effects";
include "include/wodinc";
include "include/poison";
include "include/dotempmods";


///////////////////
//  generates a random name for the NPC based on its graphic
///////////////////

function RandomName (byref me, localized := 0)

	//figure out where in the names cfg file this sort of creature's name type would be located
	var name_index := 0;
	case ( me.graphic )
		CID_BIRD:				name_index := 1;
		0x2b:
		CID_DAEMON:
		CID_DAEMON_SWORD:		name_index := 2;
		CID_DRAGON_RED:
		CID_DRAGON_GREY:
		CID_DRAGON_RUST:
		CID_DRAGON_FIRE:
		CID_DRAGON_CRIMSON:
		CID_DRAGON_BLACK: name_index := 3;
		CID_HUMAN_MALE: 		name_index := 5;
                                  if (localized)
					case (DetermineLocal(me))
						1: name_index := 13;
					endcase
				endif
		CID_HUMAN_FEMALE:		name_index := 6;
                                  if (localized)
					case (DetermineLocal(me))
						1: name_index := 14;
					endcase
				endif
		CID_LIZARDMAN:			name_index := 9;
		CID_LIZARDMAN_W_MACE:	name_index := 9;
		CID_LIZARDMAN_W_SPEAR:	name_index := 9;
		CID_ORC:				name_index := 10;
		CID_ORC_W_CLUB:		name_index := 10;
		CID_ORC_LORD:			name_index := 10;
		CID_OGRE:				name_index := 11;
		CID_RATMAN:			name_index := 12;
		CID_RATMAN_W_SWORD:		name_index := 12;
		CID_RATMAN_W_AXE:		name_index := 12;
		CID_ELF_MALE:   name_index := 15;
		CID_ELF_FEMALE:   name_index := 16;
		CID_GARGOYLE_MALE:   name_index := 17;
		CID_GARGOYLE_FEMALE:   name_index := 18;
		default:				return;
	endcase

	var names_cfg_file := ReadConfigFile ( ":npcs:names" );
	var elem := FindConfigElem (names_cfg_file, name_index);
	var number_of_names := GetConfigInt (elem, "Count");

	var dice_string := CStr ( "1d" + CStr(number_of_names) );
	var name_position := RandomDiceRoll (dice_string);
	var newname := GetConfigString (elem, "Name"+name_position);

	return (newname);
endfunction




///////////////////
//  Returns a random NPC name without requiring an actual NPC
///////////////////

function RandomNPCName (name_index)
	//if no index, randomly pick male or female
	if (!name_index)
		name_index := RandomInt (2) + 5;
	endif

	var names_cfg_file := ReadConfigFile ( ":npcs:names" );
	var elem := FindConfigElem (names_cfg_file, name_index);
	var number_of_names := GetConfigInt (elem, "Count");

	var name_position := RandomInt (number_of_names) + 1;
	var newname := GetConfigString (elem, "Name"+name_position);

	return (newname);
endfunction



///////////////////
//  limits the wandering range of the NPC
///////////////////

function drop_anchor()
	var MyTemplateElem := GetNpcTemplateElem (me.npctemplate);
	var dstart := MyTemplateElem.dstart;
	if (!dstart)
		dstart := 10;
	endif

	var psub := MyTemplateElem.psub;
	if (!psub)
		psub := 10;
	endif

	if (dstart)
		SetAnchor (me.x, me.y, dstart, psub);
	endif
endfunction




///////////////////
//  determines if the NPC is one that should loot
///////////////////

function IAmALooter ()
	if ( GetObjProperty (me, "summoned") )
		return 0;
	endif

	var MyTemplateElem := GetNpcTemplateElem (me.npctemplate);
	if (MyTemplateElem.noloot)
		return 0;
	endif

	return 1;
endfunction




///////////////////
//  NPC loots items
///////////////////

function grabloot()
	foreach item in ListItemsNearLocation (me.x, me.y, me.z, 6, me.realm)
		if (item.movable or item.objtype == UOBJ_CORPSE)
			if (CheckLoSAt (me, item.x, item.y, item.z) )
				var sh := GetStandingHeight (item.x, item.y, item.z, item.realm);
				if (sh.multi or sh.multi.serial)
					break;
				endif

				var info := cint(GetMapInfo( item.x , item.y, item.realm ).landtile);
				if (info > cint(0x405) and info < cint(0x456))
					break;
				endif

				if ( item.movable and (item.objtype < UOBJ_STARTHAIR or item.objtype > UOBJ_ENDHAIR)  )
					RunToItem (item);
					if ( Distance (me, item) < 2 and MoveItemToContainer (item, me.backpack) )
						EraseObjProperty (me, "killme");
						say ("*yoink*");
						sleep(2);
						return;
					else
						return;
					endif

				elseif (item.objtype == UOBJ_CORPSE)
					if (!GetObjProperty(item,"#ignoreit") )
						var Items := array { };
						foreach thingie in EnumerateItemsIncontainer (item)
							if (thingie.objtype < STARTHAIR or thingie.objtype > ENDHAIR and !GetObjProperty(thingie,"#ignoreit") )
								Items [ len(Items)+1 ] := thingie;
							endif
						endforeach

						if ( len (Items) )
							RunToItem (item);
							if (distance (me, item) < 2)
								foreach thingie in Items
									if ( MoveItemToContainer (thingie, me.backpack) )
										say("*yoink*");
										EraseObjProperty (me, "killme");
										sleep(2);
										return;
									else
										SetObjProperty (thingie, "#ignoreit", 1);
									endif
								endforeach
							endif
						else
							SetObjProperty (item, "#ignoreit", 1);
						endif
					endif
				endif
			endif
		endif
	endforeach
endfunction




///////////////////
//  NPC runs towards item
///////////////////

function RunToItem (byref item)
	for numsteps := 1 to 8
		if ( Distance (me, item) < 2 )
			return;
		else
			RunToward (item);
		endif
	endfor
endfunction




///////////////////
//  clears out any events waiting in the NPC's que
///////////////////

function ClearEventQueue ()
	var ev;
	repeat
		ev := wait_for_event(0);
	until (!ev);
endfunction




///////////////////
//  Clear our hitlist if we're at full HP
///////////////////

function ClearHitList()
	if (GetVital (me, "Life") == GetVitalMaximumValue (me, "Life"))
		EraseObjProperty (me, "#hitlist");
	endif
endfunction





///////////////////
//  NPC commits suicide
///////////////////

function KillMe ()
	foreach item in EnumerateItemsInContainer (me.backpack)
		//make sure its only top-level items
		if (item.container and item.container == me.backpack)
			if (!item.newbie and item.movable)
				if ((item.objtype < UOBJ_STARTHAIR or item.objtype > UOBJ_ENDHAIR))
		MoveObjectToLocation( item, me.x, me.y, me.z, me.realm, MOVEOBJECT_FORCELOCATION );
					if (item.decayat)
						item.decayat := ReadGameClock() + 600;
					endif
				endif
			endif
		endif
	endforeach

	MoveObjectToLocation( me, 5288, 1176, 0, REALM_BRITANNIA, MOVEOBJECT_FORCELOCATION );
	SetObjProperty (me, "guardkill", 1);
	ApplyRawDamage (me, CINT (GetVital (me, "Life")/100) + 100);
endfunction





///////////////////
//  NPC commits suicide
///////////////////

function KillNPC (npc)
	//make absolutely sure that its an NPC...
	if (!npc.npctemplate)
		return;
	endif

	foreach item in EnumerateItemsInContainer (npc.backpack)
		//make sure its only top-level items
		if (item.container and item.container == npc.backpack)
			if (!item.newbie and item.movable)
				if ((item.objtype < UOBJ_STARTHAIR or item.objtype > UOBJ_ENDHAIR))
	MoveObjectToLocation( item, npc.x, npc.y, npc.z, npc.realm, MOVEOBJECT_FORCELOCATION );
					if (item.decayat)
						item.decayat := ReadGameClock() + 600;
					endif
				endif
			endif
		endif
	endforeach

	MoveObjectToLocation( npc, 5288, 1176, 0, REALM_BRITANNIA, MOVEOBJECT_FORCELOCATION );
	SetObjProperty (npc, "guardkill", 1);
	RevokePrivilege (npc, "invul");
	ApplyRawDamage (npc, CINT (GetVital (npc, "Life")/100) + 100);
endfunction




///////////////////
//  acts as though the NPC used a bandage (but doesn't actually use a bandage)
///////////////////

function ApplyHealing()
	if (GetVital (me, "Life") < CINT (GetVitalMaximumValue (me, "Life") * 0.25))
		if (!GetObjProperty (me, "#nextpotion"))
			SetObjProperty (me, "#nextpotion", ReadGameClock() - 20);
		endif
		if (GetObjProperty (me, "#nextpotion") < ReadGameClock () )
			var potion_count := GetObjProperty (me, "#usedpotioncount");
			if (!potion_count)
				potion_count := 0;
			endif
			if (potion_count < 5)
				sleepms (RandomInt (1000) + 500);
				var healed_amount := RandomDiceRoll ("5d12+12") * 100;
				if (CINT (GetVitalMaximumValue (me, "Life")/2) < healed_amount)
					healed_amount := CINT (GetVitalMaximumValue (me, "Life")/2);
				endif
				if (healed_amount < 4000)
					healed_amount := 4000;
				endif
				if (GetVital (me, "Life") + healed_amount > GetVitalMaximumValue (me, "Life"))
					healed_amount := GetVitalMaximumValue (me, "Life") - GetVital (me, "Life");
				endif

				SetVital (me, "Life", GetVital (me, "Life") + healed_amount);
				PlaySoundEffect (me, SFX_DRINK);
				sleepms (250);
				PlayObjectCenteredEffect (me, EFFECT_BLUE_SPARKLE, 10, 10);
				PlaySoundEffect (me,  SFX_HEAL );
				SetObjProperty (me, "#usedpotioncount", (potion_count+1));
				SetObjProperty (me, "#nextpotion", ReadGameClock()+20);
				return;
			endif
		endif
	endif

	if (!GetObjProperty (me, "#nexthealing"))
		SetObjProperty (me, "#nexthealing", ReadGameClock() - 20);
	endif
	if (GetObjProperty (me, "#nexthealing") > ReadGameClock () )
		return;
	endif
	var bandage_count := GetObjProperty (me, "#usedbandagecount");
	if (!bandage_count)
		bandage_count := 0;
	endif

	if (bandage_count > 10)
		SetObjProperty (me, "#nexthealing", ReadGameClock()+20);
		return;
	endif

	if (me.poisoned)
		CurePoison (me, 0, 0);
	else
		sleepms (RandomInt (1000) + 500);
		var healed_amount := RandomDiceRoll ("5d10+10") * 100;
		if (CINT (GetVitalMaximumValue (me, "Life")/4) < healed_amount)
			healed_amount := CINT (GetVitalMaximumValue (me, "Life")/4);
		endif
		if (healed_amount < 2000)
			healed_amount := 2000;
		endif
		if (GetVital (me, "Life") + healed_amount > GetVitalMaximumValue (me, "Life"))
			healed_amount := GetVitalMaximumValue (me, "Life") - GetVital (me, "Life");
		endif
		SetVital (me, "Life", GetVital (me, "Life") + healed_amount);
	endif

	var bandaging_time := 20 - CINT (GetAttribute (me, "Anatomy")/15);
	SetObjProperty (me, "#usedbandagecount", (bandage_count+1));
	SetObjProperty (me, "#nexthealing", ReadGameClock()+bandaging_time);
endfunction




///////////////////
//  makes the NPC walk back towards their home points.   Also used to make the NPC respawn using the merchant spawner, so
//  don't use this with an NPC that's spawned using another spawner, because otherwise you'll end up with 2
///////////////////

function WalkHome()
	if (me.frozen)
		return 0;
	endif

	var myhome := array {};
	if (!GetObjProperty (me, "myhome"))
		myhome[1] := me.x;
		myhome[2] := me.y;
		myhome[3] := me.z;
		myhome[4] := me.realm;
		SetObjProperty (me, "myhome", myhome);
	else
		myhome := GetObjProperty (me, "myhome");
	endif

	if ( me.x == myhome[1] and me.y == myhome[2] )
		SetAnchor ( me.x, me.y, 3, 80 );
		return;
	endif
  if (!myhome[4])
    myhome[4] := me.realm;
  endif

	for i := 1 to 12
		WalkTowardLocation (myhome[1], myhome[2]);
		if (me.x == myhome[1] and me.y == myhome[2] )
			SetAnchor (myhome[1], myhome[2], 3, 80);
			return;
		endif
	endfor
	MoveObjectToLocation( me, myhome[1], myhome[2], myhome[3], myhome[4], MOVEOBJECT_FORCELOCATION );
	SetAnchor (myhome[1], myhome[2], 3, 80);
endfunction


function DetermineLocal (me)
	if (me.realm == "tokuno")
		return 1;
	else
		if (me.realm == "malas")
			if ((me.x >= 0 and me.x <= 224) and (me.y >= 0 and me.y <= 720))
				return 1; //Tokuno
			elseif ((me.x >= 255 and me.x <= 506) and (me.y >= 619 and me.y <= 1180))
				return 1; //Tokuno
			else
				return 2; //Malas
			endif
		endif
	endif
	return 0;
endfunction



