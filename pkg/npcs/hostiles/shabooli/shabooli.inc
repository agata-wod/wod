///////////////////
//  looks around the area for other NPCs that are 'friendly' with the NPC being attacked, and asks
//  them to attack
///////////////////

function GetHelpFromAllies (byref opponent)

	var ev := struct;
	ev.+type := EVID_ALL_ATTACK_CMD;
	ev.+source := opponent;

	foreach mobile in ListMobilesNearLocation (me.x, me.y, me.z, HALT_THRESHOLD, me.realm)
		if (mobile.npctemplate["shabooli"] and !mobile.warmode and mobile != me)
			SendEvent (mobile, ev);
		endif
	endforeach

	foreach mobile in ListMobilesNearLocationEx (me.x, me.y, me.z, 4, LISTEX_FLAG_HIDDEN, me.realm)
		if (mobile.npctemplate["shabooli"] and !mobile.warmode and mobile != me)
			SendEvent (mobile, ev);
		endif
	endforeach


endfunction
