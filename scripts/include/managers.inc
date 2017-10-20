use os;
use uo;
use cfgfile;

include "include/constants";


function ManagerAnswer( process, answer )

	var ev	:= array;
	ev[1]	:= EVID_MANAGER_ANSWER;
	ev[2]	:= answer;

	process.SendEvent(ev);

	return 1;

endfunction


function SendAuctionEvent( event_type, uobject := 0, mobile := 0, bid := 0, description := error )

	var pid := GetGlobalProperty(PROPID_GLOBAL_AUCTION_MANAGER);
	if( !pid )
		var ret		:= error;
		ret.errortext	:= "Auction manager's pid not found";
		return ret;
	endif

	var manager := getprocess(pid);
	if( !manager )
		var ret		:= error;
		ret.errortext	:= "Auction manager not found";
		return ret;
	endif

	var ev	:= array;
	ev[1]	:= event_type;
	ev[2]	:= getprocess(getpid());

	case( event_type )
		EVID_AUCTION_ADD_ITEM:		ev[3] := uobject;
						ev[4] := mobile;
						ev[5] := bid;
						if( description != error )
							ev[6] := CStr(description);
						else
							ev[6] := "";
						endif
					
		EVID_AUCTION_ADD_PLAYER:	ev[3] := uobject;
	
		EVID_AUCTION_BID_ON_ITEM:	ev[3] := uobject;
						ev[4] := mobile;
						ev[5] := bid;
					
		EVID_AUCTION_REMOVE_PLAYER:	ev[3] := uobject;
		EVID_AUCTION_SELL_ITEM:		ev[3] := uobject;
	endcase


	manager.SendEvent(ev);
	
	ev := wait_for_event(120);
	if( ev[1] == EVID_MANAGER_ANSWER )
		return ev[2];
	endif

	var ret		:= error;
	ret.errortext	:= "Auction manager didn't answer";
	return ret;

endfunction


function SendBookCaseEvent( event_type, bookcase := 0 )

	var pid := GetGlobalProperty(PROPID_GLOBAL_SPAWNBOOKCASES_MANAGER);
	if( !pid )
		var ret		:= error;
		ret.errortext	:= "Bookcases manager's pid not found";
		return ret;
	endif

	var manager := getprocess(pid);
	if( !manager )
		var ret		:= error;
		ret.errortext	:= "Bookcases manager not found";
		return ret;
	endif

	var ev	:= array;
	ev[1]	:= event_type;
	ev[2]	:= getprocess(getpid());
	case( event_type )
		EVID_BOOKCASE_CREATED:
			ev[3] := bookcase;
		
		EVID_BOOKCASE_DESTROYED:
			ev[3] := bookcase;
	endcase

	manager.SendEvent(ev);
	
	ev := wait_for_event(120);
	if( ev[1] == EVID_MANAGER_ANSWER )
		return ev[2];
	endif

	var ret		:= error;
	ret.errortext	:= "Bookcases manager didn't answer";
	return ret;

endfunction


function SendChestEvent( event_type, chest := 0 )

	var pid := GetGlobalProperty(PROPID_GLOBAL_SPAWNCHESTS_MANAGER);
	if( !pid )
		var ret		:= error;
		ret.errortext	:= "Chests manager's pid not found";
		return ret;
	endif

	var manager := getprocess(pid);
	if( !manager )
		var ret		:= error;
		ret.errortext	:= "Chests manager not found";
		return ret;
	endif

	var ev	:= array;
	ev[1]	:= event_type;
	ev[2]	:= getprocess(getpid());
	case( event_type )
		EVID_CHEST_CREATED:
			ev[3] := chest;
		
		EVID_CHEST_DESTROYED:
			ev[3] := chest;
	endcase

	manager.SendEvent(ev);
	
	ev := wait_for_event(120);
	if( ev[1] == EVID_MANAGER_ANSWER )
		return ev[2];
	endif

	var ret		:= error;
	ret.errortext	:= "Chests manager didn't answer";
	return ret;

endfunction


function SendListsManagerEvent( event_type, mobile := 0 )

	var pid := GetGlobalProperty(PROPID_GLOBAL_LISTS_MANAGER);
	if( !pid )
		var ret		:= error;
		ret.errortext	:= "Lists manager's pid not found";
		return ret;
	endif

	var manager := getprocess(pid);
	if( !manager )
		var ret		:= error;
		ret.errortext	:= "Lists manager not found";
		return ret;
	endif

	var ev	:= array;
	ev[1]	:= event_type;
	ev[2]	:= getprocess(getpid());
	case( event_type )
		EVID_LISTS_ADD_TO_AUCTION_LIST:
		EVID_LISTS_ADD_TO_MSG_LIST:
		EVID_LISTS_ADD_TO_STAFF_LIST:
		EVID_LISTS_REMOVE_FROM_AUCTION_LIST:
		EVID_LISTS_REMOVE_FROM_MSG_LIST:
		EVID_LISTS_REMOVE_FROM_STAFF_LIST:
			ev[3] := mobile;
	endcase

	manager.SendEvent(ev);
	
	ev := wait_for_event(120);
	if( ev[1] == EVID_MANAGER_ANSWER )
		return ev[2];
	endif

	var ret		:= error;
	ret.errortext	:= "Lists manager didn't answer";
	return ret;

endfunction


function SendSpawnPointEvent( event_type, point := 0, serial := 0 )

	var pid := GetGlobalProperty(PROPID_GLOBAL_SPAWNPOINTS_MANAGER);
	if( !pid )
		var ret		:= error;
		ret.errortext	:= "Spawnpoint manager's pid not found";
		return ret;
	endif

	var manager := getprocess(pid);
	if( !manager )
		var ret		:= error;
		ret.errortext	:= "Spawnpoint manager not found";
		return ret;
	endif

	var ev	:= array;
	ev[1]	:= event_type;
	ev[2]	:= getprocess(getpid());
	case( event_type )
		EVID_SPAWNPOINT_CHECK_POINT:
			ev[3] := point;
		
		EVID_SPAWNPOINT_CREATED:
			ev[3] := point;
		
		EVID_SPAWNPOINT_DEATH:
			ev[3] := SystemFindObjectBySerial(CInt(point));
			ev[4] := serial;
			manager.SendEvent(ev);
			return 1;
		
		EVID_SPAWNPOINT_DESTROYED:
			ev[3] := point;
		
		EVID_SPAWNPOINT_RESETED:
			ev[3] := point;
	endcase

	manager.SendEvent(ev);
	
	ev := wait_for_event(240);
	if( ev[1] == EVID_MANAGER_ANSWER )
		return ev[2];
	endif

	var ret		:= error;
	ret.errortext	:= "Spawnpoint manager didn't answer";
	return ret;

endfunction


function WarnManagerOfNpcDeath( corpse )

	var pointserial := GetObjProperty( corpse, PROPID_MOBILE_SPAWNPOINT_SERIAL );
	if( !pointserial )
		return 0;
	endif

	var point := SystemFindObjectBySerial(pointserial);
	if( !point )
		return 0;
	endif

	var mobile_serial := GetObjProperty( corpse, PROPID_MOBILE_SELF_SERIAL );
	if( !mobile_serial )
		return 0;
	endif

	return start_script( ":spawnpoint:spawndeath", {point,mobile_serial} );
	//return SendSpawnPointEvent( EVID_SPAWNPOINT_DEATH, pointserial, mobile_serial );

endfunction