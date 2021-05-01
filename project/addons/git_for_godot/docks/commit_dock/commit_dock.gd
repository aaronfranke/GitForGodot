tool
extends Control

var _simple_native
var _dock_manager

var _is_commit_disabled := false
var _stage_and_commit := false

onready var commit_name = $VBoxContainer/CommitName
onready var commit_description = $VBoxContainer/CommitDescription
onready var commit_button = $VBoxContainer/CommitButtons/Commit
onready var amend_button = $VBoxContainer/CommitButtons/Amend

onready var _stage_status = $VBoxContainer/StageStatus


func setup(simple_native, dock_manager):
	_simple_native = simple_native
	_dock_manager = dock_manager


func _process(_delta):
	var new_is_commit_disabled = commit_name.text.empty() or not (_stage_status.is_unstaged or _stage_status.is_staged)
	if new_is_commit_disabled != _is_commit_disabled:
		_is_commit_disabled = new_is_commit_disabled
		commit_button.disabled = new_is_commit_disabled

	var new_stage_and_commit = _stage_status.is_unstaged and not _stage_status.is_staged
	if new_stage_and_commit != _stage_and_commit:
		_stage_and_commit = new_stage_and_commit
		_change_commit_text()


func _on_Stage_pressed():
	_simple_native.stage_all()
	_dock_manager.force_refresh = true


func _on_Unstage_pressed():
	_simple_native.unstage_all()
	_dock_manager.force_refresh = true


func _on_Discard_pressed():
	_simple_native.discard_unstaged()
	_dock_manager.force_refresh = true


func _on_Commit_pressed():
	if _stage_and_commit:
		_simple_native.stage_all()
	_simple_native.commit(amend_button.pressed, commit_name.text, commit_description.text)
	amend_button.pressed = false
	_on_Amend_pressed()
	_dock_manager.force_refresh = true


func _on_Amend_pressed():
	_simple_native.get_branch_list()
	if amend_button.pressed:
		var message = _simple_native.get_head_message()
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
