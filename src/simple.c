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

#define REGISTER_INSTANCE_METHOD(m_name)                 \
	godot_instance_method m_name = { NULL, NULL, NULL }; \
	m_name.method = &simple_##m_name;                    \
	nativescript_api->godot_nativescript_register_method(p_handle, "SIMPLE", #m_name, attributes, m_name);

	REGISTER_INSTANCE_METHOD(get_data);
	REGISTER_INSTANCE_METHOD(get_status);
	REGISTER_INSTANCE_METHOD(stage_one);
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
	validate_git_repo_is_initialized();
	// Get the index of the repository.
	git_index *index;
	git_repository_index(&index, repo);
	git_status_list *status_list;
	int64_t err = git_status_list_new(&status_list, repo, NULL);
	if (err) {
		//return;
	}
	int64_t status = (int64_t)git_status_list_entrycount(status_list);
	godot_dictionary all_changes;
	godot_dictionary_new(&all_changes);
	for (int i = 0; i < status; i++) {
		const git_status_entry *status_entry = git_status_byindex(status_list, i);
		if (status_entry->status & (GIT_STATUS_INDEX_CHANGES | GIT_STATUS_WT_CHANGES)) {
			godot_variant status;
			godot_variant_new_int(&status, status_entry->status);
			godot_string path_string;
			if (status_entry->status & GIT_STATUS_INDEX_CHANGES) {
				path_string = cptos(status_entry->head_to_index->new_file.path);
			} else { // status_entry->status & GIT_STATUS_WT_CHANGES
				path_string = cptos(status_entry->index_to_workdir->new_file.path);
			}
			godot_variant path_variant;
			godot_variant_new_string(&path_variant, &path_string);
			godot_dictionary_set(&all_changes, &path_variant, &status);
			// Clean up.
			godot_string_destroy(&path_string);
			godot_variant_destroy(&path_variant);
			godot_variant_destroy(&status);
		}
	}
	godot_variant all_changes_variant;
	godot_variant_new_dictionary(&all_changes_variant, &all_changes);
	godot_dictionary_destroy(&all_changes);
	return all_changes_variant;
}

godot_variant simple_stage_one(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args) {
	validate_git_repo_is_initialized();
	// Get the index of the repository.
	git_index *index;
	git_repository_index(&index, repo);
	// Get file path argument.
	godot_variant *arg = p_args[0];
	godot_string path_string = godot_variant_as_string(arg);
	godot_char_string path_char_string = stocs(&path_string);
	const char *path_charptr = godot_char_string_get_data(&path_char_string);
	// Glob pattern for this file path.
	git_strarray array = { 0 };
	array.count = 1;
	array.strings = calloc(1, sizeof(char *));
	array.strings[0] = (char *)path_charptr;
	// Add all files to the staging area.
	git_index_add_all(index, &array, 0, NULL, NULL);

	// Write in-memory changes to disk and clean up.
	git_index_write(index);
	git_index_free(index);
	godot_string_destroy(&path_string);
	godot_variant_destroy(arg);
}

godot_variant simple_stage_all(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args) {
	validate_git_repo_is_initialized();
	// Get the index of the repository.
	git_index *index;
	git_repository_index(&index, repo);
	// Glob pattern for all files.
	git_strarray array = { 0 };
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
	print(cptos("unstage all print"));
	validate_git_repo_is_initialized();
	// Get the index of the repository.
	git_index *index;
	git_repository_index(&index, repo);
	// Get information for the current HEAD of the repository.
	git_oid head_oid;
	git_reference_name_to_id(&head_oid, repo, "HEAD");
	git_commit *head_commit;
	git_commit_lookup(&head_commit, repo, &head_oid);
	git_tree *head_tree;
	git_commit_tree(&head_tree, head_commit);
	git_index_read_tree(index, head_tree);
	// Write in-memory changes to disk and clean up.
	git_index_write(index);
	git_index_free(index);
	git_tree_free(head_tree);
	git_commit_free(head_commit);
}

godot_variant simple_discard_unstaged(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args) {
	print(cptos("discard unstaged print"));
	validate_git_repo_is_initialized();
	// Get the index of the repository.
	git_index *index;
	git_repository_index(&index, repo);

	// Write in-memory changes to disk and clean up.
	git_index_write(index);
	git_index_free(index);
}

godot_variant simple_commit(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args) {
	print(cptos("commit print"));
	validate_git_repo_is_initialized();
	// Get the index of the repository.
	git_index *index;
	git_repository_index(&index, repo);
	print(itos(p_num_args));
	//print(godot_variant_as_string(p_args[0]));
	//print(godot_variant_as_string(p_args[1]));
	//print(godot_variant_as_string(p_args[2]));
	godot_bool amend = godot_variant_as_bool(p_args[0]);
	// Parse the commit message.
	godot_string newline = cptos("\n");
	godot_string name = godot_variant_as_string(p_args[1]);
	godot_string desc = godot_variant_as_string(p_args[2]);
	godot_string message_str = godot_string_operator_plus(&name, &newline);
	message_str = godot_string_operator_plus(&message_str, &desc);
	godot_char_string message_cs = stocs(&message_str);
	const char *message = api->godot_char_string_get_data(&message_cs);
	//print(cptos(message));
	// Get information for the tree of the commit.
	git_oid index_tree_oid;
	git_index_write_tree(&index_tree_oid, index);
	git_tree *index_tree;
	git_tree_lookup(&index_tree, repo, &index_tree_oid);
	// Get information for the current HEAD of the repository.
	git_oid parent_oid;
	git_reference_name_to_id(&parent_oid, repo, "HEAD");
	git_commit *parent;
	git_commit_lookup(&parent, repo, &parent_oid);
	// Open Git config.
	git_config *config;
	git_repository_config_snapshot(&config, repo);
	// Committer information.
	const char *user_name;
	if (git_config_get_string(&user_name, config, "user.name")) {
		user_name = "Git User";
	}
	const char *user_email;
	if (git_config_get_string(&user_email, config, "user.email")) {
		user_email = "test@example.com";
	}
	// Get the time.
	godot_object *os_singleton = godot_global_get_singleton("OS");
	godot_method_bind *os_get_unix_time = api->godot_method_bind_get_method("_OS", "get_unix_time");
	int64_t unix_time;
	api->godot_method_bind_ptrcall(os_get_unix_time, os_singleton, NULL, &unix_time);
	// Set up the signature.
	git_signature *committer;
	git_signature_new(&committer, user_name, user_email, unix_time, 0);
	git_oid new_commit_oid;
	// Create the commit.
	if (amend) {
		const git_signature *author = git_commit_author(parent);
		git_commit_amend(&new_commit_oid, parent, "HEAD", author, committer, "UTF-8", message, index_tree);
	} else {
		git_signature *author = committer;
		const git_commit **parents = (const git_commit **)&parent;
		git_commit_create(&new_commit_oid, repo, "HEAD", author, committer, "UTF-8", message, index_tree, 1, parents);
	}
	// Clean up memory.
	godot_char_string_destroy(&message_cs);
	godot_string_destroy(&message_str);
	godot_string_destroy(&newline);
	git_signature_free(committer);
	git_tree_free(index_tree);
	git_commit_free(parent);
	git_index_free(index);
}
