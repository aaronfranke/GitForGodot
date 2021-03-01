tool
extends Control

var simple_native
var staging_dock
var _amend = false

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
	simple_native.commit(_amend, commit_name.text, commit_description.text)
	staging_dock.update_status()


func _on_Amend_pressed():
	_amend = amend_button.pressed
	if _amend:
		commit_button.text = "Amend previous commit"
	else:
		commit_button.text = "Commit changes"
