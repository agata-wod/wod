var runaway := 0;

function CloseDistance (byref opponent)
	var d := distance (me, opponent);
	if (d > 15)
		RunToward (opponent);
		return CLOSEDISTANCE_MOVED;
	endif

	if (HasBow ())
		if (d < 5)
			if (runaway < 15)
				RunAwayFrom (opponent);
				runaway := runaway + 1;
				return CLOSEDISTANCE_MOVED;
			endif
			ChangeWeapons ();
			return CLOSEDISTANCE_DIDNT_MOVE;
		else
			if (!CheckLoSAt (me, opponent.x, opponent.y, opponent.z))
				RunRandom (opponent, 2);
				return CLOSEDISTANCE_MOVED;
			endif
			return CLOSEDISTANCE_DIDNT_MOVE;
		endif	
	else		//We don't have a bow equipped
		if (d > 1)
			RunToward ( opponent );
			return CLOSEDISTANCE_MOVED;
		endif
		return CLOSEDISTANCE_DIDNT_MOVE;
	endif
endfunction




///////////////////
//  Determines if we have a bow equipped, and if so, if we have ammo for it
///////////////////

function HasBow ()

	var ammotype := 0;
	var bowarmed := 0;
	var weapon := me.weapon;
	if (!weapon.objtype)
		return 0;
	endif
	case (weapon.objtype)
		0x13B2:
		0x26c2:
		0x27f0:
		0x2d2a:
		0x2d2b:
			bowarmed := 1;
			ammotype := 0xf3f;//bow and arrows
		0x0f4f:
		0x13fd:
		0x26c3:
			bowarmed := 1;
			ammotype := 0x1bfb;//xbow and bolts
		0x2790:
			bowarmed := 1;
			ammotype := 0x27ac;//shuriken
		0x27db:
			bowarmed := 1;
			ammotype := 0x255d;//throwing axes
		0x27f5:
			bowarmed := 1;
			ammotype := 0x2804;//darts
		0x16050:
			bowarmed := 1;
			ammotype := 0x16051;//firebow and firearrows
		default: return 0;
	endcase
	if (!bowarmed)
		return 0;
	endif

	foreach item in enumerateitemsincontainer (me.backpack)
		if (item.objtype == ammotype)
			return 1;
		endif
	endforeach

	ChangeWeapons ();
	return 0;
endfunction




///////////////////
//  Equips a non-bow weapon
///////////////////

function ChangeWeapons ()
	var weapon := GetEquipmentByLayer (me, LAYER_HAND1);
	var shield := GetEquipmentByLayer (me, LAYER_HAND2);

	if (weapon)
		MoveItemToContainer (weapon, me.backpack);
	endif
	if (shield)
		MoveItemToContainer (shield, me.backpack);
	endif

	foreach item in ListRootItemsInContainer (me.backpack)
		if (item.isa (POLCLASS_WEAPON) )
			if (item.objtype != 0x13B2 and item.objtype != 0x0f4f
					and item.objtype != 0x13fd and item.objtype != 0x6050
					and item.objtype != 0x26c2 and item.objtype != 0x27f0
					and item.objtype != 0x2d2a and item.objtype != 0x2d2b
					and item.objtype != 0x26c3 and item.objtype != 0x2790
          and item.objtype != 0x27db and item.objtype != 0x27f5)
				EquipItem (me, item);
			endif
		endif
	endforeach
endfunction
