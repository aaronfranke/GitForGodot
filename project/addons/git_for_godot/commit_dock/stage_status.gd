tool
extends Label

var staged_file_count = 0 setget _set_staged_file_count
var unstaged_file_count = 0 setget _set_unstaged_file_count

func _ready():
	update_status()


func update_status():
	var is_staged = staged_file_count > 0
	var is_unstaged = unstaged_file_count > 0
	text = ""

	if not is_staged and not is_unstaged:
		text = "No changes, working tree clean."
		return

	if is_staged:
		text = str(staged_file_count) + " staged files"
		if is_unstaged:
			text += ", "

	if is_unstaged:
		text += str(staged_file_count) + " unstaged files"


func _set_staged_file_count(value):
	staged_file_count = value
	update_status()


func _set_unstaged_file_count(value):
	unstaged_file_count = value
	update_status()
