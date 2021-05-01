tool
extends Node

var auto_refresh_branches_delay := 0.1
var auto_refresh_status_delay := 0.5
var force_refresh := true

var _editor_plugin
var _simple_native
var _branch_dock
var _commit_dock
var _remote_dock
var _staging_dock
var _auto_refresh_branches_time = auto_refresh_branches_delay
var _auto_refresh_status_time = auto_refresh_status_delay
var _old_branch_dictionary
var _old_status_dictionary


func _process(delta):
	if not _simple_native:
		printerr("DockManager: GDNative object is null!")
		if not _editor_plugin:
			printerr("DockManager: Fatal: Editor plugin is null!")
			return
		var editor_interface = _editor_plugin.get_editor_interface()
		yield(get_tree(), "idle_frame")
		editor_interface.set_plugin_enabled("git_for_godot", false)
		editor_interface.set_plugin_enabled("git_for_godot", true)
		return

	if force_refresh:
		_refresh_branches()
		_refresh_status()
		force_refresh = false
		return

	if auto_refresh_branches_delay > 0.0:
		_auto_refresh_branches_time -= delta
		if _auto_refresh_branches_time < 0.0:
			_auto_refresh_branches_time += auto_refresh_branches_delay
			# Refreshing the branches typically takes a small amount of time (< 1 ms).
			_refresh_branches()

	if auto_refresh_status_delay > 0.0:
		_auto_refresh_status_time -= delta
		if _auto_refresh_status_time < 0.0:
			_auto_refresh_status_time += auto_refresh_status_delay
			# Refreshing the status typically takes a large amount of time (hundreds of ms).
			_refresh_status()


func setup(editor_plugin, simple_native, branch_dock, commit_dock, remote_dock, staging_dock):
	_editor_plugin = editor_plugin
	_simple_native = simple_native
	_branch_dock = branch_dock
	_commit_dock = commit_dock
	_remote_dock = remote_dock
	_staging_dock = staging_dock
	_branch_dock.setup(_simple_native, self)
	_commit_dock.setup(_simple_native, self)
	_remote_dock.setup(_simple_native, self)
	_staging_dock.setup(_simple_native, self, _commit_dock)
	var wip = $"../ColorRect/MarginContainer/VBoxContainer/ScrollContainer/GitHistory/Commits/Names/WIP"
	_commit_dock.get_node(@"VBoxContainer/StageStatus").wip_node = wip


func _refresh_branches():
	if _simple_native.has_method("get_branch_list"):
		var branch_dictionary = _simple_native.get_branch_list()
		if _old_branch_dictionary:
			if branch_dictionary.hash() == _old_branch_dictionary.hash():
				return # No need to redraw, it's the same as the old dictionary.
		_old_branch_dictionary = branch_dictionary
		_branch_dock.update_status(branch_dictionary)
		_remote_dock.update_status(branch_dictionary)
	else:
		printerr("DockManager: get_branch_list failed!")


func _refresh_status():
	if _simple_native.has_method("get_status"):
		var status_dictionary = _simple_native.get_status()
		if (not force_refresh) and _old_status_dictionary:
			if status_dictionary.hash() == _old_status_dictionary.hash():
				return # No need to redraw, it's the same as the old dictionary.
		_staging_dock.update_status()
	else:
		printerr("DockManager: get_status failed!")
