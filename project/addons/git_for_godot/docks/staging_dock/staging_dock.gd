tool
extends Control

const staging_file_scene = preload("res://addons/git_for_godot/docks/staging_dock/staging_file.tscn")

var _simple_native
var _dock_manager
var _commit_dock_status_label

var _old_status_dictionary

var staged_file_count_label: Label
var unstaged_file_count_label: Label
var staged_files_vbox: VBoxContainer
var unstaged_files_vbox: VBoxContainer


func setup(simple_native, dock_manager, commit_dock):
	_simple_native = simple_native
	_dock_manager = dock_manager
	_commit_dock_status_label = commit_dock.get_child(0).get_node("StageStatus")
	staged_file_count_label = get_child(0).get_node(@"StagedStatus")
	unstaged_file_count_label = get_child(0).get_node(@"UnstagedStatus")
	staged_files_vbox = get_child(0).get_node(@"StagedFiles/ScrollContainer/VBoxContainer")
	unstaged_files_vbox = get_child(0).get_node(@"UnstagedFiles/ScrollContainer/VBoxContainer")


func update_status():
	var status_dictionary
	if _simple_native and _simple_native.has_method("get_status"):
		status_dictionary = _simple_native.get_status()
	else:
		return # TODO: Further investigation is needed. Why does it fail?
	if _old_status_dictionary:
		if status_dictionary.hash() == _old_status_dictionary.hash():
			return # No need to redraw, it's the same as the old dictionary.
	_old_status_dictionary = status_dictionary
	for i in staged_files_vbox.get_children():
		i.free()
	for i in unstaged_files_vbox.get_children():
		i.free()
	var staged_file_count := 0
	var unstaged_file_count := 0
	for key in status_dictionary.keys():
		var value = status_dictionary[key]
		if value & LibGit2Defines.GIT_STATUS_INDEX_CHANGES:
			staged_file_count += 1
			var file_instance = staging_file_scene.instance()
			file_instance.setup(_simple_native, _dock_manager, key, value & LibGit2Defines.GIT_STATUS_INDEX_CHANGES)
			staged_files_vbox.add_child(file_instance)
		if value & LibGit2Defines.GIT_STATUS_WT_CHANGES:
			unstaged_file_count += 1
			var file_instance = staging_file_scene.instance()
			file_instance.setup(_simple_native, _dock_manager, key, value & LibGit2Defines.GIT_STATUS_WT_CHANGES)
			unstaged_files_vbox.add_child(file_instance)

	# Update the commit dock with the current status.
	staged_file_count_label.text = str(staged_file_count) + " staged file" + ("" if staged_file_count == 1 else "s")
	unstaged_file_count_label.text = str(unstaged_file_count) + " unstaged file" + ("" if unstaged_file_count == 1 else "s")
	_commit_dock_status_label.set_staged_and_unstaged_file_counts(staged_file_count, unstaged_file_count)
