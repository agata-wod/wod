use polsys;

 //1001: Dismount (cliloc1=1075635,cliloc2=1075636)
 //1002: Disarm (cliloc1=1075637,cliloc2=1075638)
 //1005: Nightsight (cliloc1=1075643,cliloc2=1075644)
 //1006: Death Strike 
 //1007: Evil Omen 
 //1008: unknown? (GumpID 0x7556)
 //1009: Regeneration (cliloc1=1044106,cliloc2=1075106)
 //1010: Divine Fury 
 //1011: Enemy Of One 
 //1012: Stealth (cliloc1=1044107,cliloc2=1075655)
 //1013: Active Meditation (cliloc1=1044106,cliloc2=1075106)
 //1014: Blood Oath caster 
 //1015: Blood Oath curse 
 //1016: Corpse Skin 
 //1017: Mindrot 
 //1018: Pain Spike 
 //1019: Strangle 
 //1020: Gift of Renewal 
 //1021: Attune Weapon 
 //1022: Thunderstorm 
 //1023: Essence of Wind 
 //1024: Ethereal Voyage 
 //1025: Gift Of Life 
 //1026: Arcane Empowerment 
 //1027: Mortal Strike 
 //1028: Reactive Armor (cliloc1=1075812,cliloc2=1075813)
 //1029: Protection (cliloc1=1075814,cliloc2=1075815)
 //1030: Arch Protection (cliloc1=1075816,cliloc2=1075816)
 //1031: Magic Reflection (cliloc1=1075817,cliloc2=1075818)
 //1032: Incognito (cliloc1=1075819,cliloc2=1075820)
 //1033: Disguised 
 //1034: Animal Form
 //1035: Polymorph (cliloc1=1075824,cliloc2=1075820)
 //1036: Invisibility (cliloc1=1075825,cliloc2=1075826)
 //1037: Paralyze (cliloc1=1075827,cliloc2=1075828)
 //1038: Poison (cliloc1=0x0F8627, cliloc2=0x1069B1)
 //1039: Bleed (cliloc1=0x106a75,cliloc2=0x106a76)
 //1040: Clumsy (cliloc1=0x106a77,cliloc2=0x106a78)
 //1041: Feeble Mind (cliloc1=0x106a79,cliloc2=0x106a7a)
 //1042: Weaken (cliloc1=1075837,cliloc2=1075838)
 //1043: Curse (cliloc1=1075835,cliloc2=1075836)
 //1044: Mass Curse (cliloc1=0x106a7f,cliloc2=0x106a80)
 //1045: Agility (cliloc1=0x106a81,cliloc2=0x106a82)
 //1046: Cunning (cliloc1=0x106a83,cliloc2=0x106a84)
 //1047: Strength (cliloc1=0x106a85,cliloc2=0x106a86)
 //1048: Bless (cliloc1=0x106a87,cliloc2=0x106a88)
 //1049: Sleep
 //1050: Stone Form
 //1051: Spell Plague
 //1052: Nether Bolt
 //1053: Fly (cliloc1=1112193,cliloc2=1112567)
 

const BUFFICON_NIGHTSIGHT := 0x03E9;
const BUFFICON_STEALTH := 0x03F4;
const BUFFICON_MEDITATION := 0x03F5;
const BUFFICON_REACTIVE_ARMOR := 0x0404;
const BUFFICON_PROTECTION := 0x0405;
const BUFFICON_ARCHPROTECTION := 0x0406;
const BUFFICON_MAGIC_REFLECTION := 0x0407;
const BUFFICON_INCOGNITO := 0x0408;
const BUFFICON_POLYMORPH := 0x040B;
const BUFFICON_INVISIBILITY := 0x040C;
const BUFFICON_PARALYZE := 0x040D;
const BUFFICON_POISON := 0x040E;
const BUFFICON_BLEED := 0x040F;
const BUFFICON_CLUMSY := 0x0410;
const BUFFICON_FEEBLEMIND := 0x0411;
const BUFFICON_WEAKEN := 0x0412;
const BUFFICON_CURSE := 0x0413;
const BUFFICON_MASS_CURSE := 0x0414;
const BUFFICON_AGILITY := 0x0415;
const BUFFICON_CUNNING := 0x0416;
const BUFFICON_STRENGTH := 0x0417;
const BUFFICON_BLESS := 0x0418;
const BUFFICON_FLYING := 0x041E;

