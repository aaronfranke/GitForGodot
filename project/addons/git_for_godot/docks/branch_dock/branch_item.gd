tool
extends Control

# These are set in setup.
var _simple_native
var _dock_manager
var _branch_name: String
var _branch_dock: Control
var _rename_popup: ConfirmationDialog
var _rename_name: LineEdit
var _rename_ok: Button
var _delete_popup: ConfirmationDialog
var _upstream_popup: ConfirmationDialog
var _upstream_remote: OptionButton
var _upstream_branch: LineEdit
var _regex: RegEx

onready var menu_button = $Menu
onready var popup_menu = $PopupMenu


func _process(_delta):
	if not _regex:
		return
	if _rename_name:
		var result = _regex.search_all("refs/heads/" + _rename_name.text)
		_rename_ok.disabled = result.empty()


func setup(simple_native, dock_manager, branch_dock, regex, branch_name, branch_type):
	_simple_native = simple_native
	_dock_manager = dock_manager
	_branch_dock = branch_dock
	_branch_name = branch_name
	name = branch_name
	$Name.text = branch_name
	_rename_popup = $RenamePopup
	_rename_name = _rename_popup.get_node(@"Name")
	_rename_name.text = branch_name
	_rename_ok = _rename_popup.get_ok()
	_delete_popup = $DeletePopup
	_delete_popup.dialog_text = "Are you sure you wish to delete branch " + branch_name + "?"
	_delete_popup.get_ok().text = "Delete"
	_upstream_popup = $UpstreamPopup
	var upstream_hbox = _upstream_popup.get_node(@"VBoxContainer/Upstream")
	_upstream_remote = upstream_hbox.get_node(@"Remote")
	_upstream_branch = upstream_hbox.get_node(@"Branch")
	var remotes: Array = simple_native.get_remote_list()
	var upstream_name: String = simple_native.get_upstream_branch(branch_name)
	if not upstream_name.empty():
		var upstream_name_parts = upstream_name.split("/", true, 1)
		_upstream_branch.text = upstream_name_parts[1]
		_upstream_remote.add_item(upstream_name_parts[0])
		remotes.erase(upstream_name_parts[0])
	for item in remotes:
		_upstream_remote.add_item(item)
	var checkout = $Checkout
	checkout.set_tooltip("Check out branch " + branch_name + ".\n\n" +
			"If any unstaged changes are present,\n" +
			"those files will be preserved, and\n" +
			"any staged changes will be discarded.\n" +
			"Consider discarding all uncommitted\n" +
			"changes before switching branches.\n")
	if branch_type & LibGit2Defines.GitBranch.HEAD:
		checkout.texture_normal = load("res://addons/git_for_godot/icons/Check.svg")
		checkout.texture_hover = load("res://addons/git_for_godot/icons/CheckHover.svg")
		checkout.texture_pressed = load("res://addons/git_for_godot/icons/CheckHover.svg")
	_regex = regex


func _on_Checkout_pressed():
	_simple_native.checkout_branch(name)
	_dock_manager.force_refresh = true


func _on_Menu_pressed():
	menu_button.release_focus()
	popup_menu.popup(Rect2(menu_button.rect_global_position + Vector2(24, 0), popup_menu.rect_size))


func _on_PopupMenu_id_pressed(id):
	match id:
		0: # Rename branch
			_rename_popup.popup_centered()
		1: # Delete branch
			_delete_popup.popup_centered()
		2: # Change upstream
			_upstream_popup.popup_centered()


func _on_RenamePopup_confirmed():
	var result = _regex.search_all("refs/heads/" + _rename_name.text)
	if not result.empty():
		_simple_native.rename_branch(_branch_name, _rename_name.text)
		# Make the branch and remote docks refresh next frame.
		_dock_manager.force_refresh = true


func _on_DeletePopup_confirmed():
	_simple_native.delete_branch(_branch_name)
	# Make the branch and remote docks refresh next frame.
	_dock_manager.force_refresh = true


func _on_UpstreamPopup_confirmed():
	var upstream_branch_name = _upstream_remote.get_item_text(_upstream_remote.get_item_index(_upstream_remote.get_selected_id()))
	upstream_branch_name += "/" + _upstream_branch.text
	_simple_native.set_upstream_branch(_branch_name, upstream_branch_name)
	# Make the branch and remote docks refresh next frame.
	_dock_manager.force_refresh = true
