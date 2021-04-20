tool
extends HBoxContainer

var simple_native

onready var _settings_popup = $Settings/SettingsPopup


func _on_Fetch_pressed():
	simple_native.fetch_all()


func _on_Pull_pressed():
	print("_on_Pull_pressed")


func _on_Push_pressed():
	print("_on_Push_pressed")
	simple_native.push()


func _on_Settings_pressed():
	_settings_popup.popup_centered()
