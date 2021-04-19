tool
extends EditorPlugin


const SimpleNative = preload("res://addons/git_for_godot/gdnative/simple.gdns")
const MainScreen = preload("res://addons/git_for_godot/history_main_screen/history_main_screen.tscn")
const BranchDock = preload("res://addons/git_for_godot/branch_dock/branch_dock.tscn")
const CommitDock = preload("res://addons/git_for_godot/commit_dock/commit_dock.tscn")
const RemoteDock = preload("res://addons/git_for_godot/remote_dock/remote_dock.tscn")
const StagingDock = preload("res://addons/git_for_godot/staging_dock/staging_dock.tscn")

var simple_native
var main_screen_instance
var branch_dock_instance
var commit_dock_instance
var remote_dock_instance
var staging_dock_instance
var instances

var auto_hide_docks := true


func _enter_tree():
	# Create objects and scenes.
	simple_native = SimpleNative.new()
	main_screen_instance = MainScreen.instance()
	branch_dock_instance = BranchDock.instance()
	commit_dock_instance = CommitDock.instance()
	remote_dock_instance = RemoteDock.instance()
	staging_dock_instance = StagingDock.instance()
	instances = [main_screen_instance, branch_dock_instance, commit_dock_instance, remote_dock_instance, staging_dock_instance]

	# Set scene variables.
	main_screen_instance.simple_native = simple_native
	branch_dock_instance.simple_native = simple_native
	commit_dock_instance.simple_native = simple_native
	remote_dock_instance.simple_native = simple_native
	staging_dock_instance.simple_native = simple_native
	branch_dock_instance.remote_dock = remote_dock_instance
	commit_dock_instance.staging_dock = staging_dock_instance
	remote_dock_instance.branch_dock = branch_dock_instance
	staging_dock_instance.commit_dock = commit_dock_instance
	var wip = main_screen_instance.get_node(@"ColorRect/MarginContainer/VBoxContainer/ScrollContainer/GitHistory/Commits/Names/WIP")
	commit_dock_instance.get_node(@"VBoxContainer/StageStatus").wip_node = wip
	# Add the main panel to the editor's main viewport.
	get_editor_interface().get_editor_viewport().add_child(main_screen_instance)
	if not auto_hide_docks:
		add_control_to_dock(EditorPlugin.DOCK_SLOT_LEFT_UR, branch_dock_instance)
		add_control_to_dock(EditorPlugin.DOCK_SLOT_RIGHT_BL, commit_dock_instance)
		add_control_to_dock(EditorPlugin.DOCK_SLOT_LEFT_BR, remote_dock_instance)
		add_control_to_dock(EditorPlugin.DOCK_SLOT_RIGHT_UL, staging_dock_instance)
	# Hide the main panel. Very much required.
	make_visible(false)


func _exit_tree():
	if instances:
		for i in instances:
			try_remove_control_from_docks(i)
			i.queue_free()


func has_main_screen():
	return true


func make_visible(visible):
	if auto_hide_docks:
		if visible:
			try_add_control_to_dock(EditorPlugin.DOCK_SLOT_LEFT_UR, branch_dock_instance)
			try_add_control_to_dock(EditorPlugin.DOCK_SLOT_RIGHT_BL, commit_dock_instance)
			try_add_control_to_dock(EditorPlugin.DOCK_SLOT_LEFT_BR, remote_dock_instance)
			try_add_control_to_dock(EditorPlugin.DOCK_SLOT_RIGHT_UL, staging_dock_instance)
		else:
			try_remove_control_from_docks(branch_dock_instance)
			try_remove_control_from_docks(commit_dock_instance)
			try_remove_control_from_docks(remote_dock_instance)
			try_remove_control_from_docks(staging_dock_instance)
	if main_screen_instance:
		main_screen_instance.visible = visible


func try_add_control_to_dock(slot, dock):
	if dock:
		add_control_to_dock(slot, dock)
	else:
		printerr("Dock is null, this should never happen! If you can figure out what causes this, please report a bug or open a PR.")


func try_remove_control_from_docks(dock):
	if dock and dock.get_parent() is TabContainer:
		remove_control_from_docks(dock)


func get_plugin_name():
	return "Git History"


func get_plugin_icon():
	# Must return some kind of Texture for the icon.
	return preload("res://addons/git_for_godot/history_main_screen/Git-icon-white.svg")
