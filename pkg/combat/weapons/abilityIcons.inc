
function SetWeaponAbility (byref who, ability)
	if (GetObjProperty (who, "#disabledabilities") > ReadGameClock())
		//cant use abilities
		ResetWeaponAbilities (who);
		SendSysMessage (who, "You can't use weapon abilities right now.");
		return 0;
	else
		SetObjProperty (who, "#weaponability", ability);
	endif
	return 1;
endfunction

function ResetWeaponAbilities (byref who)
	EraseObjProperty (who, "#weaponability");
	var packet := CreatePacket(0xBF, MSGLEN_VARIABLE);
	packet.SetSize(5);
	packet.SetInt8(4, 0x21);
	packet.SendPacket(who);
endfunction

