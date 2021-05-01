tool
extends Control

const REMOTE_ITEM_SCENE = preload("res://addons/git_for_godot/docks/remote_dock/remote_item.tscn")

var _simple_native
var _dock_manager

var _old_branch_dictionary

onready var _create_popup: ConfirmationDialog = $NewRemote/CreateButton/CreatePopup
onready var _remote_name: LineEdit = $NewRemote/CreateButton/CreatePopup/VBoxContainer/RemoteName
onready var _remote_url: LineEdit = $NewRemote/CreateButton/CreatePopup/VBoxContainer/RemoteURL


func setup(simple_native, dock_manager):
	_simple_native = simple_native
	_dock_manager = dock_manager


func _process(_delta):
	# Validate remote name and URL each frame and disable the button if invalid.
	# TODO: This only checks if the names are empty for now.
	var should_be_disabled = _remote_name.text.empty() or _remote_url.text.empty()
	var ok = _create_popup.get_ok()
	if ok.disabled != should_be_disabled:
		ok.disabled = should_be_disabled


func update_status(branch_dictionary: Dictionary):
	# Free old children.
	for i in $RemoteListPanel/ScrollContainer/RemoteListVBox.get_children():
		i.free()

	var remotes = _simple_native.get_remote_list()

	# Add new children.
	for item in remotes:
		var remote_item_instance = REMOTE_ITEM_SCENE.instance()
		remote_item_instance.setup(_simple_native, branch_dictionary, item)
		$RemoteListPanel/ScrollContainer/RemoteListVBox.add_child(remote_item_instance)


func _on_CreateButton_pressed():
	_create_popup.popup_centered()


func _on_CreatePopup_confirmed():
	_simple_native.add_remote(_remote_name.text, _remote_url.text)
