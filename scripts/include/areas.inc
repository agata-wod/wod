use uo;
use os;
use basic;
use cfgfile;

//include "include/multiutil";
include "include/constants/cfgfiles";
include "include/constants/propids";

const AREAID_BRITAIN		:= "Britain";
const AREAID_BRITISH_CASTLE	:= "British Castle";
const AREAID_BLACKTHORN_CASTLE	:= "Blackthorn Castle";
const AREAID_BUCCANEERS_DEN	:= "Buccaneer's Den";
const AREAID_COVE		:= "Cove";
const AREAID_DELUCIA		:= "Delucia";
const AREAID_JHELOM		:= "Jhelom";
const AREAID_MAGINCIA		:= "Magincia";
const AREAID_MINOC		:= "Minoc";
const AREAID_MOONGLOW		:= "Moonglow";
const AREAID_NUJELM		:= "Nujel'm";
const AREAID_OCLLO		:= "Ocllo";
const AREAID_PAPUA		:= "Papua";
const AREAID_SERPENTS_HOLD	:= "Serpent's Hold";
const AREAID_SKARA_BRAE		:= "Skara Brae";
const AREAID_TRINSIC		:= "Trinsic";
const AREAID_YEW		:= "Yew";
const AREAID_CRAFTMINE		:= "Craft Mine";



function CheckPoz( byref x, byref y )

	// Arena
	if( x >= 1385 )
		if( x <= 1414 )
			if( y >= 3729 )
				if( y <= 3758 )
					return 0;
				endif
			endif
		endif
	
		// Wind
		if( x >= 5120 )
			if( x <= 5374 )
				if( y >= 0 )
					if( y <= 254 )
						return 0;
					endif
				endif
			endif
		
			// Jail
			if( x >= 5272 )
				if( x <= 5310 )
					if( y >= 1160 )
						if( y <= 1190 )
							return 0;
						endif
					endif
				endif
			
				// Newbie Dungeon
				if( x >= 2557 )
					if( x <= 2635 )
						if( y >= 470 )
							if( y <= 626 )
								return 0;
							endif
						endif
					endif
				
					// Race Gates
					if( x >= 5536 )
						if( x <= 5570 )
							if( y >= 1154 )
								if( y <= 1183 )
									return 0;
								endif
							endif
						endif
					endif
				endif
			endif
		endif
	endif

	return 1;

endfunction


function IsForbiddenAreaFor( who, x, y )

	if( who.cmdlevel )
		return 0;
	endif

	var old_prio	:= set_priority(100);
	var disabled	:= GetGlobalProperty( PROPID_GLOBAL_AREAS_FORBIDDEN );
	if( !disabled )
		disabled := {};
	endif

	var banned := GetObjProperty( who, PROPID_MOBILE_AREAS_BANNED );
	if( !banned )
		if( !len(disabled) )
			set_priority(old_prio);
			return 0;
		else
			banned := {};
		endif
	endif

	var areas_cfg	:= ReadConfigFile( CFGFILE_AREAS );
	var areas	:= GetConfigStringArray( areas_cfg[1], "Area" );
	if(!areas)
		set_priority(old_prio);
		return 0;
	endif

	var allowed := GetObjProperty( who, PROPID_MOBILE_AREAS_ALLOWED_FORBIDDEN );
	if( !allowed )
		allowed := {};
	endif

	var i;
	var splited;
	var where := struct;
	where.+x  := x;
	where.+y  := y;
	for( i := 1; i <= areas.size(); i := i + 1 )
		if( banned[i] )
			splited := SplitWords( areas[i] );
			if( IsInBox( where, CInt(splited[1]), CInt(splited[2]), CInt(splited[3]), CInt(splited[4]) ) )
				set_priority(old_prio);
				return 1;
			endif
		elseif( disabled[i] )
			if( !allowed[i] )
				splited := SplitWords( areas[i] );
				if( IsInBox( where, CInt(splited[1]), CInt(splited[2]), CInt(splited[3]), CInt(splited[4]) ) )
					set_priority(old_prio);
					return 1;
				endif
			endif
		endif
	endfor

	set_priority(old_prio);

	return 0;

