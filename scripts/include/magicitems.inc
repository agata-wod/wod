use uo;
use os;
use util;
use cfgfile;

include "include/client";
include "include/objtype";
include "include/itemtypes";


Const ITEMTYPE_WEAPON		:= 1;
Const ITEMTYPE_ARMOR		:= 2;
Const ITEMTYPE_SHIELD		:= 3;
Const ITEMTYPE_CLOTHING		:= 4;
Const ITEMTYPE_MUSICAL		:= 5;
Const ITEMTYPE_WAND			:= 6;
Const ITEMTYPE_JEWELRY		:= 7;

Const SKILLBONUS_LEVEL		:= 2;

///////////////////
//  determines if the given item is magical
///////////////////

function IsMagicalItem (item)
	if ((item.maxhp_mod > 0) or item.dmg_mod or item.ar_mod)
		return 1;
	endif
	if (GetObjProperty (item, "equipspell") or GetObjProperty (item, "hitspell"))
		return 1;
	endif
	if (GetObjProperty (item, "silver") or GetObjProperty (item, "skilladv"))
		return 1;
	endif
	if (GetObjProperty (item, "dex") or GetObjProperty (item, "str") or GetObjProperty (item, "int") )
		return 1;
	endif
	if (GetObjProperty (item, "spell") or GetObjProperty (item, "charges"))
		return 1;
	endif
	if (GetObjProperty (item, "skilladv"))
		return 1;
	endif
	if (GetObjProperty (item, "skillbonus"))
		return 1;
	endif

	return 0;
endfunction




///////////////////
//  determines what sort of item the passed item is
///////////////////

function GetItemtype (item)
	case (item.objtype)
		0x0DF2:
		0x0DF3:
		0x0DF4:
		0x0DF5:
			return ITEMTYPE_WAND;

		0X1088:
		0X1086:
		0x108A:
		0X1089:
		0X1085:
		0X1087:
			return ITEMTYPE_JEWELRY;

		UOBJ_FLUTE:
		UOBJ_TAMBORINE:
		UOBJ_TAMBORINE2:
		UOBJ_LUTE:
		UOBJ_LUTE2:
		UOBJ_DRUM:
		UOBJ_HARP:
		UOBJ_STANDING_HARP:
		UOBJ_BAMBOO_FLUTE:
		UOBJ_BAMBOO_FLUTE2:
		UOBJ_AUDCHAR:
		UOBJ_AUDCHAR2:
		UOBJ_FIDDLE:
			return ITEMTYPE_MUSICAL;

		UOBJ_BUCKLER:
		UOBJ_BRONZESHIELD:
		UOBJ_METALSHIELD:
		UOBJ_KITESHIELD:
		UOBJ_KITESHIELD2:
		UOBJ_HEATER:
		UOBJ_HEATER2:
		UOBJ_WOODEN_SHIELD:
		UOBJ_WOODEN_KITESHIELD:
		UOBJ_WOODEN_KITESHIELD2:
		UOBJ_CHAOS_SHIELD:
		UOBJ_ORDER_SHIELD:
		UOBJ_ORDER_SHIELD2:
		UOBJ_G_WOODEN_SHIELD:
		UOBJ_G_KITESHIELD:
		UOBJ_G_SMALL_PLATESHIELD:
		UOBJ_G_MEDIUM_PLATESHIELD:
		UOBJ_G_LARGE_PLATESHIELD:
		UOBJ_G_LARGE_STONESHIELD:
		UOBJ_G_KITESHIELD2:
		UOBJ_G_WOODEN_SHIELD2:
		UOBJ_G_LARGE_PLATESHIELD2:
		UOBJ_G_MEDIUM_PLATESHIELD2:
		UOBJ_G_SMALL_PLATESHIELD2:
		UOBJ_G_LARGE_STONESHIELD2:
		UOBJ_G_CHAOS_SHIELD:
		UOBJ_G_CHAOS_SHIELD2:
		UOBJ_G_ORDER_SHIELD:
		UOBJ_G_ORDER_SHIELD2:
			return ITEMTYPE_SHIELD;

		0x1F03:
		0x204E:
		0x153B:
		0x153D:
		0x1541:
		0x1EFD:
		0x1EFF:
		0x1F01:
		0x1F7B:
		0x1F9F:
		0x1FA1:
		0x1FFD:
		0x1515:
		0x1517:
		0x153A:
		0x1516:
		0x152E:
		0x1537:
		0x1539:
		0x170D:
		0x170F:
		0x170B:
		0x1711:
		0x153F:
		0x1543:
		0x1713:
		0x1714:
		0x1715:
		0x1716:
		0x1717:
		0x1718:
		0x1719:
		0x171A:
		0x171B:
		0x171C:
		0x141b:
		0x1545:
		0x1547:
		0x1549:
		0x154b:
		0x2305:
		0x2306:
		0x2307:
		0x2308:
		0x2309:
		0x230a:
		0x230b:
		0x230c:
		0x230d:
		0x230e:
		0x230f:
		0x2310:
		0x2782:
		0x2783:
		0x278f:
		0x2794:
		0x2796:
		0x2797:
		0x2798:
		0x2799:
		0x279a:
		0x279b:
		0x279c:
		0x27a0:
		0x27a1:
		0x27cd:
		0x27ce:
		0x27da:
		0x27df:
		0x27e1:
		0x27e2:
		0x27e3:
		0x27e4:
		0x27e5:
		0x27e6:
		0x27e7:
		0x27eb:
		0x27ec:
		0x2fb9:
		0x2fba:
		0x2fc3:
		0x2fc4:
		0x3173:
		0x3174:
		0x3175:
		0x3176:
		0x3177:
		0x3178:
		0x3179:
		0x317a:
		0x4000:
		0x4001:
		0x4002:
		0x4003:
		0x41d8:
		0x41d9:
		0x42de:
		0x42df:
		0x450d:
		0x450e:
		0x457e:
		0x457f:
		0x45a4:
		0x45a5:
		0x45b1:
		0x45b3:
		0x4644:
		0x4645:
		0x46aa:
		0x46ab:
		0x46b4:
		0x46b5:
		0x50d8:
		0x50d9:
			return ITEMTYPE_CLOTHING;			

		default:
			if (item.isa (POLCLASS_ARMOR) )
				return ITEMTYPE_ARMOR;
			elseif (item.isa (POLCLASS_WEAPON))
				return ITEMTYPE_WEAPON;
			endif
			return 0;

	endcase
