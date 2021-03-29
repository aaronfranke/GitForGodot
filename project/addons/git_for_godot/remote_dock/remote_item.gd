tool
extends Control

const REMOTE_BRANCH_SCENE = preload("res://addons/git_for_godot/remote_dock/remote_branch.tscn")

# These are set in setup.
var _simple_native
var _remote_name: String
var _rename_popup: ConfirmationDialog
var _rename_name: LineEdit
var _rename_ok: Button
var _delete_popup: ConfirmationDialog
var _change_urls_popup: ConfirmationDialog

onready var menu_button = $RemoteDetails/Menu
onready var popup_menu = $RemoteDetails/PopupMenu


func setup(simple_native, branch_dictionary: Dictionary, remote_name: String):
	_simple_native = simple_native
	_remote_name = remote_name
	name = remote_name
	$RemoteDetails/Name.text = remote_name
	_rename_popup = $RemoteDetails/RenamePopup
	_rename_name = _rename_popup.get_node(@"Name")
	_rename_name.text = remote_name
	_rename_ok = _rename_popup.get_ok()
	_delete_popup = $RemoteDetails/DeletePopup
	_delete_popup.dialog_text = "Are you sure you wish to delete remote " + remote_name + "?"
	_delete_popup.get_ok().text = "Delete"
	_change_urls_popup = $RemoteDetails/ChangeURLsPopup
	print("remote item " + remote_name)

	for key in branch_dictionary.keys():
		var value = branch_dictionary[key]
		print("remote item " + str(value) + " " + key)
		if value & LibGit2Defines.GitBranch.REMOTE:
			if key.begins_with(remote_name):
				var remote_branch_name = key.substr(remote_name.length() + 1)
				var remote_branch_instance = REMOTE_BRANCH_SCENE.instance()
				remote_branch_instance.setup(remote_branch_name, value)
				$RemoteBranchList.add_child(remote_branch_instance)


func _on_Menu_pressed():
	menu_button.release_focus()
	popup_menu.popup(Rect2(menu_button.rect_global_position + Vector2(24, 0), popup_menu.rect_size))


func _on_RenamePopup_confirmed():
	print("_on_RenamePopup_confirmed")


func _on_DeletePopup_confirmed():
	print("_on_DeletePopup_confirmed")


func _on_ChangeURLsPopup_confirmed():
	print("_on_ChangeURLsPopup_confirmed")


func _on_PopupMenu_id_pressed(id):
	match id:
		0: # Fetch remote
			_simple_native.fetch_one(_remote_name)
		1: # Rename remote
			_rename_popup.popup_centered()
		2: # Delete remote
			_delete_popup.popup_centered()
		3: # Change URLs
			_change_urls_popup.popup_centered()
