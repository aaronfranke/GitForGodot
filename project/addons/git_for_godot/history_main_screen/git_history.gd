tool
extends HSplitContainer

var _simple_native


func setup(simple_native):
	_simple_native = simple_native
	simple_native.get_all_commits_dictionary(100)
