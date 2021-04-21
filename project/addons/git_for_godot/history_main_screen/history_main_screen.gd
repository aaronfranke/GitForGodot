tool
extends MarginContainer


func setup(editor_plugin, simple_native, branch_dock, commit_dock, remote_dock, staging_dock):
	var dock_manager = $DockManager
	$ColorRect/MarginContainer/VBoxContainer/BasicActions.simple_native = simple_native
	$ColorRect/MarginContainer/VBoxContainer/ScrollContainer/GitHistory.setup(simple_native, dock_manager)
	$ColorRect/MarginContainer/VBoxContainer/BasicActions/Settings/SettingsPopup.get_node("VBoxContainer").setup(editor_plugin, $DockManager)
	dock_manager.setup(editor_plugin, simple_native, branch_dock, commit_dock, remote_dock, staging_dock)
