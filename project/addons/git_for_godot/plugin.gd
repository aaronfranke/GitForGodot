tool
extends EditorPlugin


const SimpleNative = preload("res://addons/git_for_godot/gdnative/simple.gdns")
const MainScreen = preload("res://addons/git_for_godot/history_main_screen/history_main_screen.tscn")
const CommitDock = preload("res://addons/git_for_godot/commit_dock/commit_dock.tscn")
const StagingDock = preload("res://addons/git_for_godot/staging_dock/staging_dock.tscn")

var simple_native
var main_screen_instance
var commit_dock_instance
var staging_dock_instance


func _enter_tree():
	simple_native = SimpleNative.new()
	main_screen_instance = MainScreen.instance()
	commit_dock_instance = CommitDock.instance()
	staging_dock_instance = StagingDock.instance()

	commit_dock_instance.simple_native = simple_native
	# Add the main panel to the editor's main viewport.
	get_editor_interface().get_editor_viewport().add_child(main_screen_instance)
	# Hide the main panel. Very much required.
	make_visible(false)


func _exit_tree():
	remove_control_from_docks(commit_dock_instance)
	if commit_dock_instance:
		commit_dock_instance.queue_free()
	if main_screen_instance:
		main_screen_instance.queue_free()


func has_main_screen():
	return true


func make_visible(visible):
	if visible:
		add_control_to_dock(EditorPlugin.DOCK_SLOT_RIGHT_BL, commit_dock_instance)
		add_control_to_dock(EditorPlugin.DOCK_SLOT_RIGHT_UL, staging_dock_instance)
		hide_bottom_panel()
	else:
		remove_control_from_docks(commit_dock_instance)
		remove_control_from_docks(staging_dock_instance)
	if main_screen_instance:
		main_screen_instance.visible = visible


func get_plugin_name():
	return "Git History"


func get_plugin_icon():
	# Must return some kind of Texture for the icon.
	return preload("res://addons/git_for_godot/history_main_screen/Git-icon-white.svg")