const BUFFSTATE_ENABLED := 0x0001;
const BUFFSTATE_DISABLED := 0x0000;

function BuffDebuffIcon(byref who, icon, state, time := 0, amt := array {})
	if(who.isa(POLCLASS_NPC))
        return;
	endif
	
	SetBuffDebuffIcon(who, icon, state, time, amt);
endfunction

function SetBuffDebuffIcon(byref who, icon, state, time := 0, amt := array {})
  if (!who)
    return;
  endif
	const BUFF_CMD := 0xDF;
	const OFFSET_MSGLEN := 1;
	const OFFSET_SERIAL := 3;
	var cliloc1 := BuffDebuffCliloc1(icon);
	var cliloc2 := BuffDebuffCliloc2(icon);
 
	var str := "  " + CStr(CInt(Abs(amt[1])));
	var i;
	for (i:=2; i<=amt.size(); i:=i+1)
		str += CChr(0x09) + CStr(CInt(Abs(amt[i])));
	endfor
 
	var outpkt2 := CreatePacket(BUFF_CMD, MSGLEN_VARIABLE);
 
	outpkt2.SetInt16(OFFSET_MSGLEN, outpkt2.GetSize());
	outpkt2.SetInt32(OFFSET_SERIAL, who.serial);
 
	outpkt2.SetInt16(7, icon); // Icon Nr.
	outpkt2.SetInt8(9, 0);
	outpkt2.SetInt8(10, state); //2 additional data, 1 add, 0 remove
	outpkt2.SetInt32(11, 0);
	outpkt2.SetInt16(15, icon); // Icon Nr.
	outpkt2.SetInt8(17, 0);
	outpkt2.SetInt8(18, state); //2 additional data, 1 add, 0 remove
 
	if(state)
		outpkt2.SetInt32(19, 0);
		outpkt2.SetInt16(23, time);
		outpkt2.SetInt16(25, 0x0);
		outpkt2.SetInt8(27, 0x0);
		outpkt2.SetInt32(28, cliloc1);
		outpkt2.SetInt32(32, cliloc2);
		outpkt2.SetInt32(36, 0x0);
		outpkt2.SetInt8(40, 0x0);
		outpkt2.SetInt8(41, 0x1);
		outpkt2.SetUnicodeStringFlipped(42,CAscZ(str),1);
	endif
	outpkt2.SendPacket(who);
endfunction


function BuffDebuffCliloc1 (icon)
	case (icon)
		BUFFICON_NIGHTSIGHT: return 0x001069BB;
		BUFFICON_STEALTH: return 0x001069C7;
		BUFFICON_MEDITATION: return 0x000FEE8A;
		BUFFICON_REACTIVE_ARMOR: return 0x00106A64;
		BUFFICON_PROTECTION: return 0x00106A66;
		BUFFICON_ARCHPROTECTION: return 0x00106A68;
		BUFFICON_MAGIC_REFLECTION: return 0x00106A69;
		BUFFICON_INCOGNITO: return 0x00106A6B;
		BUFFICON_POLYMORPH: return 0x00106A70;
		BUFFICON_INVISIBILITY: return 0x00106A71;
		BUFFICON_PARALYZE: return 0x00106A73;
		BUFFICON_POISON: return 0x000F8627;
		BUFFICON_BLEED: return 0x00106A75;
		BUFFICON_CLUMSY: return 0x00106A77;
		BUFFICON_FEEBLEMIND: return 0x00106A79;
		BUFFICON_WEAKEN: return 0x00106A7D;
		BUFFICON_CURSE: return 0x00106A7B;
		BUFFICON_MASS_CURSE: return 0x00106A7F;
		BUFFICON_AGILITY: return 0x00106A81;
		BUFFICON_CUNNING: return 0x00106A83;
		BUFFICON_STRENGTH: return 0x00106A85;
		BUFFICON_BLESS: return 0x00106A87;
		BUFFICON_FLYING: return 0x0010F881;
		default: return 0;
	endcase
	return 0;
endfunction

