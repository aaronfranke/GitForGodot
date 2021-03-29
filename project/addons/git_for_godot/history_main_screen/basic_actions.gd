tool
extends HBoxContainer

var simple_native


func _on_Fetch_pressed():
	print(simple_native)
	print("_on_Fetch_pressed")
	simple_native.fetch_all()


func _on_Pull_pressed():
	print("_on_Pull_pressed")


func _on_Push_pressed():
	print("_on_Push_pressed")
