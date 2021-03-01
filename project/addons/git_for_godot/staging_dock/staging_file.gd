tool
extends Control

var simple_native
var _name


func setup(name, status):
	_name = name
	$Label.text = name

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
	simple_native.stage_one(_name)
