tool
extends MarginContainer

var simple_native


func _ready():
	$ColorRect/MarginContainer/VBoxContainer/BasicActions.simple_native = simple_native
	$ColorRect/MarginContainer/VBoxContainer/ScrollContainer/GitHistory.setup(simple_native)
