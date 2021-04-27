tool
extends MarginContainer

var simple_native
var editor_plugin


func _ready():
	$ColorRect/MarginContainer/VBoxContainer/BasicActions.simple_native = simple_native
	$ColorRect/MarginContainer/VBoxContainer/ScrollContainer/GitHistory.setup(simple_native)
	$ColorRect/MarginContainer/VBoxContainer/BasicActions/Settings/SettingsPopup.get_node("VBoxContainer").setup(editor_plugin)
