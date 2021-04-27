tool
extends Control

const SAVE_PATH := "user://git_for_godot_settings.ini"

var _editor_plugin

onready var displayed_commits := $DisplayedCommits/SpinBox
onready var auto_hide := $AutoHide/Panel/CheckBox
onready var auto_show := $AutoShow/Panel/CheckBox


func setup(editor_plugin):
	_editor_plugin = editor_plugin
	var config := ConfigFile.new()
	config.load(SAVE_PATH)
	displayed_commits.value = int(config.get_value("history", "displayed_commits", int(displayed_commits.value)))
	auto_hide.pressed = config.get_value("ui", "auto_hide", auto_hide.pressed)
	auto_show.pressed = config.get_value("ui", "auto_show", auto_show.pressed)
	auto_show.disabled = not auto_hide.pressed
	_editor_plugin.auto_hide_docks = auto_hide.pressed
	_editor_plugin.auto_show_docks = auto_show.pressed


func _on_SettingsPopup_confirmed():
	var config := ConfigFile.new()
	config.set_value("history", "displayed_commits", int(displayed_commits.value))
	config.set_value("ui", "auto_hide", auto_hide.pressed)
	config.set_value("ui", "auto_show", auto_show.pressed)
	config.save(SAVE_PATH)


func _on_AutoHide_toggled(button_pressed):
	_editor_plugin.auto_hide_docks = button_pressed
	auto_show.disabled = not button_pressed


func _on_AutoShow_toggled(button_pressed):
	_editor_plugin.auto_show_docks = button_pressed
