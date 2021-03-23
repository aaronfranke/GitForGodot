tool
extends Control

const REMOTE_ITEM_SCENE = preload("res://addons/git_for_godot/remote_dock/remote_item.tscn")

var branch_dock
var simple_native

var _old_branch_dictionary


func update_status(branch_dictionary: Dictionary, force_refresh: bool):
	if (not force_refresh) and _old_branch_dictionary:
		if branch_dictionary.hash() == _old_branch_dictionary.hash():
			return # No need to redraw, it's the same as the old dictionary.
	_old_branch_dictionary = branch_dictionary

	# Free old children.
	for i in $RemoteListPanel/ScrollContainer/RemoteListVBox.get_children():
		i.free()

	var remotes = simple_native.get_remote_list()

	# Add new children.
	for item in remotes:
		var remote_item_instance = REMOTE_ITEM_SCENE.instance()
		remote_item_instance.setup(branch_dictionary, item)
		$RemoteListPanel/ScrollContainer/RemoteListVBox.add_child(remote_item_instance)
