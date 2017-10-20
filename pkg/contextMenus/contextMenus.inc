
const C_CONTEXT_MENU	:= "contextMenu";
enum EContextMenuResult
	eCMR_Buy := 1,
	eCMR_Sell,
	eCMR_Bank,
	eCMR_WriteCheck,
	eCMR_CommGuard,
	eCMR_CommFollow,
	eCMR_CommDrop,
	eCMR_CommKill,
	eCMR_CommStop,
	eCMR_CommTransfer,
	eCMR_CommStay,
	eCMR_Release,
	eCMR_Hire,
	eCMR_CheckBalance,
	eCMR_StablePet,
	eCMR_OpenBackpack,
	eCMR_Quest,
	eCMR_GateTravel,
	eCMR_CraftItem,
	eCMR_Train,
	eCMR_Clear,
	eCMR_SummonPet,
	eCMR_Join,
	eCMR_Resign,
	eCMR_Dismiss,
	eCMR_Status
endenum

enum EContextMenuText
	eCMT_Buy			:= 6103,
	eCMT_Sell			:= 6104,
	eCMT_Bank			:= 6105,
	eCMT_WriteCheck	:= 6106,
	eCMT_CommGuard	:= 6107,
	eCMT_CommFollow	:= 6108,
	eCMT_CommDrop		:= 6109,
	eCMT_CommKill		:= 6111,
	eCMT_CommStop		:= 6112,
	eCMT_CommTransfer	:= 6113,
	eCMT_CommStay		:= 6114,
	eCMT_Release		:= 6118,
	eCMT_Hire			:= 6120,
	eCMT_CheckBalance	:= 6124,
	eCMT_StablePet		:= 6126,
	eCMT_OpenBackpack	:= 6145,
	eCMT_Quest		:= 1024,
	eCMT_GateTravel		:= 2062,
	eCMT_CraftItem		:= 6138,
	eCMT_Train		:= 10069,
	eCMT_Clear		:= 413,
	eCMT_SummonPet	:= 6181,
	eCMT_Join			:= 6116,
	eCMT_Resign		:= 6115,
	eCMT_Dismiss		:= 6129,
	eCMT_Status		:= 2134
endenum

function CreateNewContextMenu() 
	return array;     
endfunction     

function ContextMenuAddItem(byref menu, result, text, flags := 0x20, color := 0xFFFF)
	menu.append(array {CInt(result), CInt(text), flags, color});
endfunction 