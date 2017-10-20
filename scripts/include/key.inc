/*
 * key.inc - key functions
 */
include "include/client";
include "include/objtype";

var KEYTYPES := array{ UOBJ_COPPER_KEY,
									UOBJ_GOLD_KEY,
									UOBJ_IRON_KEY,
									UOBJ_MAGIC_KEY,
									UOBJ_RUSTY_IRON_KEY };


// does 'who' have a key matching this lockid?
function FindKey(who, what)

	var lockid := GetObjProperty( what, "lockid" );

	if (!lockid) // not keyed
		return 1;
	endif

	foreach item in EnumerateItemsInContainer( who.backpack )
		if (item.graphic in KEYTYPES)
			if (GetObjProperty( item, "lockid" ) == lockid)
				return item;
			endif
		endif
	endforeach

	return 0;

endfunction