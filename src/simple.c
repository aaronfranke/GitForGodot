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
	REGISTER_INSTANCE_METHOD(get_head_message);
	REGISTER_INSTANCE_METHOD(get_branch_list);
	REGISTER_INSTANCE_METHOD(checkout_branch);
	REGISTER_INSTANCE_METHOD(create_branch);
	REGISTER_INSTANCE_METHOD(rename_branch);
	REGISTER_INSTANCE_METHOD(delete_branch);
	REGISTER_INSTANCE_METHOD(get_upstream_branch);
	REGISTER_INSTANCE_METHOD(set_upstream_branch);

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

INSTANCE_METHOD(get_data) {
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

INSTANCE_METHOD(get_status) {
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

INSTANCE_METHOD(stage_one) {
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

INSTANCE_METHOD(stage_all) {
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

INSTANCE_METHOD(unstage_all) {
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

INSTANCE_METHOD(discard_unstaged) {
	print(cptos("discard unstaged print"));
	validate_git_repo_is_initialized();
	// Force-checkout from the index to discard unstaged changes.
	git_checkout_options opt = GIT_CHECKOUT_OPTIONS_INIT;
	opt.checkout_strategy = GIT_CHECKOUT_FORCE;
	git_checkout_index(repo, NULL, &opt);
}

INSTANCE_METHOD(commit) {
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

INSTANCE_METHOD(get_head_message) {
	validate_git_repo_is_initialized();
	// Get the index of the repository.
	git_index *index;
	git_repository_index(&index, repo);
	// Get information for the tree of the commit.
	git_oid index_tree_oid;
	git_index_write_tree(&index_tree_oid, index);
	git_tree *index_tree;
	git_tree_lookup(&index_tree, repo, &index_tree_oid);
	// Get information for the current HEAD of the repository.
	git_oid head_oid;
	git_reference_name_to_id(&head_oid, repo, "HEAD");
	git_commit *head;
	git_commit_lookup(&head, repo, &head_oid);
	// Get the summary and body and store them in variants.
	godot_variant summary_variant;
	{
		const char *summary_charpointer = git_commit_summary(head);
		godot_string summary_str = cptos(summary_charpointer);
		godot_variant_new_string(&summary_variant, &summary_str);
		godot_string_destroy(&summary_str);
	}
	godot_variant body_variant;
	{
		const char *body_charpointer = git_commit_body(head);
		godot_string body_str = cptos(body_charpointer);
		godot_variant_new_string(&body_variant, &body_str);
		godot_string_destroy(&body_str);
	}
	// Create an array of the message to return and convert to a variant.
	godot_array message_array;
	godot_array_new(&message_array);
	godot_array_push_back(&message_array, &summary_variant);
	godot_array_push_back(&message_array, &body_variant);
	godot_variant ret;
	godot_variant_new_array(&ret, &message_array);
	// Clean up memory.
	godot_variant_destroy(&summary_variant);
	godot_variant_destroy(&body_variant);
	godot_array_destroy(&message_array);
	git_tree_free(index_tree);
	git_commit_free(head);
	git_index_free(index);
	return ret;
}

INSTANCE_METHOD(get_branch_list) {
	validate_git_repo_is_initialized();
	// Get the index of the repository.
	git_index *index;
	git_repository_index(&index, repo);
	// Create a dictionary to later return as a variant.
	godot_dictionary branch_dict;
	godot_dictionary_new(&branch_dict);
	{
		git_branch_iterator *iterator;
		git_branch_iterator_new(&iterator, repo, GIT_BRANCH_ALL);
		git_reference *out_branch_ref;
		git_branch_t out_branch_type;
		while (!git_branch_next(&out_branch_ref, &out_branch_type, iterator)) {
			// Branch type as variant.
			if (git_branch_is_head(out_branch_ref) == 1) {
				// TODO: git_branch_is_head vs git_branch_is_checked_out?
				out_branch_type |= 4; // GitBranch.HEAD
			}
			godot_variant branch_type_variant;
			godot_variant_new_int(&branch_type_variant, out_branch_type);
			// Branch name as variant.
			const char *branch_name_cp;
			git_branch_name(&branch_name_cp, out_branch_ref);
			godot_string branch_name_str = cptos(branch_name_cp);
			godot_variant branch_name_variant;
			godot_variant_new_string(&branch_name_variant, &branch_name_str);
			// Set the dictionary entry.
			godot_dictionary_set(&branch_dict, &branch_name_variant, &branch_type_variant);
			// Clean up.
			godot_string_destroy(&branch_name_str);
			godot_variant_destroy(&branch_name_variant);
			godot_variant_destroy(&branch_type_variant);
		}
		git_branch_iterator_free(iterator);
	}
	godot_variant ret;
	godot_variant_new_dictionary(&ret, &branch_dict);
	// Clean up memory.
	godot_dictionary_destroy(&branch_dict);
	git_index_free(index);
	return ret;
}

INSTANCE_METHOD(checkout_branch) {
	validate_git_repo_is_initialized();
	// Get the index of the repository.
	git_index *index;
	git_repository_index(&index, repo);
	// Parse the branch name.
	godot_string prefix = cptos("refs/heads/");
	godot_string branch_name_str = godot_variant_as_string(p_args[0]);
	godot_string branch_ref_str = godot_string_operator_plus(&prefix, &branch_name_str);
	godot_char_string branch_ref_cs = stocs(&branch_ref_str);
	const char *branch_ref_cp = godot_char_string_get_data(&branch_ref_cs);
	// Switch branches.
	git_repository_set_head(repo, branch_ref_cp);
	git_checkout_options opt = GIT_CHECKOUT_OPTIONS_INIT;
	opt.baseline_index = index;
	git_checkout_head(repo, &opt);
	// Write in-memory changes to disk and clean up.
	godot_string_destroy(&prefix);
	godot_string_destroy(&branch_name_str);
	godot_string_destroy(&branch_ref_str);
	godot_char_string_destroy(&branch_ref_cs);
	git_index_write(index);
	git_index_free(index);
}

INSTANCE_METHOD(create_branch) {
	validate_git_repo_is_initialized();
	// Parse the branch name.
	godot_string branch_str = godot_variant_as_string(p_args[0]);
	godot_char_string branch_cs = stocs(&branch_str);
	const char *branch_cp = godot_char_string_get_data(&branch_cs);
	// Get information for the current HEAD of the repository.
	git_oid head_oid;
	git_reference_name_to_id(&head_oid, repo, "HEAD");
	git_commit *head;
	git_commit_lookup(&head, repo, &head_oid);
	// Create the branch.
	git_reference *branch;
	git_branch_create(&branch, repo, branch_cp, head, 0);
	// Clean up memory.
	godot_string_destroy(&branch_str);
	godot_char_string_destroy(&branch_cs);
	git_commit_free(head);
	git_reference_free(branch);
}

INSTANCE_METHOD(rename_branch) {
	validate_git_repo_is_initialized();
	// Parse the old branch name.
	godot_string old_branch_str = godot_variant_as_string(p_args[0]);
	godot_char_string old_branch_cs = stocs(&old_branch_str);
	const char *old_branch_cp = godot_char_string_get_data(&old_branch_cs);
	// Parse the new branch name.
	godot_string new_branch_str = godot_variant_as_string(p_args[1]);
	godot_char_string new_branch_cs = stocs(&new_branch_str);
	const char *new_branch_cp = godot_char_string_get_data(&new_branch_cs);
	// Get the branch.
	git_reference *old_branch;
	git_branch_lookup(&old_branch, repo, old_branch_cp, GIT_BRANCH_LOCAL);
	// Rename the branch.
	git_reference *new_branch;
	git_branch_move(&new_branch, old_branch, new_branch_cp, 0);
	// Clean up memory.
	godot_string_destroy(&old_branch_str);
	godot_string_destroy(&new_branch_str);
	godot_char_string_destroy(&old_branch_cs);
	godot_char_string_destroy(&new_branch_cs);
	git_reference_free(old_branch);
	git_reference_free(new_branch);
}

INSTANCE_METHOD(delete_branch) {
	validate_git_repo_is_initialized();
	// Parse the branch name.
	godot_string branch_str = godot_variant_as_string(p_args[0]);
	godot_char_string branch_cs = stocs(&branch_str);
	const char *branch_cp = godot_char_string_get_data(&branch_cs);
	// Get the branch.
	git_reference *branch;
	print(itos(git_branch_lookup(&branch, repo, branch_cp, GIT_BRANCH_LOCAL)));
	// Delete the branch.
	git_branch_delete(branch);
	// Clean up memory.
	godot_string_destroy(&branch_str);
	godot_char_string_destroy(&branch_cs);
	git_reference_free(branch);
}

INSTANCE_METHOD(get_upstream_branch) {
	validate_git_repo_is_initialized();
	// Parse the branch name.
	godot_string branch_str = godot_variant_as_string(p_args[0]);
	godot_char_string branch_cs = stocs(&branch_str);
	const char *branch_cp = godot_char_string_get_data(&branch_cs);
	// Clean up memory.
}

INSTANCE_METHOD(set_upstream_branch) {
	validate_git_repo_is_initialized();
	// Parse the branch name.
	godot_string branch_str = godot_variant_as_string(p_args[0]);
	godot_char_string branch_cs = stocs(&branch_str);
	const char *branch_cp = godot_char_string_get_data(&branch_cs);
	// Clean up memory.
}
