-scripts-
guild_questmaster		- ai for the questmaster NPC that hands out quests
guild_shopkeep			- ai for the guild's shopkeep
buyitems				- script called when a player wants to buy items from the shopkeep


-properties-
"players_guild" - placed on the player when he joins a guild.  contains:
	[1]	- name of the guild
	[2]	- date the player joined the guild
	[3]	- last time they payed guild dues
	[4]	- guild ranking points (determines guild title)

"guild_quest"
	[1]	- quest cfg file
	[2]	- quest key
	[3]	- objtype of the item desired
	[4]	- property name on the item desired that marks that its what's being looked for
	[5]	- property value on the item desired that marks that its what's being looked for
	[6]	- number of the item needed
	[7]	- time the quest is over
	[8]	- number of points that the player will recieve when they complete the quest




-notes-
When a player quits a guild, the name of the guild they're a member of is set to "none", and the date
	is set to the time they quit.  They can't join any other guild for a week from that point.