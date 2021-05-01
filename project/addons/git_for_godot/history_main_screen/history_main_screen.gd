tool
extends MarginContainer


func setup(editor_plugin, simple_native, branch_dock, commit_dock, remote_dock, staging_dock):
	$ColorRect/MarginContainer/VBoxContainer/BasicActions.simple_native = simple_native
	$ColorRect/MarginContainer/VBoxContainer/ScrollContainer/GitHistory.setup(simple_native)
	$ColorRect/MarginContainer/VBoxContainer/BasicActions/Settings/SettingsPopup.get_node("VBoxContainer").setup(editor_plugin, $DockManager)
	$DockManager.setup(editor_plugin, simple_native, branch_dock, commit_dock, remote_dock, staging_dock)
