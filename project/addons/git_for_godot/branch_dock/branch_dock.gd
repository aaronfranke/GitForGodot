tool
extends Control

const AUTO_REFRESH_DELAY = 0.5
const BRANCH_ITEM_SCENE = preload("res://addons/git_for_godot/branch_dock/branch_item.tscn")

var simple_native
var auto_refresh_time = AUTO_REFRESH_DELAY

var _old_branch_dictionary

onready var branch_list_vbox = get_node(@"BranchListPanel/ScrollContainer/BranchListVBox")

func _process(delta):
	auto_refresh_time -= delta
	if auto_refresh_time < 0.0:
		auto_refresh_time += AUTO_REFRESH_DELAY
		if visible:
			update_status()


func update_status():
	var branch_dictionary
	if simple_native and simple_native.has_method("get_branch_list"):
		branch_dictionary = simple_native.get_branch_list()
	else:
		return # TODO: Further investigation is needed. Why does it fail?
	if _old_branch_dictionary and branch_dictionary.hash() == _old_branch_dictionary.hash():
		return # No need to redraw, it's the same as the old dictionary.
	_old_branch_dictionary = branch_dictionary
	for i in branch_list_vbox.get_children():
		i.free()

	for key in branch_dictionary.keys():
		var value = branch_dictionary[key]
		print(str(value) + " " + key)
		if value & LibGit2Defines.GitBranch.LOCAL:
			var branch_item_instance = BRANCH_ITEM_SCENE.instance()
			branch_item_instance.simple_native = simple_native
			branch_item_instance.setup(key, value)
			branch_list_vbox.add_child(branch_item_instance)