endfunction




///////////////////
//  applies a damage modifier to the passed item
///////////////////

function DoDmgModifier (item, modifier)

	case (modifier)
		1:
			item.dmg_mod := 1;
			AppendMagicSuffixItem (item, "quality");
		2:
			item.dmg_mod := 2;
			AppendMagicSuffixItem (item, "might");
		3:
			item.dmg_mod := 3;
			AppendMagicSuffixItem (item, "maiming");
		4:
			item.dmg_mod := 4;
			AppendMagicSuffixItem (item, "power");
		5:
			item.dmg_mod := 5;
			AppendMagicSuffixItem (item, "destruction");
		default:
			item.dmg_mod := 0;
//			EraseObjProperty (item, "suffix");
	endcase

endfunction




///////////////////
//  gives the weapon a skill bonus
///////////////////

function DoWeaponSkillModifier (item, modifier)

	var itemcfg := readconfigfile(":*:itemdesc");
	var elem := FindConfigElem (itemcfg, item.objtype);
	var skillname := "";
	var skillnum := -1;

	if ( elem and elem.Attribute )
		if ( elem.Attribute == "Swords" )
			skillname := "swordsmanship";
			skillnum := 40;
		elseif ( elem.Attribute == "Mace" )
			skillname := "macefighting";
			skillnum := 41;
		elseif ( elem.Attribute == "Archery" )
			skillname := "archery";
			skillnum := 31;
		elseif ( elem.Attribute == "Fencing" )
			skillname := "fencing";
			skillnum := 42;
		elseif ( elem.Attribute == "Throwing" )
			skillname := "throwing";
			skillnum := 57;
		else
			return;
		endif
	else
		return;
	endif

	var skillbonus := modifier*SKILLBONUS_LEVEL;

	SetObjProperty ( item, "skilladv", skillnum);
	SetObjProperty ( item, "skill"+skillnum, skillbonus);
	AppendMagicSuffixItem (item, skillname);
