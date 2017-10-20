//use math;

include "include/client";
include "include/math";
include "include/gumps";
include "include/attributes";
include "include/newperformaction";

const ORDERGUARDTYPE := 1;
const CHAOSGUARDTYPE := 2;
const CHAOSSHIELDTYPE := 0x1bc3;
const ORDERSHIELDTYPE := 0x1bc4;
const GARGCHAOSSHIELDTYPE := 0x4228;
const GARGORDERSHIELDTYPE := 0x422a;

function AddVirtue (who, amount := 1, nomsg:=0)
	if (!amount)
		amount := 1;
	endif

	var yourvirtue := GetObjProperty (who, "virtue");
	if (!yourvirtue)
		yourvirtue := 0;
	elseif (yourvirtue > 50000)
		return;
	endif

	var desc;
	case (Abs(amount))
		1: desc := " a tiny amount of ";
		2: desc := " a very small amount of ";
		3: desc := " a small amount of ";
		4: desc := " some ";
		5: desc := " a good amount of ";
		6: desc := " a lot of ";
		7: desc := " a great amount of ";
		default: desc := " a massive amount of ";
	endcase

	yourvirtue := yourvirtue + amount;
	if (!nomsg)
		if (amount > 0)
			SendSysMessage (who, "You have gained" + desc + "Virtue");
		else
			SendSysMessage (who, "You have lost" + desc + "Virtue");
		endif
	endif
	SetObjProperty (who, "virtue", yourvirtue);

	if (GetObjProperty (who, "guardtype") == ORDERGUARDTYPE)
		if ( randomint(8) < amount )
			BonusHonor (who);
		endif
	endif
	SetPlayerVirtueTitle (who);
endfunction

function AwardVirtueForRes(you,them)

	if ( GetObjProperty (them, "skillloss") )
		EraseObjProperty (them, "skillloss");
		AddVirtue (you, 5);
	endif

endfunction




///////////////////
//  Figures out how much virtue the player has
///////////////////

function GetVirtueAmount (you)
	var yourvirtue := GetObjProperty (you, "virtue");
	if (!yourvirtue)
		return 0;
	else
		return yourvirtue;
	endif
endfunction




///////////////////
//  Sets the title for a virtue guard
///////////////////

function SetPlayerVirtueTitle (player)
	var playervirtue := GetObjProperty (player, "virtue");
	if (GetObjProperty (player, "guardtype"))
		if (GetObjProperty (player, "guardtype") == CHAOSGUARDTYPE)
			player.title_prefix := GetVirtueGuardTitle (player, playervirtue);
		else
			var playerhonor := GetObjProperty (player, "honor");
			playervirtue := playerhonor[2];
			player.title_prefix := GetVirtueGuardTitle (player, playervirtue);
		endif
	else
		player.title_prefix := GetNonVGTitle (player, playervirtue);
	endif
endfunction



///////////////////
//  Sets the title for a virtue guard
///////////////////

function GetPlayerVirtueTitle (player)
	var playervirtue := GetObjProperty (player, "virtue");
	if (GetObjProperty (player, "guardtype"))
		if (GetObjProperty (player, "guardtype") == CHAOSGUARDTYPE)
			return GetVirtueGuardTitle (player, playervirtue);
		else
			var playerhonor := GetObjProperty (player, "honor");
			playervirtue := playerhonor[2];
			return GetVirtueGuardTitle (player, playervirtue);
		endif
	else
		return GetNonVGTitle (player, playervirtue);
	endif
endfunction



///////////////////
//  Figures out the appropriate title for a virtue guard
///////////////////