function BuffDebuffCliloc2 (icon)
	case (icon)
		BUFFICON_NIGHTSIGHT: return 0x001069BC;
		BUFFICON_STEALTH: return 0x001069C8;
		BUFFICON_MEDITATION: return 0x001067A2;
		BUFFICON_REACTIVE_ARMOR: return 0x00106A65;
		BUFFICON_PROTECTION: return 0x00102E60;
		BUFFICON_ARCHPROTECTION: return 0x00102E60;
		BUFFICON_MAGIC_REFLECTION: return 0x00106A6A;
		BUFFICON_INCOGNITO: return 0x00106A6C;
		BUFFICON_POLYMORPH: return 0x00106A88;
		BUFFICON_INVISIBILITY: return 0x00106A72;
		BUFFICON_PARALYZE: return 0x00106A74;
		BUFFICON_POISON: return 0x001069B1;
		BUFFICON_BLEED: return 0x00106A76;
		BUFFICON_CLUMSY: return 0x00106A78;
		BUFFICON_FEEBLEMIND: return 0x00106A7a;
		BUFFICON_WEAKEN: return 0x00106A7E;
		BUFFICON_CURSE: return 0x00106A80;
		BUFFICON_MASS_CURSE: return 0x00106A80;
		BUFFICON_AGILITY: return 0x00106A82;
		BUFFICON_CUNNING: return 0x00106A84;
		BUFFICON_STRENGTH: return 0x00106A86;
		BUFFICON_BLESS: return 0x00106A88;
		BUFFICON_FLYING: return 0x0010F9F7;
		default: return 0;
	endcase
	return 0;
endfunction

function ResetBuffDebuffIcons(byref who)
	if(who.isa(POLCLASS_NPC))
        return;
	endif
	
	var mods := GetObjProperty(who, "#mods");
	if (!mods)
		return;
	endif
	
	var buffamt, duration;

	foreach amod in mods
		case (amod[1])
			"cstr":
			"str" : buffamt := array {amod[2]};
					duration := amod[3] - ReadGameClock();
					if (amod[2] > 0)
						BuffDebuffIcon(who, BUFFICON_STRENGTH, BUFFSTATE_ENABLED, duration, buffamt);
					else
						BuffDebuffIcon(who, BUFFICON_WEAKEN, BUFFSTATE_ENABLED, duration, buffamt);
					endif
			"cdex":
			"dex" :	buffamt := array {amod[2]};
					duration := amod[3] - ReadGameClock();
					if (amod[2] > 0)
						BuffDebuffIcon(who, BUFFICON_AGILITY, BUFFSTATE_ENABLED, duration, buffamt);
					else
						BuffDebuffIcon(who, BUFFICON_CLUMSY, BUFFSTATE_ENABLED, duration, buffamt);
					endif
			"cint":
			"int" :	buffamt := array {amod[2]};
					duration := amod[3] - ReadGameClock();
					if (amod[2] > 0)
						BuffDebuffIcon(who, BUFFICON_CUNNING, BUFFSTATE_ENABLED, duration, buffamt);
					else
						BuffDebuffIcon(who, BUFFICON_FEEBLEMIND, BUFFSTATE_ENABLED, duration, buffamt);
					endif
			"car":
			"ar"  :	buffamt := array {amod[2]};
					duration := amod[3] - ReadGameClock();
					BuffDebuffIcon(who, BUFFICON_PROTECTION, BUFFSTATE_ENABLED, duration, buffamt);
			"p"   :	duration := amod[3] - ReadGameClock();
					BuffDebuffIcon(who, BUFFICON_PARALYZE, BUFFSTATE_ENABLED, duration);
			"call":
			"all" : buffamt := array {amod[2], amod[2], amod[2]};
					duration := amod[3] - ReadGameClock();
					if (amod[2] > 0)
						BuffDebuffIcon(who, BUFFICON_BLESS, BUFFSTATE_ENABLED, duration, buffamt);
					else
						BuffDebuffIcon(who, BUFFICON_CURSE, BUFFSTATE_ENABLED, duration, buffamt);
					endif
			"cpoly":
			"poly": buffamt := array {amod[2], amod[2], amod[2], CINT(amod[2]/3)};
					duration := amod[3] - ReadGameClock();
					BuffDebuffIcon(who, BUFFICON_POLYMORPH, BUFFSTATE_ENABLED, duration, buffamt);
		endcase
	endforeach
endfunction