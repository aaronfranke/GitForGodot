tool
extends Control

var simple_native
var staging_dock

onready var commit_name = $VBoxContainer/CommitName
onready var commit_description = $VBoxContainer/CommitDescription
onready var commit_button = $VBoxContainer/CommitButtons/Commit
onready var amend_button = $VBoxContainer/CommitButtons/Amend


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
		commit_button.text = "Amend previous commit"
	else:
		commit_name.text = ""
		commit_description.text = ""
		commit_button.text = "Commit changes"
