tool
extends Control

var simple_native
var staging_dock

var _stage_and_commit := false

onready var commit_name = $VBoxContainer/CommitName
onready var commit_description = $VBoxContainer/CommitDescription
onready var commit_button = $VBoxContainer/CommitButtons/Commit
onready var amend_button = $VBoxContainer/CommitButtons/Amend

onready var _stage_status = $VBoxContainer/StageStatus


func _process(_delta):
	var new_stage_and_commit = _stage_status.is_unstaged and not _stage_status.is_staged
	if new_stage_and_commit != _stage_and_commit:
		_stage_and_commit = new_stage_and_commit
		_change_commit_text()


func _on_Stage_pressed():
	simple_native.stage_all()
	staging_dock.update_status()


func _on_Unstage_pressed():
	simple_native.unstage_all()
	staging_dock.update_status()


func _on_Discard_pressed():
	simple_native.discard_unstaged()
	staging_dock.update_status()


func _on_Commit_pressed():
	if _stage_and_commit:
		simple_native.stage_all()
	simple_native.commit(amend_button.pressed, commit_name.text, commit_description.text)
	amend_button.pressed = false
	_on_Amend_pressed()
	staging_dock.update_status()


func _on_Amend_pressed():
	simple_native.get_branch_list()
	if amend_button.pressed:
		var message = simple_native.get_head_message()
		commit_name.text = message[0]
		commit_description.text = message[1]
	else:
		commit_name.text = ""
		commit_description.text = ""
	_change_commit_text()


func _change_commit_text():
	if amend_button.pressed:
		if _stage_and_commit:
			commit_button.text = "Stage and amend previous commit"
		else:
			commit_button.text = "Amend previous commit"
	else:
		if _stage_and_commit:
			commit_button.text = "Stage and commit changes"
		else:
			commit_button.text = "Commit changes"