endfunction


function IsForbiddenAreaToGateFrom( byref who )

	return IsForbiddenAreaToRecallFrom( who );

endfunction


function IsForbiddenAreaToGateTo( byref who, byref x, byref y )

	return IsForbiddenAreaToRecallTo( who, x, y );

endfunction


function IsForbiddenAreaToRecallFrom( byref who )

	if( who.cmdlevel )
		return 0;
	endif

	var old_prio := set_priority(100);
	if( !CheckPoz( who.x, who.y ) )
		set_priority(old_prio);
		return 1;
	endif

	set_priority(old_prio);
	return 0;

endfunction


function IsForbiddenAreaToRecallTo( byref who, byref x, byref y )

	if( who.cmdlevel )
		return 0;
	endif

	var old_prio := set_priority(100);

	if( !CheckPoz( x, y ) )
		set_priority(old_prio);
		return 1;
	endif
/*
	var multi := GetMultiAtLocation( x, y );
	if( multi )
		if( !IsWelcomeInHouse( who, multi ) )
			set_priority(old_prio);
			return 1;
		endif
	endif
*/
	var disabled_recall := GetGlobalProperty( PROPID_GLOBAL_AREAS_NORECALL );
	if( !disabled_recall )
		disabled_recall := {};
	endif

	var forbidden	:= GetGlobalProperty( PROPID_GLOBAL_AREAS_FORBIDDEN );
	if( !forbidden )
		forbidden := {};
	endif

	var banned := GetObjProperty( who, PROPID_MOBILE_AREAS_BANNED );
	if( !banned )
		if( !disabled_recall.size() )
			if( !forbidden.size() )
				set_priority(old_prio);
				return 0;
			endif
		endif
		banned := {};
	endif

	var areas_cfg	:= ReadConfigFile( CFGFILE_AREAS );
	var areas	:= GetConfigStringArray( areas_cfg[1], "Area" );
	if( !areas )
		set_priority(old_prio);
		return 0;
	endif

	var allowed_recall := GetObjProperty( who, PROPID_MOBILE_AREAS_ALLOWED_RECALLING );
	if( !allowed_recall )
		allowed_recall := {};
	endif

	var allowed_forbidden := GetObjProperty( who, PROPID_MOBILE_AREAS_ALLOWED_FORBIDDEN );
	if( !allowed_forbidden )
		allowed_forbidden := {};
	endif
	
	var object	:= struct;
	object.+x	:= x;
	object.+y	:= y;

	var i;
	var splited;
	for( i := 1; i <= areas.size(); i := i + 1 )
		if( banned[i] )
			splited := SplitWords( areas[i] );
			if( IsInBox( object, CInt(splited[1]), CInt(splited[2]), CInt(splited[3]), CInt(splited[4]) ) )
				set_priority(old_prio);
				return 1;
			endif
		elseif( disabled_recall[i] )
			if( !allowed_recall[i] )
				splited := SplitWords( areas[i] );
				if( IsInBox( object, CInt(splited[1]), CInt(splited[2]), CInt(splited[3]), CInt(splited[4]) ) )
					set_priority(old_prio);
					return 1;
				endif
			endif
		elseif( forbidden[i] )
			if( !allowed_forbidden[i] )
				splited := SplitWords( areas[i] );
				if( IsInBox( object, CInt(splited[1]), CInt(splited[2]), CInt(splited[3]), CInt(splited[4]) ) )
					set_priority(old_prio);
					return 1;
				endif
			endif
		endif
	endfor

	set_priority(old_prio);
	return 0;

endfunction


