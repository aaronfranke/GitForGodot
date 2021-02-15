tool
extends Control

var simple_native

func _ready():
	pass


func _process(delta):
	#print(visible)
	pass


func _on_Stage_pressed():
	simple_native.stage_all()


func _on_Unstage_pressed():
	simple_native.unstage_all()
