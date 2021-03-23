tool
extends Control

const AUTO_REFRESH_DELAY = 0.5
const staging_file_scene = preload("res://addons/git_for_godot/staging_dock/staging_file.tscn")

var simple_native
var commit_dock

var auto_refresh_time = AUTO_REFRESH_DELAY
var force_refresh := true

var _old_status_dictionary

onready var staged_file_count_label = get_child(0).get_node(@"StagedStatus")
onready var unstaged_file_count_label = get_child(0).get_node(@"UnstagedStatus")
onready var staged_files_vbox = get_child(0).get_node(@"StagedFiles/ScrollContainer/VBoxContainer")
onready var unstaged_files_vbox = get_child(0).get_node(@"UnstagedFiles/ScrollContainer/VBoxContainer")

func _process(delta):
	auto_refresh_time -= delta
	if force_refresh or auto_refresh_time < 0.0:
		auto_refresh_time += AUTO_REFRESH_DELAY
		if visible or commit_dock.visible:
			update_status()


func update_status():
	var status_dictionary
	if simple_native and simple_native.has_method("get_status"):
		status_dictionary = simple_native.get_status()
	else:
		return # TODO: Further investigation is needed. Why does it fail?
	if (not force_refresh) and _old_status_dictionary:
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
		#print(str(value) + " " + key)
		if value & LibGit2Defines.GIT_STATUS_INDEX_CHANGES:
			staged_file_count += 1
			var staging_file_instance = staging_file_scene.instance()
			staging_file_instance.setup(key, value & LibGit2Defines.GIT_STATUS_INDEX_CHANGES)
			staged_files_vbox.add_child(staging_file_instance)
		if value & LibGit2Defines.GIT_STATUS_WT_CHANGES:
			unstaged_file_count += 1
			var staging_file_instance = staging_file_scene.instance()
			staging_file_instance.simple_native = simple_native
			staging_file_instance.setup(key, value & LibGit2Defines.GIT_STATUS_WT_CHANGES)
			unstaged_files_vbox.add_child(staging_file_instance)

	# Update the commit dock with the current status.
	staged_file_count_label.text = str(staged_file_count) + " staged file" + ("" if staged_file_count == 1 else "s")
	unstaged_file_count_label.text = str(unstaged_file_count) + " unstaged file" + ("" if unstaged_file_count == 1 else "s")
	var commit_dock_status_label = commit_dock.get_child(0).get_node("StageStatus")
	commit_dock_status_label.set_staged_and_unstaged_file_counts(staged_file_count, unstaged_file_count)
	force_refresh = false
