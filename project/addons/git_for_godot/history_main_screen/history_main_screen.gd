tool
extends MarginContainer

var simple_native
var editor_plugin


func _ready():
	$ColorRect/MarginContainer/VBoxContainer/BasicActions.simple_native = simple_native
	$ColorRect/MarginContainer/VBoxContainer/ScrollContainer/GitHistory.setup(simple_native)


func _on_AutoHideDocks_toggled(button_pressed):
	editor_plugin.auto_hide_docks = button_pressed
