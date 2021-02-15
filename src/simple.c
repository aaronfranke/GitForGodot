#include "simple.h"
#include "print.h"

typedef struct user_data_struct {
	char data[256];
	int major;
	int minor;
	int patch;
} user_data_struct;

git_repository *repo = NULL;

void GDN_EXPORT godot_nativescript_init(void *p_handle) {
	godot_instance_create_func create = { NULL, NULL, NULL };
	create.create_func = &simple_constructor;

	godot_instance_destroy_func destroy = { NULL, NULL, NULL };
	destroy.destroy_func = &simple_destructor;

	nativescript_api->godot_nativescript_register_class(p_handle, "SIMPLE", "Reference", create, destroy);

	godot_instance_method get_data = { NULL, NULL, NULL };
	get_data.method = &simple_get_data;

	godot_instance_method get_status = { NULL, NULL, NULL };
	get_status.method = &simple_get_status;

	godot_method_attributes attributes = { GODOT_METHOD_RPC_MODE_DISABLED };

	nativescript_api->godot_nativescript_register_method(p_handle, "SIMPLE", "get_data", attributes, get_data);
	nativescript_api->godot_nativescript_register_method(p_handle, "SIMPLE", "get_status", attributes, get_status);
}

GDCALLINGCONV void *simple_constructor(godot_object *p_instance, void *p_method_data) {
	user_data_struct *user_data = api->godot_alloc(sizeof(user_data_struct));
	int *major, *minor, *patch;
	major = &user_data->major;
	minor = &user_data->minor;
	patch = &user_data->patch;
	git_libgit2_version(major, minor, patch);
	strcpy(user_data->data, "World from GDNative!");
	git_libgit2_init();
	git_repository_open(&repo, "/home/aaronfranke/workspace/GitForGodot/.git");

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
