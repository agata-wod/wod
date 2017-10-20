use uo;
use os;
use util;

include "include/client";
include "include/objtype";
include "include/utility";
include "include/eventid";
include "include/accounts";
include "include/clock";
include "include/gumps";


///////////////////
//  figures out how many tickets the player has
///////////////////

function CountPlayersTickets (player_acctname, guild_name)
	var tickets := GetAccountProperty (player_acctname, guild_name + "_tickets");
	if (tickets)
		return tickets;
	endif
	return 0;
endfunction




///////////////////
//  adds or subtracts tickets from a player's account (pass a negative number to subtract)
///////////////////

function AddTicketsToPlayer (player_acctname, new_amount, guild_name)
	set_critical (1);

	var tickets := GetAccountProperty (player_acctname, guild_name + "_tickets");
	if (!tickets)
		tickets := 0;
	endif

	tickets := tickets + CINT (new_amount);
	SetAccountProperty (player_acctname, guild_name + "_tickets", tickets);

	set_critical (0);
endfunction




///////////////////
//  adds tickets to a monthly counter.  Only awarded tickets for completed tasks go here
///////////////////

function AddToMonthlyTicketCounter (player_acctname, new_amount, guild_name)
	set_critical (1);
	
	var year := Systime_GetYear ();
	var month := lower (Systime_GetMonth ());

	var monthly_tickets := GetAccountProperty (player_acctname, guild_name + "_tickets_" + year + "_" + month);
	if (!monthly_tickets)
		monthly_tickets := 0;
	endif
	monthly_tickets := monthly_tickets + CINT (new_amount);
	SetAccountProperty (player_acctname, guild_name + "_tickets_" + year + "_" + month, monthly_tickets);

	set_critical (0);
endfunction



///////////////////
//  determines if the player given is a member of the guild
///////////////////

function IsAGuildMember (player, guild_name)
	var players_guild := GetObjProperty (player, "players_guild");
	if (!players_guild)
		return 0;
	endif

	if (players_guild[1] == guild_name)
		return 1;
	endif

	return 0;
endfunction




///////////////////
//  determines whether the player can join a guild
///////////////////

function CanJoinGuild (player)
	var players_guild := GetObjProperty (player, "players_guild");
	if (!players_guild)
		return 1;
	endif

	//when a player quits a guild, they're guild is set to "none", and they can't join another guild for a week
	if (players_guild[1] == "none")
		if (!players_guild[2])
			return 1;
		endif
		if (players_guild[2] + 86400 < ReadGameClock () )
			return 1;
		endif
	endif

	return 0;
endfunction




///////////////////
//  set properties on the player for them to join the guild
///////////////////

function JoinGuild (player, guild_name, guild_title)
	var players_guild := array{};
	players_guild[1] := guild_name;
	players_guild[2] := ReadGameClock ();
	players_guild[3] := ReadGameClock ();
	players_guild[4] := 0;
	SetObjProperty (player, "players_guild", players_guild);
	AddGuildRankingPoints (player, 1, guild_title);
endfunction




///////////////////
//  set properties on the player for them to quit their guild
///////////////////

function QuitGuild (player, guild_name)
	var players_guild := array{};
	players_guild[1] := "none";
	players_guild[2] := ReadGameClock ();
	players_guild[3] := ReadGameClock ();
	SetObjProperty (player, "players_guild", players_guild);
	EraseObjProperty (player, "guild_quest");
	player.title_race := "";

	var tickets := CountPlayersTickets (player.acctname, guild_name);
	AddTicketsToPlayer (player.acctname, (0-tickets), guild_name);
endfunction




///////////////////
//  figures out if and how much money is owed to the guild
///////////////////

function DetermineMoneyOwed (player)
	var players_guild := GetObjProperty (player, "players_guild");
	if (!players_guild)
		return 0;
	endif

	var last_payment := players_guild[3];
	if (!last_payment)
		last_payment := ReadGameClock();
	endif

	var pasttime := ReadGameClock() - last_payment;
	if (pasttime < 0)
		return 0;
	endif

	var feeperiods := CINT (pasttime/604800);
	if (!feeperiods)
		return 0;
	endif

	return (feeperiods * 100);
endfunction




///////////////////
//  Pays off the guild fees
///////////////////

function PayOffGuildFees (player)
	var players_guild := GetObjProperty (player, "players_guild");
	if (!players_guild)
		Syslog ("Error trying to pay off guild dues!");
		return 0;
	endif

	players_guild[3] := ReadGameClock();
	SetObjProperty (player, "players_guild", players_guild);
endfunction




///////////////////
//  Returns the number of Guild ranking points
///////////////////

function GetGuildRankingPoints (player, guild_title)
  if (guild_title)
  endif
	var players_guild := GetObjProperty (player, "players_guild");
	if (!players_guild)
		player.title_race := "";
		return;
	endif

	var guild_points := players_guild[4];
	if (!guild_points)
		guild_points := 0;
	endif
	return guild_points;
endfunction




///////////////////
//  Adds points to a person's guild ranking and sets their title
///////////////////

