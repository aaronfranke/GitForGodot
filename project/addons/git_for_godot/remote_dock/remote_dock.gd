tool
extends Control

const REMOTE_ITEM_SCENE = preload("res://addons/git_for_godot/remote_dock/remote_item.tscn")

var branch_dock
var simple_native

var _old_branch_dictionary


func update_status(branch_dictionary: Dictionary):
	if _old_branch_dictionary and branch_dictionary.hash() == _old_branch_dictionary.hash():
		return # No need to redraw, it's the same as the old dictionary.
	_old_branch_dictionary = branch_dictionary

	# Free old children.
	for i in $RemoteListPanel/ScrollContainer/RemoteListVBox.get_children():
		i.free()

	# Make a dictionary with only one entry for each remote.
	var remotes_dict = Dictionary()
	for key in branch_dictionary.keys():
		var value = branch_dictionary[key]
		if value & LibGit2Defines.GitBranch.REMOTE:
			var remote_name = key.split("/", true, 1)[0]
			remotes_dict[remote_name] = value

	# Add new children.
	for key in remotes_dict.keys():
		var value = remotes_dict[key]
		print("remote dock " + str(value) + " " + key)
		if value & LibGit2Defines.GitBranch.REMOTE:
			var remote_item_instance = REMOTE_ITEM_SCENE.instance()
			remote_item_instance.setup(branch_dictionary, key)
			$RemoteListPanel/ScrollContainer/RemoteListVBox.add_child(remote_item_instance)
