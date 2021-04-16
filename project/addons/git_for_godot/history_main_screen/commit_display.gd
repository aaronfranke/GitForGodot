tool
extends ColorRect

# Keep this in sync with the "git_commit_to_godot_array" method in the C code.
enum CommitArrayIndex {
	TIMESTAMP = 0,
	SUMMARY = 1,
	BODY = 2,
	AUTHOR_EMAIL = 3,
	AUTHOR_NAME = 4,
	AUTHOR_TIME = 5,
	COMMITTER_EMAIL = 6,
	COMMITTER_NAME = 7,
	COMMITTER_TIME = 8,
	PARENT_COUNT = 9,
	PARENT_FIRST = 10,
}

var commit_time
var commit_hash

var _simple_native
var _commit_array
var _details_generated := false
var _local_time_offset_from_utc := 0
var _mouse_over := false

onready var _popup_menu = $PopupMenu
onready var _details_popup = $DetailsPopup
onready var _details_popup_vbox = $DetailsPopup/VBoxContainer
onready var _reset_popup = $ResetPopup
onready var _create_branch_popup = $CreateBranchPopup


func _input(input_event):
	if _mouse_over and input_event is InputEventMouseButton and input_event.is_pressed():
		if input_event.button_index == BUTTON_RIGHT or input_event.doubleclick:
			color = Color(0.16, 0.19, 0.25)
			_popup_menu.popup(Rect2(get_global_mouse_position(), _popup_menu.rect_size))


func setup(simple_native, commit_hash_, commit_array, commit_time_, system_unix_time, local_time_offset_from_utc):
	_simple_native = simple_native
	commit_time = commit_time_
	commit_hash = commit_hash_
	_commit_array = commit_array
	_local_time_offset_from_utc = local_time_offset_from_utc
	$HBoxContainer/Name.text = commit_array[CommitArrayIndex.SUMMARY]
	name = commit_hash
	var datetime = OS.get_datetime_from_unix_time(commit_time + local_time_offset_from_utc)
	if abs(system_unix_time - commit_time) > 80000:
		$HBoxContainer/Time.text = "%s-%02d-%02d" % [datetime.year, datetime.month, datetime.day]
	else:
		$HBoxContainer/Time.text = "%s:%02d:%02d" % [datetime.hour, datetime.minute, datetime.second]


func _on_CommitDisplay_mouse_entered():
	_mouse_over = true


func _on_CommitDisplay_mouse_exited():
	_mouse_over = false


func _on_PopupMenu_popup_hide():
	color = Color.transparent


func _on_PopupMenu_id_pressed(id):
	match id:
		0: # View details.
			if not _details_generated:
				_generate_details()
			_details_popup.popup_centered()
		1: # Cherry-pick commit.
			pass
		2: # Revert commit.
			pass
		3: # Reset to this commit.
			_reset_popup.popup_centered()
		4: # Create branch here.
			_create_branch_popup.popup_centered()


# For efficiency reasons, only generate the details when needed.
func _generate_details():
	_details_generated = true
	var summary_label = Label.new()
	summary_label.text = _commit_array[CommitArrayIndex.SUMMARY]
	_details_popup_vbox.add_child(summary_label)
	var body_label = Label.new()
	body_label.text = _commit_array[CommitArrayIndex.BODY]
	_details_popup_vbox.add_child(body_label)
	var hash_label = Label.new()
	hash_label.text = "Hash: " + commit_hash
	_details_popup_vbox.add_child(hash_label)
	var time_label = Label.new()
	time_label.text = "Commit time: " + _timestamp_to_date(commit_time)
	_details_popup_vbox.add_child(time_label)
	var author_name_label = Label.new()
	author_name_label.text = "Author name: " + _commit_array[CommitArrayIndex.AUTHOR_NAME]
	_details_popup_vbox.add_child(author_name_label)
	var author_email_label = Label.new()
	author_email_label.text = "Author email: " + _commit_array[CommitArrayIndex.AUTHOR_EMAIL]
	_details_popup_vbox.add_child(author_email_label)
	if commit_time != _commit_array[CommitArrayIndex.AUTHOR_TIME]:
		var author_time_label = Label.new()
		author_time_label.text = "Author time: " + _timestamp_to_date(_commit_array[CommitArrayIndex.AUTHOR_TIME])
		_details_popup_vbox.add_child(author_time_label)
	if _commit_array[CommitArrayIndex.AUTHOR_NAME] != _commit_array[CommitArrayIndex.COMMITTER_NAME]:
		var committer_name_label = Label.new()
		committer_name_label.text = "Committer name: " + _commit_array[CommitArrayIndex.COMMITTER_NAME]
		_details_popup_vbox.add_child(committer_name_label)
	if _commit_array[CommitArrayIndex.AUTHOR_EMAIL] != _commit_array[CommitArrayIndex.COMMITTER_EMAIL]:
		var committer_email_label = Label.new()
		committer_email_label.text = "Committer email: " + _commit_array[CommitArrayIndex.COMMITTER_EMAIL]
		_details_popup_vbox.add_child(committer_email_label)
	if _commit_array[CommitArrayIndex.AUTHOR_TIME] != _commit_array[CommitArrayIndex.COMMITTER_TIME] \
			and commit_time != _commit_array[CommitArrayIndex.COMMITTER_TIME]:
		var committer_time_label = Label.new()
		committer_time_label.text = "Committer time: " + _timestamp_to_date(_commit_array[CommitArrayIndex.COMMITTER_TIME])
		_details_popup_vbox.add_child(committer_time_label)


func _timestamp_to_date(timestamp):
	var datetime = OS.get_datetime_from_unix_time(timestamp + _local_time_offset_from_utc)
	return "%s-%02d-%02d %02d:%02d:%02d" % [datetime.year, datetime.month, datetime.day, datetime.hour, datetime.minute, datetime.second]
