///////////////////
//  determines if the NPC template passed is one of an undead creature
///////////////////

function IsAnUndeadCreature (npctemplate)
	var NpcElem := GetNpcTemplateElem (npctemplate);
	if (CINT (NpcElem.undead))
		return CINT(NpcElem.undead);
	endif

	return 0;
endfunction




///////////////////
//  determines if the NPC is one that doesn't leave a corpse behind
///////////////////

function HasNoCorpse (npctemplate)

	var NpcElem := GetNpcTemplateElem ( npctemplate );
	if (NpcElem.nocorpse)
		return 1;
	endif

	return 0;

endfunction




///////////////////
//  good npcs use this function to determine what they should attack
///////////////////

function IsHostile (byref mob)
	if (mob.acctname)
		return 0;
	elseif (mob.script ["tamed"])
		return 0;
	endif

	var NpcElem := GetNpcTemplateElem ( mob.npctemplate );
	if (NpcElem.hostile)
		return 1;
	endif

	return 0;
endfunction




///////////////////
//  aggressive npcs use this function to determine what they should attack
///////////////////

function IsGood (byref mob)
	if (mob.acctname)
		if (mob.cmdlevel)
			if (GetObjProperty (mob, "attackable"))
				return 1;
			else
				return 0;
			endif
		endif
		return 1;
	elseif (mob.script["guard"])
		return 1;
	endif
	
	if (mob.script ["tamed"])
		var masterserial1 := GetObjProperty (mob, "master");
		if (!masterserial1)
			return 1;
		endif
		var master1 := SystemFindObjectBySerial (masterserial1);
		if (!master1 or !master1.cmdlevel)
			return 1;
		endif

		//gms, check the attackable cprop
		if (GetObjProperty (master1, "attackable"))
			return 1;
		endif
		return 0;
	endif

	if ( mob.script == "humuc" or mob.script["playermerchant"] )
		return 0;
	endif

	var NpcElem := GetNpcTemplateElem (mob.npctemplate);
	if (NpcElem.alignment == "good")
		return 1;
	endif

	return 0;
endfunction




///////////////////
//  finds the NPC's entry in npcdesc (useful for putting NPCs in packages)
///////////////////

function GetNpcTemplateElem (npctemplate)
	if ( npctemplate[":"] )
		npctemplate[":"] := "";
		npctemplate := npctemplate[(find(npctemplate, ":", 1)+1), len(npctemplate)];
	endif
	var cfg := ReadConfigFile (":*:npcdesc");
	return FindConfigElem (cfg, npctemplate);
endfunction




///////////////////
//  trims off the extra stuff to find the basic NPCtemplate (usedful for putting NPCs in packages)
///////////////////

function TrimTemplate(npctemplate)
	if ( npctemplate[":"] )
		npctemplate[":"] := "";
		npctemplate := npctemplate[(find(npctemplate, ":", 1)+1), len(npctemplate)];
	endif
	return npctemplate;
endfunction




///////////////////
//  figures out what npcdesc file the NPC is found in (useful for putting NPCs in packages)
///////////////////

function FindNpcDesc (npctemplate)
	if ( npctemplate[":"] )
		var npcdesc := npctemplate[1,find(npctemplate, ":", 2)];
		return npcdesc + "npcdesc";
	else
		return "::npcdesc";
	endif
endfunction




///////////////////
//  figures out what package the NPC is in
///////////////////

function FindNPCPackage (template)
	template := TrimTemplate (template);
	var cfg := ReadConfigFile ("::npcdesc");
	if (FindConfigElem (cfg, template))
		return "none";
	endif

	foreach pkgname in (polcore().packages)
		cfg := ReadConfigFile (":" + pkgname + ":npcdesc");
		if (cfg)
			if (FindConfigElem (cfg, template))
				return CSTR (pkgname);
			endif
		endif
	endforeach

	return 0;
endfunction