function GetVirtueGuardTitle (who, virtue)
	if (virtue == error)
		return "";
	endif

	if (virtue < 200)
		return "Squire";
	elseif ( virtue < 400 )
		if (who.gender)
			return "Dame";
		else
			return "Sir";
		endif
	elseif ( virtue < 800 )
		if (who.gender)
			return "The Esteemed Dame";
		else
			return "The Esteemed Sir";
		endif
	elseif ( virtue < 1600 )
		if (who.gender)
			return "The Noble Dame";
		else
			return "The Noble Sir";
		endif
	elseif ( virtue < 3200 )
		if (who.gender)
			return "The Illustrious Dame";
		else
			return "The Illustrious Sir";
		endif
	elseif ( virtue < 6400 )
		if (who.gender)
			return "Lady";
		else
			return "Lord";
		endif
	elseif ( virtue < 12800 )
		if (who.gender)
			return "The Noble Lady";
		else
			return "The Noble Lord";
		endif
	elseif ( virtue < 25600 )
		if (who.gender)
			return "The Illustrious Lady";
		else
			return "The Illustrious Lord";
		endif
	else
		if (who.gender)
			return "The Glorious Lady";
		else
			return "The Glorious Lord";
		endif
	endif
endfunction




///////////////////
//  Figures out the appropriate title for a non-virtue guard
///////////////////

function GetNonVGTitle (who, virtue)

        who := "not used right now";

	if (virtue == error)
		return "";
	endif

	if ( virtue < -25600 )
		return "The Sinister";
	elseif ( virtue < -12800 )
		return "The Vile";
	elseif ( virtue < -6400 )
		return "The Ignoble";
	elseif ( virtue < -3200 )
		return "The Malicious";
	elseif ( virtue < -1600 )
		return "The Dastardly";
	elseif ( virtue < -800 )
		return "The Dishonorable";
	elseif ( virtue < -400 )
		return "The Unsavory";
	elseif ( virtue < -200 )
		return "The Rude";
	elseif ( virtue < 200 )
		return "";
	elseif ( virtue < 400 )
		return "The Honorable";
	elseif ( virtue < 800 )
		return "The Commendable";
	elseif ( virtue < 1600 )
		return "The Admirable";
	elseif ( virtue < 3200 )
		return "The Reknowned";
	elseif ( virtue < 6400 )
		return "The Famed";
	elseif ( virtue < 12800 )
		return "The Noble";
	elseif ( virtue < 25600 )
		return "The Illustrious";
	else
		return "The Glorious";
	endif
endfunction




///////////////////
//  Returns how much virtue the player needs to get to the next 'level'
///////////////////

function GetFullVirtueRequiredForNextLevel (player)
	var playervirtue := GetObjProperty (player, "virtue");
	if (GetObjProperty (player, "guardtype"))
		if (GetObjProperty (player, "guardtype") == ORDERGUARDTYPE)
			var playerhonor := GetObjProperty (player, "honor");
			playervirtue := playerhonor[2];
		endif
	endif
	
	if (!playervirtue)
		return 200;
	elseif ( playervirtue < -25600 )
		return 25600;
	elseif ( playervirtue < -12800 )
		return 12800;
	elseif ( playervirtue < -6400 )
		return 6400;
	elseif ( playervirtue < -3200 )
		return 3200;
	elseif ( playervirtue < -1600 )
		return 1600;
	elseif ( playervirtue < -800 )
		return 800;
	elseif ( playervirtue < -400 )
		return 400;
	elseif ( playervirtue < -200 )
		return 200;
	elseif ( playervirtue < 200 )
		return 200;
	elseif ( playervirtue < 400 )
		return 200;
	elseif ( playervirtue < 800 )
		return 400;
	elseif ( playervirtue < 1600 )
		return 800;
	elseif ( playervirtue < 3200 )
		return 1600;
	elseif ( playervirtue < 6400 )
		return 3200;
	elseif ( playervirtue < 12800 )
		return 6400;
	elseif ( playervirtue < 25600 )
		return 12800;
	else
		return 25600;
	endif
endfunction




///////////////////
//  Returns how much virtue makes up this virtue 'level'
///////////////////

