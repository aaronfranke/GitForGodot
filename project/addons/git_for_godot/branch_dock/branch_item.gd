tool
extends Control

var simple_native


func setup(branch_name, branch_type):
	name = branch_name
	$Name.text = branch_name
	var checkout = $Checkout
	checkout.set_tooltip("Check out branch " + branch_name + ".\n\n" +
			"If any unstaged changes are present, \n" +
			"those files will be preserved, and\n" +
			"any staged changes will be discarded.\n" +
			"Consider discarding all uncommitted\n" +
			"changes before switching branches.\n")
	if branch_type & LibGit2Defines.GitBranch.HEAD:
		checkout.texture_normal = load("res://addons/git_for_godot/icons/Check.svg")
		checkout.texture_hover = load("res://addons/git_for_godot/icons/CheckHover.svg")
		checkout.texture_pressed = load("res://addons/git_for_godot/icons/CheckHover.svg")


func _on_Checkout_pressed():
	simple_native.checkout_branch(name)


func _on_Menu_pressed():
	print("menu")
