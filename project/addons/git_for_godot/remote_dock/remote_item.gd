tool
extends Control

const REMOTE_BRANCH_SCENE = preload("res://addons/git_for_godot/remote_dock/remote_branch.tscn")


func setup(branch_dictionary: Dictionary, remote_name: String):
	name = remote_name
	$RemoteDetails/Name.text = name
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
	print("menu")