function AddGuildRankingPoints (player, new_points, guild_title)
  if (guild_title)
  endif
	new_points := CINT (new_points);

	var players_guild := GetObjProperty (player, "players_guild");
	if (!players_guild)
		player.title_race := "";
		return;
	endif

	var old_points := players_guild[4];
	if (!old_points)
		old_points := 0;
	endif

	old_points := old_points + new_points;
	if (old_points < 0)
		old_points := 0;
	endif
	players_guild[4] := old_points;
	SetObjProperty (player, "players_guild", players_guild);

	//Don't want GMs with titles
	if (player.cmdlevel)
		return;
	endif
	
	var guildtitle := GetPlayerGuildRank (players_guild[1], old_points);
	player.title_race := guildtitle;
endfunction




///////////////////
//  Figures out the player's guild title
///////////////////

function GetPlayerGuildRank (guild, guild_points)
	var guildtitle;
	case (guild)
		"adventurers_guild":
			guildtitle := "Jr. Pathfinder, AG";
			if (guild_points > 50000)
				guildtitle := "Sr. Explorer, AG";
			elseif (guild_points > 45000)
				guildtitle := "Explorer, AG";
			elseif (guild_points > 40000)
				guildtitle := "Jr. Explorer, AG";
			elseif (guild_points > 35000)
				guildtitle := "Sr. Pioneer, AG";
			elseif (guild_points > 30000)
				guildtitle := "Pioneer, AG";
			elseif (guild_points > 25000)
				guildtitle := "Jr. Pioneer, AG";
			elseif (guild_points > 20000)
				guildtitle := "Sr. Guide, AG";
			elseif (guild_points > 15000)
				guildtitle := "Guide, AG";
			elseif (guild_points > 11000)
				guildtitle := "Jr. Guide, AG";
			elseif (guild_points > 8000)
				guildtitle := "Sr. Trailblazer, AG";
			elseif (guild_points > 4500)
				guildtitle := "Trailblazer, AG";
			elseif (guild_points > 2000)
				guildtitle := "Jr. Trailblazer, AG";
			elseif (guild_points > 1000)
				guildtitle := "Sr. Pathfinder, AG";
			elseif (guild_points > 500)
				guildtitle := "Pathfinder, AG";
			endif
		default:
			Syslog ("Error:  Invalid guild!");
			guildtitle := "";
	endcase
	return guildtitle;
endfunction




///////////////////
//  Returns how many guild points the player needs to get to the next 'level'
///////////////////

function GetFullGuildPointsRequiredForNextLevel (player)
	var players_guild := GetObjProperty (player, "players_guild");
	if (!players_guild)
		return 0xffff;
	endif

	var guild_points := players_guild[4];
	if (!guild_points)
		guild_points := 0;
	endif

	if ( guild_points < 500 )
		return 250;
	elseif ( guild_points < 1000 )
		return 500;
	elseif ( guild_points < 2000 )
		return 1000;
	elseif ( guild_points < 4500 )
		return 2250;
	elseif ( guild_points < 8000 )
		return 4000;
	elseif ( guild_points < 11000 )
		return 5500;
	elseif ( guild_points < 15000 )
		return 7500;
	elseif ( guild_points < 20000 )
		return 10000;
	elseif ( guild_points < 25000 )
		return 12500;
	elseif ( guild_points < 30000 )
		return 15000;
	elseif ( guild_points < 35000 )
		return 17500;
	elseif ( guild_points < 40000 )
		return 20000;
	elseif ( guild_points < 45000 )
		return 22500;
	elseif ( guild_points < 50000 )
		return 25000;
	else
		return 25600;
	endif
endfunction




///////////////////
//  Returns how many guild points the player needs to get to the next 'level'
///////////////////

function GetGuildPointsRequiredForNextLevel (player)
	var players_guild := GetObjProperty (player, "players_guild");
	if (!players_guild)
		return 0;
	endif

	var guild_points := players_guild[4];
	if (!guild_points)
		return 200;
	endif
	
	if (guild_points < 500)
		return (500 - guild_points);
	elseif (guild_points < 1000)
		return (1000 - guild_points);
	elseif (guild_points < 2000)
		return (2000 - guild_points);
	elseif (guild_points < 4500)
		return (4500 - guild_points);
	elseif (guild_points < 8000)
		return (8000 - guild_points);
	elseif (guild_points < 11000)
		return (11000 - guild_points);
	elseif (guild_points < 15000)
		return (15000 - guild_points);
	elseif (guild_points < 20000)
		return (20000 - guild_points);
	elseif (guild_points < 25000)
		return (25000 - guild_points);
	elseif (guild_points < 30000)
		return (30000 - guild_points);
	elseif (guild_points < 35000)
		return (35000 - guild_points);
	elseif (guild_points < 40000)
		return (40000 - guild_points);
	elseif (guild_points < 45000)
		return (45000 - guild_points);
	elseif (guild_points < 50000)
		return (50000 - guild_points);
	else
		return 0;
	endif
endfunction




