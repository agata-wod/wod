include "include/client";
include "include/gumps";
include "include/string";
include "include/eventid";
include "include/sysevent";
include "include/npcbackpacks";
include "../pkg/npcs/npc_util";
include "../pkg/npcs/npcinfo";
include "../pkg/npcs/main_ai/combat/spellcasting";

const HALT_THRESHOLD := 15;

var me := Self ();
var masterserial := GetObjProperty (me , "master");
if (!masterserial)
	var waitcounter := 0;
	repeat
		sleep (1);
		waitcounter := waitcounter + 1;
		masterserial := GetObjProperty (me, "master");
	until (masterserial or waitcounter > 20);
	if (!masterserial)
		GoWild ();
	endif
endif

var master;
var freetime;
var chaseloop := 0;
var summons := 0;
var guarding :=0;
var following := 0;
var staying := 0;
var nextwatch := ReadGameClock();

Const BATTLEORDERS_NEVER_BREATH_FIRE := 1;
Const BATTLEORDERS_ALWAYS_BREATH_FIRE := 2;
Const BATTLEORDERS_CHECK_FOR_FRIENDLIES := 3;
var battle_orders := BATTLEORDERS_NEVER_BREATH_FIRE;

var dragonspeak := 0;
var caster := 0;
var saywords := 0;
var firebreather := 0;
var immobile := 0;
var healer := 0;
var purecaster := 0;
var nexthealingtime := 0;




///////////////////
//  Called whenever the NPC is started or restarted
///////////////////

function DoStartupSetup ()
	var MyTemplateElem := GetNpcTemplateElem (me.npctemplate);
	if (!MyTemplateElem)
		KillMe ();
	endif

	if (me.name ["<random>"] )
		me.name := RandomName (me);
	endif

	if (!me.backpack)
		var newpack := CreateItemAtLocation (5288, 1176, 0, UOBJ_BACKPACK, 1, REALM_BRITANNIA);
		if (!EquipItem (me, newpack))
			DestroyItem (newpack);
		endif
	endif

	if (getobjproperty (me, "freetime"))
		freetime := GetObjProperty (me, "freetime");
	else
		freetime := ReadGameClock() + 2400;
		SetObjProperty (me, "freetime", freetime);
	endif

	if (me.npctemplate ["dragon"] or me.npctemplate ["hiryu"])
		dragonspeak := 1;
	endif

	if (MyTemplateElem.script["spellcaster"] or MyTemplateElem.script["healer"])
		caster := 1;
		saywords := MyTemplateElem.saywords;
	elseif (MyTemplateElem.spellcaster)
		caster := 1;
		saywords := MyTemplateElem.saywords;
	elseif (GetObjProperty (me, "spellcaster") or GetObjProperty (me, "healer"))
		caster := 1;
		saywords := MyTemplateElem.saywords;
	endif

	//mostly for summoned elementals.  Purecasters will try to avoid melee combat
	if (MyTemplateElem.purecaster)
		caster := 1;
		purecaster := 1;
		saywords := MyTemplateElem.saywords;
	endif

	//Mostly for summoned water elements (who will heal)
	if (MyTemplateElem.istamedhealer)
		healer := 1;
		caster := 1;
		purecaster := 1;
		saywords := MyTemplateElem.saywords;
	endif
	
	if (MyTemplateElem.meleecaster)
		caster := 1;
		purecaster := 0;
		saywords := MyTemplateElem.saywords;
	endif
	

	if (MyTemplateElem.script["firebreather"])
		firebreather := 1;
	elseif (GetObjProperty (me, "firebreather"))
		firebreather := 1;
	endif

	if (MyTemplateElem.script["immobile"])
		immobile := 1;
	elseif (GetObjProperty (me, "immobile"))
		immobile := 1;
	endif

	if (!GetObjProperty (me, "#nextpoop"))
		SetObjProperty (me, "#nextpoop", ReadGameClock() + cint(RandomInt(1200)));
	endif

	//light source for NPCs that should glow
	if ( MyTemplateElem.light )
		var light_type := MyTemplateElem.light;
		var light := CreateItemAtLocation (5288, 1176, 0, light_type, 1, REALM_BRITANNIA);
		light.newbie := 1;
		light.facing := 29;
		if (!EquipItem (me, light))
			DestroyItem (light);
		endif
	endif

	//dress them
	if (MyTemplateElem.dress)
		var parms :=array {};
		parms[1]:= me;
		parms[2]:= MyTemplateElem.dress;
		start_script("::/misc/dressme", parms);
		sleep (1);
		foreach item in ListEquippedItems (me)
			if (item != me.backpack)
				item.newbie := 1;
			endif
		endforeach
	elseif (MyTemplateElem.equip)
		EquipFromTemplate (me, MyTemplateElem.equip);
		sleep (1);
		foreach item in ListEquippedItems (me)
			if (item != me.backpack)
				item.newbie := 1;
			endif
		endforeach
	endif

	if (MyTemplateElem.rank)
		var rank := CINT (MyTemplateElem.rank);
		SetObjProperty (me, "rank", rank);
	endif
endfunction