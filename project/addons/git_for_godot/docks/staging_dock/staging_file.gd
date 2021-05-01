tool
extends Control

var _simple_native
var _dock_manager
var _name


func setup(simple_native, dock_manager, file_name, status):
	_simple_native = simple_native
	_dock_manager = dock_manager
	_name = file_name
	$Label.text = file_name

	if status & LibGit2Defines.GIT_STATUS_ANY_NEW:
		$TextureRect.texture = load("res://addons/git_for_godot/icons/Add.svg")
	elif status & LibGit2Defines.GIT_STATUS_ANY_MODIFIED:
		$TextureRect.texture = load("res://addons/git_for_godot/icons/Modify.svg")
	elif status & LibGit2Defines.GIT_STATUS_ANY_DELETED:
		$TextureRect.texture = load("res://addons/git_for_godot/icons/Delete.svg")
	elif status & LibGit2Defines.GIT_STATUS_ANY_RENAMED:
		$TextureRect.texture = load("res://addons/git_for_godot/icons/Rename.svg")

	if status & LibGit2Defines.GIT_STATUS_INDEX_CHANGES:
		$Button.text = "Unstage"
	elif status & LibGit2Defines.GIT_STATUS_WT_CHANGES:
		$Button.text = "Stage"


func _on_Button_pressed():
	_simple_native.stage_one(_name)
	_dock_manager.force_refresh = true
