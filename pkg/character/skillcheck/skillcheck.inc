function CheckSkillWithItem (who, item, skillid, difficulty := -1)
	return run_script_to_completion (":skillcheck:checkskillwithitem",
		 array {who, item, skillid, difficulty});
endfunction