function GetVirtueRequiredForNextLevel (player)
	var playervirtue := GetObjProperty (player, "virtue");
	if (GetObjProperty (player, "guardtype"))
		if (GetObjProperty (player, "guardtype") == ORDERGUARDTYPE)
			var playerhonor := GetObjProperty (player, "honor");
			playervirtue := playerhonor[2];
		endif
	endif

	if (!playervirtue)
		return 200;
	endif

	//OK, time for some strange calculations...
	var tempplayervirtue := CINT (playervirtue/100);
	if (!tempplayervirtue)
		if (playervirtue > 0)
			return (200 - playervirtue);
		else
			return (0 - playervirtue);
		endif
	endif
	
	var squaredcounter := -1;
	repeat
		tempplayervirtue := CINT (tempplayervirtue/2);
		squaredcounter := squaredcounter + 1;
	until (!tempplayervirtue);

	if (playervirtue > 0)
		case (squaredcounter)
			0:	return (200 - playervirtue);
			1:	return (400 - playervirtue);
			2:	return (800 - playervirtue);
			3:	return (1600 - playervirtue);
			4:	return (3200 - playervirtue);
			5:	return (6400 - playervirtue);
			6:	return (12800 - playervirtue);
			7:	return (25600 - playervirtue);
			default:
				return 0;
		endcase;
	else
		playervirtue := 0 - playervirtue;
		case (squaredcounter)
			0:	return (0 - playervirtue);
			1:	return (playervirtue - 199);
			2:	return (playervirtue - 399);
			3:	return (playervirtue - 799);
			4:	return (playervirtue - 1599);
			5:	return (playervirtue - 3199);
			6:	return (playervirtue - 6399);
			7:	return (playervirtue - 12799);
			default:
				return (playervirtue - 25599);
		endcase;
	endif
endfunction



function CanJoin(guard, recruit, guardtype, shieldtype)

	if (recruit.npctemplate) return; endif
	if (!recruit.acctname) return; endif
	if (guard == recruit) return; endif
  if(recruit.race == RACE_GARGOYLE)
    if(guardtype == CHAOSGUARDTYPE)
      shieldtype := 0x4228;
    else
      shieldtype := 0x422a;
    endif
  endif

	var yourtype := GetObjProperty(recruit,"guardtype");
	if (yourtype)
		SendSysMessage(guard,"They're already a Virtue Guard.");
		if (yourtype == guardtype)
			var newshield := CreateItemAtLocation(5288 , 1176 , 0, shieldtype,1, REALM_BRITANNIA);
			if (!EquipItem(recruit,newshield))
				DestroyItem(newshield);
			endif
			return;
		endif
	endif

	var basestr := CINT (GetAttributeBaseValue (recruit, "Strength") / 10);
	if (basestr < 90)
		SendSysMessage(guard,"They don't seem strong enough, to you.");
		return;
	endif

	var leadership := GetAttribute (recruit, ATTRIBUTEID_LEADERSHIP);
	if (leadership < 90)
		SendSysMessage(guard,"They seem to lack the necessary leadership skills...");
		return;
	endif

	if ( GetVirtueAmount(recruit) < 100 )
		SendSysMessage (guard, "They don't seem to be terribly Virtuous...");
		return;
	endif

	SendSysMessage (recruit, "Do you want to join the Virtue Guards?");
	if (!YesNo (recruit, "Yes!")) 
		SendSysMessage(guard,"They don't want to join.");
		return; 
	endif

	setobjproperty(recruit,"guardtype",guardtype);
	recruit.title_prefix := "Squire";
	recruit.title_suffix := ", Virtue Guard";
	var newshield := CreateItemAtLocation(recruit.x , recruit.y , recruit.z,
		shieldtype,1, recruit.realm);
	EquipItem(recruit,newshield);
	PlaySoundEffect(recruit, SFX_SPELL_BLESS);
	PlayObjectCenteredEffect( recruit, FX_BLESS_EFFECT, 10,10);

	SendSysMessage(guard,"Tell them to MAKE US PROUD!");

endfunction

function BestowHonor (me, you, howmuch := 0)
	if (you.npctemplate)
		return;
	elseif (!you.acctname)
		return;
	elseif (me == you)
		return;
	endif

	var yourtype := GetObjProperty(you,"guardtype");
	if (!yourtype)
		SendSysMessage (me, "That's not a Virtue Guard");
		return; 
	endif
	if (yourtype != ORDERGUARDTYPE)
		SendSysMessage (me, "That's not an Order Guard");
		return;
	endif