function IsInACity( byref what )
			// 		min_x	max_x	min_y	max_y	name
	var cities := {		{	1361,	1589,	1536,	1765,	AREAID_BRITAIN		},	// Britain
				{	1295,	1360,	1558,	1765,	AREAID_BRITISH_CASTLE	},	// British Castle
				{	1500,	1545,	1400,	1490,	AREAID_BLACKTHORN_CASTLE},	// Blackthorn Castle
				{	2592,	2783,	2080,	2271,	AREAID_BUCCANEERS_DEN	},	// Buccanner's Den
				{	2207,	2292,	1114,	1241,	AREAID_COVE		},	// Cove
				{	5151,	5312,	3930,	4059,	AREAID_DELUCIA		},	// Delucia
				{	1292,	1508,	3667,	3883,	AREAID_JHELOM		},	// Jhelom
				{	3647,	3837,	2041,	2280,	AREAID_MAGINCIA		},	// Magincia
				{	2385,	2635,	376,	626,	AREAID_MINOC		},	// Minoc
				{	4371,	4493,	1023,	1232,	AREAID_MOONGLOW		},	// Moonglow
				{	3487,	3817,	1067,	1397,	AREAID_NUJELM		},	// Nujel'm
				{	3591,	3761,	2441,	2696,	AREAID_OCLLO		},	// Ocllo
				{	5652,	5840,	3116,	3309,	AREAID_PAPUA		},	// Papua
				{	2865,	3069,	3334,	3582,	AREAID_SERPENTS_HOLD	},	// Serpent's Hold
				{	534,	692,	2111,	2269,	AREAID_SKARA_BRAE	},	// Skara Brae
				{	1791,	2160,	2639,	2905,	AREAID_TRINSIC		},	// Trinsic
				{	494,	600,	927,	1033,	AREAID_YEW		}};	// Yew
	
	foreach city in cities
		if( IsInBox( what, city[1], city[2], city[3], city[4] ) )
			return city[5];
		endif
	endforeach

	return 0;

endfunction


function IsInAntiLootingArea( byref who )

	if( who.cmdlevel )
		return 0;
	endif

	var old_prio	:= set_priority(100);
	var disabled	:= GetGlobalProperty( PROPID_GLOBAL_AREAS_NOLOOTING );
	if( !disabled )
		disabled := {};
	endif

	var banned := GetObjProperty( who, PROPID_MOBILE_AREAS_BANNED );
	if( !banned )
		if( !len(disabled) )
			set_priority(old_prio);
			return 0;
		endif
		banned	:= {};
	endif

	var areas_cfg	:= ReadConfigFile( CFGFILE_AREAS );
	var areas	:= GetConfigStringArray( areas_cfg[1], "Area" );
	if( !areas )
		set_priority(old_prio);
		return 0;
	endif

	var allowed := GetObjProperty( who, PROPID_MOBILE_AREAS_ALLOWED_LOOTING );
	if( !allowed )
		allowed := {};
	endif

	var i;
	var splited;
	for( i := 1; i <= areas.size(); i := i + 1 )
		if( banned[i] )
			splited := SplitWords( areas[i] );
			if( IsInBox( who, CInt(splited[1]), CInt(splited[2]), CInt(splited[3]), CInt(splited[4]) ) )
				set_priority(old_prio);
				return 1;
			endif
		elseif( disabled[i] )
			if( !allowed[i] )
				splited := SplitWords( areas[i] );
				if( IsInBox( who, CInt(splited[1]), CInt(splited[2]), CInt(splited[3]), CInt(splited[4]) ) )
					set_priority(old_prio);
					return 1;
				endif
			endif
		endif
	endfor

	set_priority(old_prio);

	return 0;

endfunction


