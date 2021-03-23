tool
extends Control

const AUTO_REFRESH_DELAY = 0.5
const BRANCH_ITEM_SCENE = preload("res://addons/git_for_godot/branch_dock/branch_item.tscn")
const VALID_BRANCH_REGEX = "^(?!@$|/|.*([/.]\\.|//|@\\{|\\\\))[^\\000-\\037\\177 ~^:?*[]+/[^\\000-\\037\\177 ~^:?*[]+(?<!\\.lock|[/.])$"

var auto_refresh_time = AUTO_REFRESH_DELAY
var force_refresh := true
var remote_dock
var simple_native

var _old_branch_dictionary
var _regex: RegEx

onready var _branch_list_vbox = get_node(@"BranchListPanel/ScrollContainer/BranchListVBox")
onready var _new_branch_hbox = get_node(@"NewBranch")
onready var _new_branch_name = _new_branch_hbox.get_node(@"BranchName")
onready var _new_branch_button = _new_branch_hbox.get_node(@"CreateButton")

func _ready():
	_new_branch_button.set_tooltip("Create a new branch on top\nof the checked out branch.")


func _process(delta):
	# Validate branch name each frame and disable the create button if invalid.
	if _regex and _new_branch_name:
		var result = _regex.search_all("refs/heads/" + _new_branch_name.text)
		_new_branch_button.disabled = result.empty() or (_new_branch_name.text in _old_branch_dictionary.keys())
	auto_refresh_time -= delta
	if force_refresh or auto_refresh_time < 0.0:
		auto_refresh_time += AUTO_REFRESH_DELAY
		check_for_update()


func check_for_update():
	var branch_dictionary
	if simple_native and simple_native.has_method("get_branch_list"):
		branch_dictionary = simple_native.get_branch_list()
	else:
		return # TODO: Further investigation is needed. Why does it fail?

	if not _regex:
		_regex = RegEx.new()
		_regex.compile(VALID_BRANCH_REGEX)
	# Tell the branch dock to update.
	if force_refresh or visible:
		update_status(branch_dictionary, force_refresh)
	# Tell the remote dock to update.
	if force_refresh or remote_dock.visible:
		remote_dock.update_status(branch_dictionary, force_refresh)
	force_refresh = false


func update_status(branch_dictionary, force_refresh):
	if (not force_refresh) and _old_branch_dictionary:
		if branch_dictionary.hash() == _old_branch_dictionary.hash():
			return # No need to redraw, it's the same as the old dictionary.
	_old_branch_dictionary = branch_dictionary

	# Delete old children.
	for i in _branch_list_vbox.get_children():
		i.free()

	# Add new children.
	for key in branch_dictionary.keys():
		var value = branch_dictionary[key]
		print(str(value) + " " + key)
		if value & LibGit2Defines.GitBranch.LOCAL:
			var branch_item_instance = BRANCH_ITEM_SCENE.instance()
			branch_item_instance.setup(simple_native, self, _regex, key, value)
			_branch_list_vbox.add_child(branch_item_instance)


func _on_CreateButton_pressed():
	# Validate branch name before creating.
	var result = _regex.search_all("refs/heads/" + _new_branch_name.text)
	if not (result.empty() or (_new_branch_name.text in _old_branch_dictionary.keys())):
		simple_native.create_branch(_new_branch_name.text)
