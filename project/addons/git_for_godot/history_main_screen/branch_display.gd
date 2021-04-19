tool
extends Control

var commit

var _branch_name


func _process(_delta):
	if commit and margin_top != commit.margin_top:
		margin_top = commit.margin_top


func setup(branch_name):
	_branch_name = branch_name
	name = branch_name
	$HBoxContainer/BranchName.text = branch_name