//honor[1] is how many points I've spent, honor[2] is points received
//honor[3] is the last person you honored, so it takes 
//at least three twinks to sploit

	var myvirtue := GetVirtueAmount (me);
	var myhonor := GetObjProperty(me,"honor");
	if (!myhonor)
		myhonor := {};
		myhonor[1] := myvirtue;
		myhonor[2] := 0;
		myhonor[3] := 0;
	endif

	var yourvirtue := GetVirtueAmount (you);
	var yourhonor := GetObjProperty (you, "honor");
	if (!yourhonor)
		yourhonor := {};
		yourhonor[1] := yourvirtue;
		yourhonor[2] := 0;
		yourhonor[3] := 0;
	endif

	if (myhonor[3] != you.serial)
		var myunspent := myvirtue - myhonor[1];
		if (howmuch)
			myunspent := cint(myunspent/2);
		endif
		if ( myunspent > 0)
			yourhonor[2] := yourhonor[2] + myunspent;
			myhonor[1] := myhonor[1] + myunspent;
			myhonor[3] := you.serial;
			SetPlayerVirtueTitle (you);
			SetObjProperty(me,"honor",myhonor);
			SetObjProperty(you,"honor",yourhonor);
		endif
	endif

	if (!howmuch)
		NewPerformAction(me,NPA_ANIM_EMOTE, NPA_EMOTE_BOW);
		SendSysMessage(me,"You honor " + you.name);
		SendSysMessage(you,me.name + " honors you");
	else
		NewPerformAction (me, NPA_ANIM_EMOTE, NPA_EMOTE_SALUTE);
		SendSysMessage (me, "You respect " + you.name);
		SendSysMessage (you, me.name + " respects you");
	endif

endfunction

function BonusHonor (me, amt := 1)
	//we have to make up for "spent points" leaving the system via
	//capped characters, quitting character, and recriminations:

	var myhonor := GetObjProperty (me, "honor");
	if (!myhonor)
		return;
	endif
	myhonor[1] := myhonor[1] - amt;
	SetObjProperty (me, "honor", myhonor);
endfunction

function Censure( me, you )

	if (you.npctemplate) return; endif
	if (!you.acctname) return; endif
	if (me == you) return; endif

	var yourtype := GetObjProperty(you,"guardtype");
	if (!yourtype)
		SendSysMessage(me,"That's not a Virtue Guard");
		return; 
	endif
	if (yourtype != ORDERGUARDTYPE)
		SendSysMessage(me,"That's not an Order Guard");
		return;
	endif

//honor[1] is how many points I've spent, honor[2] is points received
//honor[3] is the last person you honored, so it takes 
//at least three twinks to sploit

	var myvirtue := GetVirtueAmount (me);
	var myhonor := GetObjProperty(me,"honor");
	if (!myhonor)
		myhonor := {};
		myhonor[1] := 0;
		myhonor[2] := 0;
		myhonor[3] := 0;
	endif

//	var yourvirtue := GetVirtueAmount (you);
	var yourhonor := GetObjProperty(you,"honor");
	if (!yourhonor)
		yourhonor := {};
		yourhonor[1] := 0;
		yourhonor[2] := 0;
		yourhonor[3] := 0;
	endif

	if (myhonor[3] != you.serial)
		var myunspent := myvirtue - myhonor[1];
		myunspent := cint(myunspent/10);
		if ( myunspent > 0)
			yourhonor[2] := yourhonor[2] - myunspent;
			myhonor[1] := myhonor[1] - myunspent;
			myhonor[3] := you.serial;
			SetPlayerVirtueTitle (you);
			SetPlayerVirtueTitle (me);
			SetObjProperty (me, "honor", myhonor);
			SetObjProperty (you, "honor", yourhonor);
		endif
	endif

	NewPerformAction(me, NPA_ANIM_FIDGET, NPA_ACT_FIDGET, NPA_SUB_FIDGET_2);
	sleepms(500);
	NewPerformAction(me, NPA_ANIM_FIDGET, NPA_ACT_FIDGET, NPA_SUB_FIDGET_1);
	sleepms(500);
	NewPerformAction(me, NPA_ANIM_EAT);
	SendSysMessage(me,"You censure " + you.name);
	SendSysMessage(you,me.name + " has censured you");

endfunction
