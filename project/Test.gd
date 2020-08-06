extends Node

const data = preload("res://addons/git_for_godot/gdnative/simple.gdns")


func _ready():
	print("Data = " + data.new().get_data())
	pass