endfunction




///////////////////
//  gives the armor a skill bonus
///////////////////

function DoArmorSkillModifier (item, modifier)
	var is_metal_armor := 0;
	if (!IsMedableArmor (item))
		is_metal_armor := 1;
	endif

	var skilllist := array{
		SKILLID_ALCHEMY,
		SKILLID_ANATOMY,
		SKILLID_ANIMALLORE,
		SKILLID_PARRY,
		SKILLID_BLACKSMITHY,
		SKILLID_PEACEMAKING,
		SKILLID_CAMPING,
		SKILLID_CARPENTRY,
		SKILLID_CARTOGRAPHY,
		SKILLID_COOKING,
		SKILLID_DETECTINGHIDDEN,
		SKILLID_TAUNT,
		SKILLID_INVOCATION,
		SKILLID_HEALING,
		SKILLID_FISHING,
		SKILLID_HIDING,
		SKILLID_PROVOCATION,
		SKILLID_INSCRIPTION,
		SKILLID_LOCKPICKING,
		SKILLID_MAGERY,
		SKILLID_MAGICRESISTANCE,
		SKILLID_TACTICS,
		SKILLID_POISONING,
		SKILLID_SPIRITSPEAK,
		SKILLID_STEALING,
		SKILLID_TAILORING,
		SKILLID_TAMING,
		SKILLID_TINKERING,
		SKILLID_TRACKING,
		SKILLID_SWORDSMANSHIP,
		SKILLID_MACEFIGHTING,
		SKILLID_FENCING,
		SKILLID_WRESTLING,
		SKILLID_LUMBERJACKING,
		SKILLID_MINING,
		SKILLID_MEDITATION,
		SKILLID_STEALTH,
		SKILLID_DISCORDANCE,
		SKILLID_THROWING,
		SKILLID_FOCUS
	};

	var skillnamelist := array{
		"alchemy",
		"anatomy",
		"animal lore",
		"parrying",
		"blacksmithy",
		"peacemaking",
		"camping",
		"carpentry",
		"cartography",
		"cooking",
		"detect hidden",
		"leadership",
		"invocation",
		"healing",
		"fishing",
		"hiding",
		"provocation",
		"inscription",
		"lockpicking",
		"magery",
		"magic resistance",
		"tactics",
		"poisoning",
		"necromancy",
		"stealing",
		"tailoring",
		"taming",
		"tinkering",
		"tracking",
		"swordsmanship",
		"macefighting",
		"fencing",
		"wrestling",
		"lumberjacking",
		"mining",
		"meditation",
		"stealth",
		"discordance",
		"throwing",
		"focus"
	};

	if (!IsGargishItem(item))
		skilllist.append (SKILLID_ARCHERY);
		skillnamelist.append ("archery");
	endif
	var skillnum := -1;
	repeat
		skillnum := RandomInt ( len (skilllist) ) + 1;
		if (!ValidSkill (skilllist [skillnum], is_metal_armor))
			skillnum := -1;
		endif
	until (skillnum != -1);

	var skillname := skillnamelist [skillnum];
	var skillbonus := modifier * SKILLBONUS_LEVEL;

	SetObjProperty ( item, "skilladv", skilllist[skillnum]);
	SetObjProperty ( item, "skill"+skilllist[skillnum], skillbonus);
	AppendMagicSuffixItem (item, skillname);
endfunction




///////////////////
//  Determines if the skill is one that can be put on a type of item
///////////////////

function ValidSkill (skillnum, is_metal_armor)
	if (!is_metal_armor)
		return 1;
	endif

	case (skillnum)
		//These can't be used or are virtually useless, so they never spawn
		SKILLID_ANIMALLORE:
		SKILLID_PEACEMAKING:
		SKILLID_DISCORDANCE:
		SKILLID_INVOCATION:
		SKILLID_PROVOCATION:
		SKILLID_MUSICIANSHIP:
		SKILLID_NECROMANCY:
		SKILLID_STEALING:
		SKILLID_MEDITATION:
		SKILLID_STEALTH:
			return 0;
		//These ones aren't terribly useful, but may be to some people.  They'll spawn less often
		SKILLID_ALCHEMY:
		SKILLID_MAGERY:
			if (RandomInt (2))
				return 0;
			endif
	endcase

	return 1;
