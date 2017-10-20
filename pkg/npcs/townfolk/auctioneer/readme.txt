auctioneer.src - the AI for the auctioneer.  This script handles interacting with players, and
	calls other sub-scripts to get stuff done
sellitem - called when a player hands the auctioneer an item, this script lets the player enter
	a description, minimum bid, auction length, etc.
buyitem - Called when a player says 'buy' to the auctioneer, this script displays all the items
	up for sale and lets the player place bids
completeauctions - called occasionally, this script checks all the items up for sale, and completes
	the transaction for all the auctions that have ended.


------------------------------
- properties placed on items -
------------------------------

endtime			- time when the auction ends
minbid			- the current going bid
sellerserial		- the seller number of the player who put the item up for auction
sellername		- name of the player who put the item up for auction
selleracctname		- accountname of the player who put the item up for auction
current_high_bidder	- accountname of the player who is currently winning the auction
current_high_bid	- the amount of the bid placed by the current high bidder

