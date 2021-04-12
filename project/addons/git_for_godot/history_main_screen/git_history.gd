tool
extends HSplitContainer

# Keep this in sync with the "git_commit_to_godot_array" method in the C code.
enum CommitDictionaryIndex {
	TIMESTAMP = 0,
	SUMMARY = 1,
	BODY = 2,
	AUTHOR_EMAIL = 3,
	AUTHOR_NAME = 4,
	AUTHOR_TIME = 5,
	COMMITTER_EMAIL = 6,
	COMMITTER_NAME = 7,
	COMMITTER_TIME = 8,
	PARENT_COUNT = 9,
	PARENT_FIRST = 10,
}

const AUTO_REFRESH_DELAY = 2.0
const BRANCH_DISPLAY_SCENE = preload("res://addons/git_for_godot/history_main_screen/branch_display.tscn")
const COMMIT_DISPLAY_SCENE = preload("res://addons/git_for_godot/history_main_screen/commit_display.tscn")

var auto_refresh_time = AUTO_REFRESH_DELAY
var force_refresh := true

var _simple_native
var _old_commit_dictionary
var _branches_holder
var _names_vbox
var _placeholder_node


func _process(delta):
	auto_refresh_time -= delta
	if force_refresh or auto_refresh_time < 0.0:
		auto_refresh_time += AUTO_REFRESH_DELAY
		check_for_update()


func setup(simple_native):
	_simple_native = simple_native
	_branches_holder = $Branches
	_names_vbox = $Commits/Names
	_placeholder_node = _names_vbox.get_child(0)


func check_for_update():
	var commit_dictionary
	if _simple_native and _simple_native.has_method("get_all_commits_dictionary"):
		commit_dictionary = _simple_native.get_all_commits_dictionary(100)
	else:
		return # TODO: Further investigation is needed. Why does it fail?

	if force_refresh or visible:
		update_status(commit_dictionary, force_refresh)
	force_refresh = false


func update_status(commit_dictionary, force_refresh):
	if (not force_refresh) and _old_commit_dictionary:
		if commit_dictionary.hash() == _old_commit_dictionary.hash():
			return # No need to redraw, it's the same as the old dictionary.
	_old_commit_dictionary = commit_dictionary

	# Delete old children.
	for i in _names_vbox.get_children():
		if i.name != "Placeholder":
			i.free()
	for i in _branches_holder.get_children():
		i.free()

	# Add new children.
	var system_unix_time = OS.get_unix_time()
	for key in commit_dictionary.keys():
		if key == "branch_heads_dictionary":
			continue
		# Create the commit display instance.
		var commit_array = commit_dictionary[key]
		var commit_display = COMMIT_DISPLAY_SCENE.instance()
		var timestamp = commit_array[CommitDictionaryIndex.TIMESTAMP]
		commit_display.setup(timestamp, system_unix_time, commit_array[CommitDictionaryIndex.SUMMARY], key)
		# Where should we add this child? We want chronological order.
		# Check all children and add where it's most appropriate.
		var target_node = _placeholder_node
		for i in range(1, _names_vbox.get_child_count()):
			var child = _names_vbox.get_child(i)
			var child_timestamp = child.commit_time
			if child_timestamp > timestamp:
				target_node = child
		_names_vbox.add_child_below_node(target_node, commit_display)
	# Add branches next to the commits at the head.
	var branch_heads_dictionary = commit_dictionary["branch_heads_dictionary"]
	for key in branch_heads_dictionary.keys():
		var branch_head_hash = branch_heads_dictionary[key]
		var branch_display = BRANCH_DISPLAY_SCENE.instance()
		branch_display.setup(key)
		branch_display.anchor_right = 1
		branch_display.margin_right = 0
		_branches_holder.add_child(branch_display)
		# Figure out which commit we should place the branch next to.
		for i in range(1, _names_vbox.get_child_count()):
			var child = _names_vbox.get_child(i)
			if branch_head_hash == child.commit_hash:
				branch_display.commit = child
				continue