function IsInAntiMagicArea( byref who )

	if( who.cmdlevel )
		return 0;
	endif

	var old_prio	:= set_priority(100);
	var areas_cfg	:= ReadConfigFile( CFGFILE_AREAS );
	var areas	:= GetConfigStringArray( areas_cfg[1], "Area" );
	if( !areas )
		set_priority(old_prio);
		return 0;
	endif

	var disabled	:= GetGlobalProperty( PROPID_GLOBAL_AREAS_ANTIMAGIC );
	if( !disabled )
		set_priority(old_prio);
		return 0;
	endif

	var allowed	:= GetObjProperty( who, PROPID_MOBILE_AREAS_ALLOWED_ANTIMAGIC );
	if( !allowed )
		allowed := {};
	endif

	var i;
	var splited;
	for( i := 1; i <= areas.size(); i := i + 1 )
		if( disabled[i] )
			if( !allowed[i] )
				splited := SplitWords( areas[i] );
				if( IsInBox( who, CInt(splited[1]), CInt(splited[2]), CInt(splited[3]), CInt(splited[4]) ) )
					set_priority(old_prio);
					return 1;
				endif
			endif
		endif
	endfor

	set_priority(old_prio);

	return 0;

endfunction


function IsInAProtectedCity( byref what )
			// 		min_x	max_x	min_y	max_y	name
	var p_cities := {	{	1361,	1589,	1536,	1765,	AREAID_BRITAIN		},	// Britain
				{	1295,	1360,	1558,	1765,	AREAID_BRITISH_CASTLE	},	// British Castle
				{	1500,	1545,	1400,	1490,	AREAID_BLACKTHORN_CASTLE},	// Blackthorn Castle
				{	2207,	2292,	1114,	1241,	AREAID_COVE		},	// Cove
				{	1292,	1508,	3667,	3883,	AREAID_JHELOM		},	// Jhelom
				{	3647,	3837,	2041,	2280,	AREAID_MAGINCIA		},	// Magincia
				{	2385,	2635,	376,	626,	AREAID_MINOC		},	// Minoc
				{	4371,	4493,	1023,	1232,	AREAID_MOONGLOW		},	// Moonglow
				{	3487,	3817,	1067,	1397,	AREAID_NUJELM		},	// Nujel'm
				{	3591,	3761,	2441,	2696,	AREAID_OCLLO		},	// Ocllo
				{	534,	692,	2111,	2269,	AREAID_SKARA_BRAE	},	// Skara Brae
				{	1791,	2160,	2639,	2905,	AREAID_TRINSIC		},	// Trinsic
				{	494,	600,	927,	1033,	AREAID_YEW		}};	// Yew
	
	foreach protected_city in p_cities
		if( IsInBox( what, protected_city[1], protected_city[2], protected_city[3], protected_city[4] ) )
			return protected_city[5];
		endif
	endforeach

	return 0;

endfunction


function IsInArea( byref what, byref area )

	case( area )				//		      x1    x2    y1    y2
		AREAID_BRITAIN:			return IsInBox( what, 1361, 1589, 1536, 1765 );
		AREAID_BRITISH_CASTLE:		return IsInBox( what, 1295, 1360, 1558, 1765 );
		AREAID_BLACKTHORN_CASTLE:	return IsInBox( what, 1500, 1545, 1400, 1490 );
		AREAID_BUCCANEERS_DEN:		return IsInBox( what, 2592, 2783, 2080, 2271 );
		AREAID_COVE:			return IsInBox( what, 2207, 2292, 1114, 1241 );
		AREAID_DELUCIA:			return IsInBox( what, 5151, 5312, 3930, 4059 );
		AREAID_JHELOM:			return IsInBox( what, 1292, 1508, 3667, 3883 );
		AREAID_MAGINCIA:		return IsInBox( what, 3647, 3837, 2041, 2280 );
		AREAID_MINOC:			return IsInBox( what, 2385, 2635, 376, 626  );
		AREAID_MOONGLOW:		return IsInBox( what, 4371, 4493, 1023, 1232 );
		AREAID_NUJELM:			return IsInBox( what, 3487, 3817, 1067, 1397 );
		AREAID_OCLLO:			return IsInBox( what, 3591, 3761, 2441, 2696 );
		AREAID_PAPUA:			return IsInBox( what, 5652, 5840, 3116, 3309 );
		AREAID_SERPENTS_HOLD:		return IsInBox( what, 2865, 3069, 3334, 3582 );
		AREAID_SKARA_BRAE:		return IsInBox( what, 534, 692, 2111, 2269 );
		AREAID_TRINSIC:			return IsInBox( what, 1791, 2160, 2639, 2905 );
		AREAID_YEW:			return IsInBox( what, 494, 600, 927, 1033 );
		AREAID_CRAFTMINE:		return IsInBox( what, 5189,5273, 1941, 2001 );
	endcase

	return 0;

