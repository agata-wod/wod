///////////////////
//
//  Assorted time-related functions
//  The date stuff is based on DateUtils Include file by Jouni Airaksinen, Mintus@Codefield.com
//
///////////////////

use os;
use uo;
use cfgfile;

Const SECONDS_PER_DAY := 86400;


///////////////////
//  returns the in-game time
///////////////////

function GetTimeofDay()
	var hours := CSTR (GetHour());
	if (len(hours) == 1)
		hours := "0" + hours;
	endif

	var minutes := CSTR (GetMinute());
	if (len(minutes) == 1)
		minutes := "0" + minutes;
	endif
	return (hours+":"+minutes);
endfunction


function GetTime ()
	var dayscfg := ReadConfigFile ("::days");
	if (!dayscfg)
		Syslog ("Unable to read days.cfg!");
		return 0;
	endif

	var elem := FindConfigElem (dayscfg, "WholeDay");
	var length := GetConfigInt (elem, "Length");
	var day_length := (60 * length); 

	var x := readgameclock();
	return (CINT (x%day_length));
endfunction

function GetTransition()
	var cfg := ReadConfigFile ( "::days" );
	var elem := FindConfigElem ( cfg , "Transition" );
	var length := GetConfigInt ( elem , "Length" );
	return length;
endfunction

function GetDayLength()
	var cfg := ReadConfigFile( "::days" );
	var elem := FindConfigElem( cfg , "WholeDay" );
	var length := GetConfigInt( elem , "Length" );
	return length;
endfunction

function GetHour()
	var time_sec := GetTime() * 24;
	var length := GetDayLength() * 60;
	var hours := CINT (time_sec / length);
	return hours;
endfunction

function GetMinute()
	var time_sec := GetTime();
	var length := (GetDayLength() * 60) * 1.0 ;

	var temp := length / time_sec;
	var raw_time := 24.0 / temp;

	var hours := cint(raw_time);
	var remainder := raw_time - hours;
	var minutes := CINT ( remainder * 60 );
	return minutes;
endfunction




///////////////////
//  Figures out what the current year is (real-world time)
///////////////////

function Systime_GetYear ()
	//Seconds since 1/1/1970
	var systime := polcore().systime;
	//Lets skip ahead to y2k
	systime := systime - 946684800;
	var current_year_counter := 0;
	var done := 0;
	repeat
		if (current_year_counter%4 == 0)
			systime := systime - 31622400;
		else
			systime := systime - 31536000;
		endif
		if (systime >= 0)
			current_year_counter := current_year_counter + 1;
		else
			done:= 1;
		endif
	until (done);
	return (2000 + current_year_counter);	
endfunction





///////////////////
//  Figures out what the current month is (by name) (real-world time)
///////////////////

