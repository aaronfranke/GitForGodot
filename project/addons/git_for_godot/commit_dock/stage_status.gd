tool
extends Label

var staged_file_count = 0 setget _set_staged_file_count
var unstaged_file_count = 0 setget _set_unstaged_file_count


func update_status():
	var is_staged = staged_file_count > 0
	var is_unstaged = unstaged_file_count > 0
	text = ""

	if not is_staged and not is_unstaged:
		text += "No changes, working tree clean."
		return

	if is_staged:
		text += str(staged_file_count) + " staged file" + ("" if staged_file_count == 1 else "s")
		if is_unstaged:
			text += ", "

	if is_unstaged:
		text += str(unstaged_file_count) + " unstaged file" + ("" if unstaged_file_count == 1 else "s")


func set_staged_and_unstaged_file_counts(staged, unstaged):
	staged_file_count = staged
	unstaged_file_count = unstaged
	update_status()


func _set_staged_file_count(value):
	staged_file_count = value
	update_status()


func _set_unstaged_file_count(value):
	unstaged_file_count = value
	update_status()
