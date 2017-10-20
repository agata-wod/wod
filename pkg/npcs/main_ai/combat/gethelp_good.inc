///////////////////
//  looks around the area for other NPCs that are 'friendly' with the NPC being attacked, and asks
//  them to attack
///////////////////

function GetHelpFromAllies ( byref opponent )
	var ev := struct;
	ev.+type := EVID_ALL_ATTACK_CMD;
	ev.+source := opponent;

	var foundhelper := 0;
	foreach mobile in ListMobilesNearLocation ( me.x, me.y, me.z, HALT_THRESHOLD, me.realm )
		if ( IsGood (mobile) and !mobile.warmode and mobile != me and mobile.npctemplate)
			SendEvent (mobile, ev);
			foundhelper := 1;
		endif
	endforeach

	if (foundhelper)
		Say ("Guards!  Help!");
	endif
endfunction
