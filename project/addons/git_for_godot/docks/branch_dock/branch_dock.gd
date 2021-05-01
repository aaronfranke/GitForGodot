tool
extends Control

const BRANCH_ITEM_SCENE = preload("res://addons/git_for_godot/docks/branch_dock/branch_item.tscn")
const VALID_BRANCH_REGEX = "^(?!@$|/|.*([/.]\\.|//|@\\{|\\\\))[^\\000-\\037\\177 ~^:?*[]+/[^\\000-\\037\\177 ~^:?*[]+(?<!\\.lock|[/.])$"

var _simple_native
var _dock_manager

var _old_branch_dictionary
var _regex: RegEx

var _branch_list_vbox: VBoxContainer
var _new_branch_hbox: HBoxContainer
var _new_branch_name: LineEdit
var _new_branch_button: Button


func setup(simple_native, dock_manager):
	_simple_native = simple_native
	_dock_manager = dock_manager
	_branch_list_vbox = get_node(@"BranchListPanel/ScrollContainer/BranchListVBox")
	_new_branch_hbox = get_node(@"NewBranch")
	_new_branch_name = _new_branch_hbox.get_node(@"BranchName")
	_new_branch_button = _new_branch_hbox.get_node(@"CreateButton")
	_new_branch_button.set_tooltip("Create a new branch on top\nof the checked out branch.")


func _process(delta):
	# Validate branch name each frame and disable the create button if invalid.
	if not _regex:
		_regex = RegEx.new()
		_regex.compile(VALID_BRANCH_REGEX)
	if _new_branch_name:
		var result = _regex.search_all("refs/heads/" + _new_branch_name.text)
		var invalid: bool = result.empty() or (_new_branch_name.text in _old_branch_dictionary.keys())
		_new_branch_button.disabled = invalid
		if invalid and not _new_branch_name.text.empty():
			_new_branch_name.add_color_override("font_color", Color(1, 0.5, 0.5))
		else:
			_new_branch_name.add_color_override("font_color", Color.white)


func update_status(branch_dictionary):
	_old_branch_dictionary = branch_dictionary
	# Delete old children.
	for i in _branch_list_vbox.get_children():
		i.free()

	# Add new children.
	for key in branch_dictionary.keys():
		var value = branch_dictionary[key]
		if value & LibGit2Defines.GitBranch.LOCAL:
			var branch_item_instance = BRANCH_ITEM_SCENE.instance()
			branch_item_instance.setup(_simple_native, _dock_manager, self, _regex, key, value)
			_branch_list_vbox.add_child(branch_item_instance)


func _on_CreateButton_pressed():
	# Validate branch name before creating.
	var result = _regex.search_all("refs/heads/" + _new_branch_name.text)
	if not (result.empty() or (_new_branch_name.text in _old_branch_dictionary.keys())):
		_simple_native.create_branch(_new_branch_name.text)
		_new_branch_name.text = ""
		_dock_manager.force_refresh = true
