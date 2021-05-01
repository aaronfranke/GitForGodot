tool
extends Control

const SAVE_PATH := "user://git_for_godot_settings.ini"

var _editor_plugin
var _dock_manager

var displayed_commits: SpinBox
var auto_refresh_status_delay: SpinBox
var auto_fetch_delay: SpinBox
var auto_hide_docks: CheckBox
var auto_show_docks: CheckBox


func setup(editor_plugin, dock_manager):
	_editor_plugin = editor_plugin
	_dock_manager = dock_manager
	displayed_commits = $DisplayedCommits/SpinBox
	auto_refresh_status_delay = $AutoRefreshStatusDelay/SpinBox
	auto_fetch_delay = $AutoFetchDelay/SpinBox
	auto_hide_docks = $AutoHideDocks/Panel/CheckBox
	auto_show_docks = $AutoShowDocks/Panel/CheckBox
	for item in [displayed_commits, auto_refresh_status_delay, auto_fetch_delay]:
		item.hint_tooltip = item.get_parent().hint_tooltip
	# Load settings from disk.
	var config := ConfigFile.new()
	config.load(SAVE_PATH)
	displayed_commits.value = int(config.get_value("history", "displayed_commits", int(displayed_commits.value)))
	auto_refresh_status_delay.value = config.get_value("time", "auto_refresh_status_delay", auto_refresh_status_delay.value)
	auto_fetch_delay.value = config.get_value("time", "auto_fetch_delay", auto_fetch_delay.value)
	auto_hide_docks.pressed = config.get_value("ui", "auto_hide_docks", auto_hide_docks.pressed)
	auto_show_docks.pressed = config.get_value("ui", "auto_show_docks", auto_show_docks.pressed)
	auto_show_docks.disabled = not auto_hide_docks.pressed
	_editor_plugin.auto_hide_docks = auto_hide_docks.pressed
	_editor_plugin.auto_show_docks = auto_show_docks.pressed
	_dock_manager.auto_refresh_status_delay = auto_refresh_status_delay.value


func save():
	# Save settings to disk.
	var config := ConfigFile.new()
	config.set_value("history", "displayed_commits", int(displayed_commits.value))
	config.set_value("time", "auto_refresh_status_delay", auto_refresh_status_delay.value)
	config.set_value("time", "auto_fetch_delay", auto_fetch_delay.value)
	config.set_value("ui", "auto_hide_docks", auto_hide_docks.pressed)
	config.set_value("ui", "auto_show_docks", auto_show_docks.pressed)
	config.save(SAVE_PATH)
	# Apply settings to objects.
	_dock_manager.auto_refresh_status_delay = auto_refresh_status_delay.value
	_editor_plugin.auto_hide_docks = auto_hide_docks.pressed
	_editor_plugin.auto_show_docks = auto_show_docks.pressed
	auto_show_docks.disabled = not auto_hide_docks.pressed