endfunction




///////////////////
//  gives the shield a skill bonus
///////////////////

function DoShieldSkillModifier (item, modifier)

	SetObjProperty ( item, "skilladv", 5);
	SetObjProperty ( item, "skill"+5, modifier*SKILLBONUS_LEVEL);
	AppendMagicSuffixItem (item, "parry");

endfunction




///////////////////
//  makes the passed weapon silver
///////////////////

function MakeWeaponSilver (item)
	SetObjProperty (item, "silver", 1);
	item.color := 926;
	var prefix := GetObjProperty (item, "prefix");
	if (prefix)
		SetObjProperty (item, "prefix", prefix + " silver" );
	else
		SetObjProperty ( item , "prefix", " silver" );
	endif
endfunction




///////////////////
//  applies a hp modifier to the passed item
///////////////////

function DoHPModifier (item, modifier)

	item.maxhp_mod := 0;

	var hp := 0;
	case (modifier)
		1:
			hp := 10;
			SetObjProperty( item , "prefix", "a fine" );
		2:
			hp := 20;
			SetObjProperty( item , "prefix", "a durable" );
		3:
			hp := 30;
			SetObjProperty( item , "prefix", "a rugged" );
		4:
			hp := 40;
			SetObjProperty( item , "prefix", "a tempered" );
		5:
			hp := 50;
			SetObjProperty( item , "prefix", "an indestructable" );
		default:
			EraseObjProperty( item , "prefix");
			return;
	endcase;

	if (hp)
		var basemod := CINT (item.maxhp/5);
		item.maxhp_mod := hp + basemod;
		item.hp := item.maxhp;
	endif

endfunction




///////////////////
//  applies a armor modifier to the passed item
///////////////////

function DoARModifier (item, modifier)

	case (modifier)
		1:
			item.ar_mod := 2;
			AppendMagicSuffixItem (item, "defense");
		2:
			item.ar_mod := 4;
			AppendMagicSuffixItem (item, "guarding");
		3:
			item.ar_mod := 6;
			AppendMagicSuffixItem (item, "protection");
		4:
			item.ar_mod := 8;
			AppendMagicSuffixItem (item, "fortification");
		5:
			item.ar_mod := 10;
			AppendMagicSuffixItem (item, "invulnerability");
		default:
			item.ar_mod := 0;
	endcase

endfunction




///////////////////
//  applies a skill mod to the musical intrument passed
///////////////////

function DoMusicalInstrumentSkillMod (item, modifier)

	case (modifier)
		1:
			SetObjProperty (item, "skillbonus", 2);
			AppendMagicSuffixItem (item, "Sonority");
		2:
			SetObjProperty (item, "skillbonus", 4);
			AppendMagicSuffixItem (item, "Accord");
		3:
			SetObjProperty (item, "skillbonus", 6);
			AppendMagicSuffixItem (item, "Melody");
		4:
			SetObjProperty (item, "skillbonus", 8);
			AppendMagicSuffixItem (item, "Harmony");
		5:
			SetObjProperty (item, "skillbonus", 10);
			AppendMagicSuffixItem (item, "Consonance");
		default:
			EraseObjProperty (item, "skillbonus");
	endcase

endfunction




///////////////////
//  Adds an extra suffix item to the item's description
///////////////////

function AppendMagicSuffixItem (item, newsuffix, isspellname := 0)
	var suffix := GetObjProperty (item, "suffix");
	if (suffix)
		SetObjProperty (item, "suffix", suffix + " and " + newsuffix);
	else
		SetObjProperty (item, "suffix", " of " + newsuffix);
	endif
	if (isspellname)
		SetObjProperty (item, "itemspellname", newsuffix);
	endif
endfunction



