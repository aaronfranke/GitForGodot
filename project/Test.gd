extends Node

const data = preload("res://addons/git_for_godot/gdnative/simple.gdns")


func _ready():
	var d = data.new()
	print("Data = " + str(d.get_data()))
	print("thing = " + str(d.get_status(true)))
