tool
extends Control


func setup(branch_name, branch_type):
	name = branch_name
	$Name.text = branch_name
	var checkout = $Checkout
	checkout.set_tooltip("Check out branch " + branch_name + ".")


func _on_Checkout_pressed():
	pass


func _on_Menu_pressed():
	print("menu")
