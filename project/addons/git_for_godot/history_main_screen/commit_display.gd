tool
extends Control

var commit_time
var commit_hash


func setup(commit_time_, system_unix_time, commit_name, commit_hash_):
	commit_time = commit_time_
	commit_hash = commit_hash_
	$HBoxContainer/Name.text = commit_name
	hint_tooltip = commit_hash
	name = commit_hash
	var local_unix_timestamp = OS.get_unix_time_from_datetime(OS.get_datetime())
	var local_time_offset_from_utc = local_unix_timestamp - OS.get_unix_time()
	var datetime = OS.get_datetime_from_unix_time(commit_time + local_time_offset_from_utc)
	if abs(system_unix_time - commit_time) > 80000:
		$HBoxContainer/Time.text = "%s-%02d-%02d" % [datetime.year, datetime.month, datetime.day]
	else:
		$HBoxContainer/Time.text = "%s:%02d:%02d" % [datetime.hour, datetime.minute, datetime.second]
