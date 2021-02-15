#include "simple.h"
#include "print.h"

typedef struct user_data_struct {
	char data[256];
	int major;
	int minor;
	int patch;
} user_data_struct;

git_repository *repo = NULL;

void validate_git_repo_is_initialized() {
	if (repo == NULL) {
		git_libgit2_init();
		git_repository_open(&repo, "/home/aaronfranke/workspace/GitForGodot/.git");
	}
}

void GDN_EXPORT godot_nativescript_init(void *p_handle) {
	godot_instance_create_func create = { NULL, NULL, NULL };
	create.create_func = &simple_constructor;

	godot_instance_destroy_func destroy = { NULL, NULL, NULL };
	destroy.destroy_func = &simple_destructor;

	nativescript_api->godot_nativescript_register_class(p_handle, "SIMPLE", "Reference", create, destroy);

	godot_method_attributes attributes = { GODOT_METHOD_RPC_MODE_DISABLED };

#define REGISTER_INSTANCE_METHOD(m_name) \
	godot_instance_method m_name = { NULL, NULL, NULL }; \
	m_name.method = &simple_##m_name; \
	nativescript_api->godot_nativescript_register_method(p_handle, "SIMPLE", #m_name, attributes, m_name);

	REGISTER_INSTANCE_METHOD(get_data);
	REGISTER_INSTANCE_METHOD(get_status);
	REGISTER_INSTANCE_METHOD(stage_all);
	REGISTER_INSTANCE_METHOD(unstage_all);
	REGISTER_INSTANCE_METHOD(discard_unstaged);
	REGISTER_INSTANCE_METHOD(commit);

#undef REGISTER_INSTANCE_METHOD
}

GDCALLINGCONV void *simple_constructor(godot_object *p_instance, void *p_method_data) {
	user_data_struct *user_data = api->godot_alloc(sizeof(user_data_struct));
	int *major, *minor, *patch;
	major = &user_data->major;
	minor = &user_data->minor;
	patch = &user_data->patch;
	git_libgit2_version(major, minor, patch);
	strcpy(user_data->data, "World from GDNative!");
	validate_git_repo_is_initialized();

	return user_data;
}

GDCALLINGCONV void simple_destructor(godot_object *p_instance, void *p_method_data, void *p_user_data) {
	api->godot_free(p_user_data);
}

godot_variant simple_get_data(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args) {
	godot_string data;
	godot_int libgit2_version_int;
	godot_variant ret;
	user_data_struct *user_data = (user_data_struct *)p_user_data;

	//api->godot_string_new(&data);
	//api->godot_string_parse_utf8(&data, user_data->data);
	//api->godot_variant_new_string(&ret, &data);
	//api->godot_string_destroy(&data);
	libgit2_version_int = 10000 * (user_data->major) + 100 * (user_data->minor) + user_data->patch;
	api->godot_variant_new_int(&ret, libgit2_version_int);

	return ret;
}

godot_variant simple_get_status(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args) {
	git_status_list *status_list;
	godot_variant ret;
	int err = git_status_list_new(&status_list, repo, NULL);
	git_index *index;
	git_repository_index(&index, repo);
	int ecount = git_index_entrycount(index);
	for (int i = 0; i < ecount; ++i) {
		const git_index_entry *e = git_index_get_byindex(index, i);

		print2(stos("path: "), stos(e->path));
		/*
		print2(stos("mtime: "), itos((int)e->mtime.seconds));
		print2(stos("fs: "), itos((int)e->file_size));
		*/
		git_oid id = e->id;
	}
	api->godot_variant_new_int(&ret, ecount);
	return ret;
}

godot_variant simple_stage_all(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args) {
	validate_git_repo_is_initialized();
	// Get the index of the repository.
	git_index *index;
	git_repository_index(&index, repo);
	// Glob pattern for all files.
	git_strarray array = {0};
	array.count = 1;
	array.strings = calloc(1, sizeof(char *));
	array.strings[0] = "*";
	// Add all files to the staging area.
	git_index_add_all(index, &array, 0, NULL, NULL);

	// Write in-memory changes to disk and clean up.
	git_index_write(index);
	git_index_free(index);
}

godot_variant simple_unstage_all(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args) {
	print(stos("unstage all print"));
	validate_git_repo_is_initialized();
	// Get the index of the repository.
	git_index *index;
	git_repository_index(&index, repo);

	// Write in-memory changes to disk and clean up.
	git_index_write(index);
	git_index_free(index);
}

godot_variant simple_discard_unstaged(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args) {
	print(stos("discard unstaged print"));
	validate_git_repo_is_initialized();
	// Get the index of the repository.
	git_index *index;
	git_repository_index(&index, repo);

	// Write in-memory changes to disk and clean up.
	git_index_write(index);
	git_index_free(index);
}

godot_variant simple_commit(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args) {
	print(stos("commit print"));
	validate_git_repo_is_initialized();
	// Get the index of the repository.
	git_index *index;
	git_repository_index(&index, repo);
	print(itos(p_num_args));
	print(godot_variant_as_string(p_args[0]));
	print(godot_variant_as_string(p_args[1]));
	print(godot_variant_as_string(p_args[2]));
	godot_bool amend = godot_variant_as_bool(p_args[0]);
	godot_char_string name = vtocs(p_args[1]);
	godot_char_string description = vtocs(p_args[2]);

	// Write in-memory changes to disk and clean up.
	git_index_write(index);
	git_index_free(index);
}
