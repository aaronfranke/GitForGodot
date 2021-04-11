tool
extends Control

const REMOTE_ITEM_SCENE = preload("res://addons/git_for_godot/remote_dock/remote_item.tscn")

var branch_dock
var simple_native

var _old_branch_dictionary

onready var _create_popup: ConfirmationDialog = $NewRemote/CreateButton/CreatePopup
onready var _remote_name: LineEdit = $NewRemote/CreateButton/CreatePopup/VBoxContainer/RemoteName
onready var _remote_url: LineEdit = $NewRemote/CreateButton/CreatePopup/VBoxContainer/RemoteURL


func _process(_delta):
	# Validate remote name and URL each frame and disable the button if invalid.
	# TODO: This only checks if the names are empty for now.
	_create_popup.get_ok().disabled = _remote_name.text.empty() or _remote_url.text.empty()


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
		remote_item_instance.setup(simple_native, branch_dictionary, item)
		$RemoteListPanel/ScrollContainer/RemoteListVBox.add_child(remote_item_instance)


func _on_CreateButton_pressed():
	_create_popup.popup_centered()


func _on_CreatePopup_confirmed():
	simple_native.add_remote(_remote_name.text, _remote_url.text)
