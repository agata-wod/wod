function AllocLockId()
  var lockid := GetGlobalProperty( "nextlockid" );
  if (!lockid)
    lockid := 1;
  endif
  SetGlobalProperty( "nextlockid", lockid+1 );
  return lockid;
endfunction 