endfunction


function IsInBox( byref object, byref x1, byref x2, byref y1, byref y2 )

	if( object.x >= x1 )
		if( object.x <= x2 )
			if( object.y >= y1 )
				if( object.y <= y2 )
					return 1;
				endif
			endif
		endif
	endif

	return 0;

endfunction


function IsInForbiddenArea( who )

	if( who.cmdlevel )
		return 0;
	endif

	var old_prio	:= set_priority(100);
	var disabled	:= GetGlobalProperty( PROPID_GLOBAL_AREAS_FORBIDDEN );
	if( !disabled )
		disabled := {};
	endif

	var banned := GetObjProperty( who, PROPID_MOBILE_AREAS_BANNED );
	if( !banned )
		if( !len(disabled) )
			set_priority(old_prio);
			return 0;
		else
			banned := {};
		endif
	endif

	var areas_cfg	:= ReadConfigFile( CFGFILE_AREAS );
	var areas	:= GetConfigStringArray( areas_cfg[1], "Area" );
	if(!areas)
		set_priority(old_prio);
		return 0;
	endif

	var allowed := GetObjProperty( who, PROPID_MOBILE_AREAS_ALLOWED_FORBIDDEN );
	if( !allowed )
		allowed := {};
	endif

	var i;
	var splited;
	for( i := 1; i <= areas.size(); i := i + 1 )
		if( banned[i] )
			splited := SplitWords( areas[i] );
			if( IsInBox( who, CInt(splited[1]), CInt(splited[2]), CInt(splited[3]), CInt(splited[4]) ) )
				set_priority(old_prio);
				return 1;
			endif
		elseif( disabled[i] )
			if( !allowed[i] )
				splited := SplitWords( areas[i] );
				if( IsInBox( who, CInt(splited[1]), CInt(splited[2]), CInt(splited[3]), CInt(splited[4]) ) )
					set_priority(old_prio);
					return 1;
				endif
			endif
		endif
	endfor

	set_priority(old_prio);

	return 0;

endfunction


function IsInGuardedArea( byref who )

	if( who.cmdlevel )
		return 1;
	endif

	var i;
	var splited;
	var old_prio	:= set_priority(100);
	var areas_cfg	:= ReadConfigFile(CFGFILE_AREAS);
	var areas	:= GetConfigStringArray( areas_cfg[2], "Area" );
	if( areas )
		for( i := 1; i <= areas.size(); i := i + 1 )
			splited := SplitWords( areas[i] );
			if( IsInBox( who, CInt(splited[1]), CInt(splited[2]), CInt(splited[3]), CInt(splited[4]) ) )
				set_priority(old_prio);
				return 0;
			endif
		endfor
	endif

	var guarded := GetGlobalProperty(PROPID_GLOBAL_AREAS_GUARDED);
	if( !guarded )
		set_priority(old_prio);
		return 0;
	endif

	areas := GetConfigStringArray( areas_cfg[1], "Area" );
	if( !areas )
		set_priority(old_prio);
		return 0;
	endif

	for( i := 1; i <= areas.size(); i := i + 1 )
		if( guarded[i] )
			splited := SplitWords( areas[i] );
			if( IsInBox( who, CInt(splited[1]), CInt(splited[2]), CInt(splited[3]), CInt(splited[4]) ) )
				set_priority(old_prio);
				return 1;
			endif
		endif
	endfor

	set_priority(old_prio);
	return 0;

endfunction