function Systime_GetMonth ()
	var MonthDays := array { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

	var MonthNames := array {
		"January",
		"February",
		"March",
		"April",
		"May",
		"June",
		"July",
		"August",
		"September",
		"October",
		"November",
		"December"
	};

	//Seconds since 1/1/1970
	var systime := polcore().systime;
	//Lets skip ahead to y2k
	systime := systime - 946684800;

	//convert to days
	var days := CINT (systime / SECONDS_PER_DAY);
	
	var current_year_counter := 0;
	var current_month_counter := 1;
	var done := 0;
	var days_in_month := 0;
	repeat
		if (current_month_counter == 2)
			if (current_year_counter%4 == 0)
				days_in_month := 29;
			else
				days_in_month := 28;
			endif
		else
			days_in_month := MonthDays[current_month_counter];
		endif
		
		if (days_in_month < days)
			days := days - days_in_month;
			current_month_counter := current_month_counter + 1;
			if (current_month_counter > 12)
				current_month_counter := 1;
				current_year_counter := current_year_counter + 1;
			endif
		else
			done := 1;
		endif
	until (done);

	return MonthNames [current_month_counter];
endfunction





///////////////////
//  Figures out what the current date is (real-world time) 
///////////////////

function Systime_GetDate ()
	var MonthDays := array { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

	//Seconds since 1/1/1970
	var systime := polcore().systime;
	//Lets skip ahead to y2k
	systime := systime - 946684800;

	//convert to days
	var days := CINT (systime / SECONDS_PER_DAY) + 1;

	var current_year_counter := 0;
	var current_month_counter := 1;
	var done := 0;
	repeat
		if (current_month_counter == 2)
			if (current_year_counter%4 == 0)
				if (days > 29)
					days := days - 29;
				else
					return days;
				endif
			else
				if (days > 28)
					days := days - 28;
				else
					return days;
				endif
			endif
		else
			if (days > 28)
				days := days - MonthDays[current_month_counter];
			else
				return days;
			endif
		endif

		if (days > 0)
			current_month_counter := current_month_counter + 1;
			if (current_month_counter > 12)
				current_month_counter := 1;
				current_year_counter := current_year_counter + 1;
			endif
		else
			done := 1;
		endif
	until (done);

	return days;
endfunction




///////////////////
//  Figures out what the current hour is, military time (real-world time) 
///////////////////

function Systime_GetHour ()
	var systime := polcore().systime;
	systime := CINT (systime % SECONDS_PER_DAY);

	var hour := CINT (systime / 3600);
	return hour;
endfunction




///////////////////
//  Figures out what the current minute is (real-world time) 
///////////////////

function Systime_GetMinute ()
	var systime := polcore().systime;
	systime := CINT (systime % 3600);

	var minute := CINT (systime / 60);
	return minute;
endfunction




///////////////////
//  This function returns a string in the format "month date, year hour:minutes"
///////////////////

function Systime_GetReadableTimeString ()
	var MonthDays := array { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

	var MonthNames := array {
		"Jan",
		"Feb",
		"Mar",
		"Apr",
		"May",
		"Jun",
		"Jul",
		"Aug",
		"Sep",
		"Oct",
		"Nov",
		"Dec"
	};

	//Seconds since 1/1/1970
	var systime := polcore().systime;
	//Lets skip ahead to y2k
	systime := systime - 946684800;
	//convert to days
	var days := CINT (systime / SECONDS_PER_DAY) + 1;
	
	var current_year_counter := 0;
	var current_month_counter := 1;
	var days_in_month, done := 0;
	repeat
		if (current_month_counter == 2)
			if (current_year_counter%4 == 0)
				days_in_month := 29;
			else
				days_in_month := 28;
			endif
		else
			days_in_month := MonthDays[current_month_counter];
		endif
		
		if (days_in_month < days)
			days := days - days_in_month;
			current_month_counter := current_month_counter + 1;
			if (current_month_counter > 12)
				current_month_counter := 1;
				current_year_counter := current_year_counter + 1;
			endif
		else
			done := 1;
		endif
	until (done);

	if (days < 10)
		days := "0" + days;
	endif
	var timestring := MonthNames [current_month_counter] + " " + days + ", " + (2000 + current_year_counter);

	//now figure out hours and minutes
	systime := polcore().systime;
	systime := CINT (systime % SECONDS_PER_DAY);
	var hour := CINT (systime / 3600);

	systime := polcore().systime;
	systime := CINT (systime % 3600);
	var minute := CINT (systime / 60);
	
	if (hour == 0)
		hour := 24;
	elseif (hour < 10)
		hour := "0" + hour;
	endif
	if (minute < 10)
		minute := "0" + minute;
	endif
	
	timestring := timestring + " " + hour + ":" + minute + "GMT";
	return timestring;
endfunction












///////////////////
//  Gets the current year_month, used for recording playtime information
///////////////////

function GetCurrentPlaytimeMonth ()
	var year := Systime_GetYear ();
	var month := lower (Systime_GetMonth ());
	
	return (year + "_" + month + "_play_time");
endfunction




///////////////////
//  Gets the previous year_month, used for recording playtime information
///////////////////

function GetPreviousPlaytimeMonth (previous_month_counter := 1)
	var year := Systime_GetYear ();
	var month := lower (Systime_GetMonth ());
	
	repeat
		case (month)
			"december":
				month := "november";
			"november":
				month := "october";
			"october":
				month := "september";
			"september":
				month := "august";
			"august":
				month := "july";
			"july":
				month := "june";
			"june":
				month := "may";
			"may":
				month := "april";
			"april":
				month := "march";
			"march":
				month := "february";
			"february":
				month := "january";
			"january":
				month := "december";
				year := year - 1;
			default:
				Syslog ("Error figuring out month " + month);
				return "";
		endcase
		previous_month_counter := previous_month_counter - 1;
	until (!previous_month_counter);

	return (year + "_" + month + "_play_time");
endfunction




///////////////////
//  Turns the number of seconds into a string describing it in time terms
///////////////////

function GetTimeDesc (seconds, showseconds := 0)
	seconds := CINT (seconds);
	var minutes := CINT (seconds/60);
	var hours := CINT (minutes/60);
	var days := CINT (hours/24);

	if (seconds < 60)
		if (showseconds)
			return (seconds + " seconds");
		else
			return "< 1 minute";
		endif
	endif
	
	hours := hours - days*24;
	minutes := minutes - (days*24 + hours)*60;
	
	var returnstring := "";
	if (days)
		if (days > 1)
			returnstring := returnstring + days + " days, ";
		else
			returnstring := returnstring + "1 day, ";
		endif
	endif
	
	if (hours or days)
		if (hours > 1)
			returnstring := returnstring + hours + " hours, ";
		else
			returnstring := returnstring + "1 hour, ";
		endif
	endif
	
	if (minutes or hours or days)
		if (minutes > 1)
			returnstring := returnstring + minutes + " minutes";
		else
			returnstring := returnstring +  "1 minute";
		endif
	endif
	
	if (showseconds)
		seconds := CINT (seconds%60);
		if (seconds > 1)
			returnstring := returnstring + ", " + seconds + " seconds, ";
		elseif (seconds == 1)
			returnstring := returnstring + ", " + "1 second, ";
		else
			returnstring := returnstring + ", " + "0 seconds, ";
		endif
	endif
	
	return returnstring;
endfunction




///////////////////
//  Turns the number of seconds into a string describing it in time terms - only shorter
///////////////////

function GetShortTimeDesc (seconds, showseconds := 0)
	seconds := CINT (seconds);
	var minutes := CINT (seconds/60);
	var hours := CINT (minutes/60);
	var days := CINT (hours/24);

	if (seconds < 60)
		if (showseconds)
			return (seconds + " sec.");
		else
			return "< 1 min.";
		endif
	endif
	
	hours := hours - days*24;
	minutes := minutes - (days*24 + hours)*60;
	
	var returnstring := "";
	if (days)
		returnstring := returnstring + days + "d ";
	endif
	
	if (hours or days)
		returnstring := returnstring + hours + "h ";
	endif
	
	if (minutes or hours or days)
		returnstring := returnstring + minutes + "m";
	endif
	
	if (showseconds)
		returnstring := returnstring + " " + seconds + "s";
	endif
	
	return returnstring;
endfunction




///////////////////
//  Turns the number of seconds into a string describing it in time terms, but only up to hours (no days)
///////////////////

function GetTimeDescInHours (seconds, showseconds := 0)
	seconds := CINT (seconds);
	var minutes := CINT (seconds/60);
	var hours := CINT (minutes/60);

	if (seconds < 60)
		if (showseconds)
			return (seconds + " seconds");
		else
			return "< 1 minute";
		endif
	endif
	
	minutes := minutes - hours*60;
	
	var returnstring := "";
	if (hours)
		if (hours > 1)
			returnstring := returnstring + hours + " hours, ";
		else
			returnstring := returnstring + "1 hour, ";
		endif
	endif
	
	if (minutes or hours)
		if (minutes > 1)
			returnstring := returnstring + minutes + " minutes";
		else
			returnstring := returnstring +  "1 minute";
		endif
	endif
	
	if (showseconds)
		seconds := CINT (seconds%60);
		if (seconds > 1)
			returnstring := returnstring + ", " + seconds + " seconds, ";
		elseif (seconds == 1)
			returnstring := returnstring + ", " + "1 second, ";
		else
			returnstring := returnstring + ", " + "0 seconds, ";
		endif
	endif
	
	return returnstring;
endfunction



