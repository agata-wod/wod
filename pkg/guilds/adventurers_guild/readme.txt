-scripts-
guild_questmaster		- ai for the questmaster NPC that hands out quests
guild_shopkeep			- ai for the guild's shopkeep
buyitems				- script called when a player wants to buy items from the shopkeep


-properties-
"players_guild" - placed on the player when he joins a guild.  contains:
	[1]	- name of the guild
	[2]	- date the player joined the guild
	[3]	- last time they payed guild dues


"guild_quest" - placed on the player when assigned a quest
	[1]	- config file that contains the quests given
	[2]	- the config key of the quest
	[3]	- the objtype of the item that needs to be returned
	[4]	- the name of the property that will be on the correct item
	[5]	- the value that the previously mentioned property should have
	[6]	- the number of items that need to be returned to complete the quest
	[7]	- the time that the quest expires
	[8]	- the number of points that will be awarded for completing the quest
	[9]	- if we're looking for something with a particular serial number, this'll be where it is
	[10] - the time the quest started


-notes-
When a player quits a guild, the name of the guild they're a member of is set to "none", and the date
	is set to the time they quit.  They can't join any other guild for a week from that point.