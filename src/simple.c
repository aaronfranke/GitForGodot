#include "simple.h"
#include "print.h"

typedef struct user_data_struct {
	char data[256];
	int major;
	int minor;
	int patch;
} user_data_struct;

git_repository *repo = NULL;
git_revwalk *walker = NULL;

#pragma region Helper methods.

void validate_git_repo_is_initialized() {
	if (repo != NULL) {
		return; // Already initialized.
	}
	VERBOSE("Initializing libgit2");
	git_libgit2_init();
	godot_string updir = cptos("../");
	godot_string dotgit = cptos(".git");
	godot_string globalized_path_string;
	{
		godot_object *ps_singleton = api->godot_global_get_singleton("ProjectSettings");
		godot_method_bind *ps_globalize_path = api->godot_method_bind_get_method("ProjectSettings", "globalize_path");
		godot_string res_string = cptos("res://");
		godot_variant res_variant;
		api->godot_variant_new_string(&res_variant, &res_string);
		const godot_variant *args[] = { &res_variant };
		godot_variant globalized_path_variant = api->godot_method_bind_call(ps_globalize_path, ps_singleton, args, 1, NULL);
		globalized_path_string = api->godot_variant_as_string(&globalized_path_variant);
		api->godot_string_destroy(&res_string);
		api->godot_variant_destroy(&res_variant);
		api->godot_variant_destroy(&globalized_path_variant);
	}
	godot_class_constructor directory_constructor = api->godot_get_class_constructor("_Directory");
	godot_object *directory = directory_constructor();
	godot_method_bind *dir_exists_method = api->godot_method_bind_get_method("_Directory", "dir_exists");
	int attempts_left = 10;
	do {
		godot_string path_string = api->godot_string_operator_plus(&globalized_path_string, &dotgit);
		const void *dir_exists_args[] = { (void *)&path_string };
		godot_bool exists = false;
		api->godot_method_bind_ptrcall(dir_exists_method, directory, dir_exists_args, &exists);
		if (exists) {
			attempts_left = -5; // Magic number that means it was found.
		} else {
			attempts_left--;
			globalized_path_string = api->godot_string_operator_plus(&globalized_path_string, &updir);
		}
		api->godot_string_destroy(&path_string);
	} while (attempts_left > 0);
	if (attempts_left != -5) {
		ERR("Could not find the .git directory for the Git repository.")
		return;
	}
	godot_string path = api->godot_string_operator_plus(&globalized_path_string, &dotgit);
	godot_char_string path_cs = stocs(&path);
	api->godot_string_destroy(&updir);
	api->godot_string_destroy(&dotgit);
	api->godot_string_destroy(&globalized_path_string);
	api->godot_string_destroy(&path);
	const char *path_cp = api->godot_char_string_get_data(&path_cs);
	git_repository_open(&repo, path_cp);
}

godot_array git_commit_to_godot_array(git_commit *commit) {
	godot_array array;
	api->godot_array_new(&array);
	{
		const git_time_t time = git_commit_time(commit);
		godot_variant variant;
		api->godot_variant_new_int(&variant, time);
		api->godot_array_push_back(&array, &variant);
		api->godot_variant_destroy(&variant);
	}
	{
		godot_string string = cptos(git_commit_summary(commit));
		godot_variant variant;
		api->godot_variant_new_string(&variant, &string);
		api->godot_array_push_back(&array, &variant);
		api->godot_string_destroy(&string);
		api->godot_variant_destroy(&variant);
	}
	{
		godot_string string = cptos(git_commit_body(commit));
		godot_variant variant;
		api->godot_variant_new_string(&variant, &string);
		api->godot_array_push_back(&array, &variant);
		api->godot_string_destroy(&string);
		api->godot_variant_destroy(&variant);
	}
	{
		git_signature *signature = (git_signature *)git_commit_author(commit);
		for (int i = 0; i < 2; i++) {
			{
				godot_string string = cptos(signature->email);
				godot_variant variant;
				api->godot_variant_new_string(&variant, &string);
				api->godot_array_push_back(&array, &variant);
				api->godot_string_destroy(&string);
				api->godot_variant_destroy(&variant);
			}
			{
				godot_string string = cptos(signature->name);
				godot_variant variant;
				api->godot_variant_new_string(&variant, &string);
				api->godot_array_push_back(&array, &variant);
				api->godot_string_destroy(&string);
				api->godot_variant_destroy(&variant);
			}
			{
				godot_variant variant;
				api->godot_variant_new_int(&variant, signature->when.time);
				api->godot_array_push_back(&array, &variant);
				api->godot_variant_destroy(&variant);
			}
			if (i == 0) {
				signature = (git_signature *)git_commit_committer(commit);
			}
		}
	}
	{
		unsigned int parent_count = git_commit_parentcount(commit);
		godot_variant variant;
		api->godot_variant_new_int(&variant, parent_count);
		api->godot_array_push_back(&array, &variant);
		api->godot_variant_destroy(&variant);
		for (unsigned int i = 0; i < parent_count; i++) {
			const git_oid *parent_oid = git_commit_parent_id(commit, i);
			char commit_hash[41] = "";
			git_oid_fmt(commit_hash, parent_oid);
			godot_string commit_hash_string = cptos(commit_hash);
			godot_variant commit_hash_variant;
			api->godot_variant_new_string(&commit_hash_variant, &commit_hash_string);
			api->godot_array_push_back(&array, &commit_hash_variant);
		}
	}
	return array;
}
#pragma endregion Helper methods.

#pragma region GDNative methods.
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
	REGISTER_INSTANCE_METHOD(get_remote_list);
	REGISTER_INSTANCE_METHOD(get_all_commits_dictionary);
	REGISTER_INSTANCE_METHOD(checkout_branch);
	REGISTER_INSTANCE_METHOD(create_branch);
	REGISTER_INSTANCE_METHOD(rename_branch);
	REGISTER_INSTANCE_METHOD(delete_branch);
	REGISTER_INSTANCE_METHOD(get_upstream_branch);
	REGISTER_INSTANCE_METHOD(set_upstream_branch);
	REGISTER_INSTANCE_METHOD(add_remote);
	REGISTER_INSTANCE_METHOD(delete_remote);
	REGISTER_INSTANCE_METHOD(rename_remote);
	REGISTER_INSTANCE_METHOD(fetch_all);
	REGISTER_INSTANCE_METHOD(fetch_one);
	REGISTER_INSTANCE_METHOD(pull);
	REGISTER_INSTANCE_METHOD(push);

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
#pragma endregion GDNative methods.

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
	VERBOSE("get_status");
	CHECK_ARG_COUNT(0);
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
	api->godot_dictionary_new(&all_changes);
	for (int i = 0; i < status; i++) {
		const git_status_entry *status_entry = git_status_byindex(status_list, i);
		if (status_entry->status & (GIT_STATUS_INDEX_CHANGES | GIT_STATUS_WT_CHANGES)) {
			godot_variant status;
			api->godot_variant_new_int(&status, status_entry->status);
			godot_string path_string;
			if (status_entry->status & GIT_STATUS_INDEX_CHANGES) {
				path_string = cptos(status_entry->head_to_index->new_file.path);
			} else { // status_entry->status & GIT_STATUS_WT_CHANGES
				path_string = cptos(status_entry->index_to_workdir->new_file.path);
			}
			godot_variant path_variant;
			api->godot_variant_new_string(&path_variant, &path_string);
			api->godot_dictionary_set(&all_changes, &path_variant, &status);
			// Clean up.
			api->godot_string_destroy(&path_string);
			api->godot_variant_destroy(&path_variant);
			api->godot_variant_destroy(&status);
		}
	}
	godot_variant all_changes_variant;
	api->godot_variant_new_dictionary(&all_changes_variant, &all_changes);
	api->godot_dictionary_destroy(&all_changes);
	return all_changes_variant;
}

INSTANCE_METHOD(stage_one) {
	VERBOSE("stage_one");
	CHECK_ARG_COUNT(1);
	validate_git_repo_is_initialized();
	// Get the index of the repository.
	git_index *index;
	git_repository_index(&index, repo);
	// Get file path argument.
	godot_variant *arg = p_args[0];
	godot_string path_string = api->godot_variant_as_string(arg);
	godot_char_string path_char_string = stocs(&path_string);
	const char *path_charptr = api->godot_char_string_get_data(&path_char_string);
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
	api->godot_string_destroy(&path_string);
	api->godot_variant_destroy(arg);
}

INSTANCE_METHOD(stage_all) {
	VERBOSE("stage_all");
	CHECK_ARG_COUNT(0);
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
	VERBOSE("unstage_all");
	CHECK_ARG_COUNT(0);
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
	VERBOSE("discard_unstaged");
	CHECK_ARG_COUNT(0);
	validate_git_repo_is_initialized();
	// Force-checkout from the index to discard unstaged changes.
	git_checkout_options opt = GIT_CHECKOUT_OPTIONS_INIT;
	opt.checkout_strategy = GIT_CHECKOUT_FORCE;
	git_checkout_index(repo, NULL, &opt);
}

INSTANCE_METHOD(commit) {
	VERBOSE("commit");
	CHECK_ARG_COUNT(3);
	validate_git_repo_is_initialized();
	// Get the index of the repository.
	git_index *index;
	git_repository_index(&index, repo);
	godot_bool amend = api->godot_variant_as_bool(p_args[0]);
	// Parse the commit message.
	godot_string newline = cptos("\n\n");
	godot_string name = api->godot_variant_as_string(p_args[1]);
	godot_string desc = api->godot_variant_as_string(p_args[2]);
	godot_string message_str = api->godot_string_operator_plus(&name, &newline);
	message_str = api->godot_string_operator_plus(&message_str, &desc);
	godot_char_string message_cs = stocs(&message_str);
	const char *message = api->godot_char_string_get_data(&message_cs);
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
	godot_object *os_singleton = api->godot_global_get_singleton("OS");
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
	api->godot_char_string_destroy(&message_cs);
	api->godot_string_destroy(&message_str);
	api->godot_string_destroy(&newline);
	git_signature_free(committer);
	git_tree_free(index_tree);
	git_commit_free(parent);
	git_index_free(index);
}

INSTANCE_METHOD(get_head_message) {
	VERBOSE("get_head_message");
	CHECK_ARG_COUNT(0);
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
		api->godot_variant_new_string(&summary_variant, &summary_str);
		api->godot_string_destroy(&summary_str);
	}
	godot_variant body_variant;
	{
		const char *body_charpointer = git_commit_body(head);
		godot_string body_str = cptos(body_charpointer);
		api->godot_variant_new_string(&body_variant, &body_str);
		api->godot_string_destroy(&body_str);
	}
	// Create an array of the message to return and convert to a variant.
	godot_array message_array;
	api->godot_array_new(&message_array);
	api->godot_array_push_back(&message_array, &summary_variant);
	api->godot_array_push_back(&message_array, &body_variant);
	godot_variant ret;
	api->godot_variant_new_array(&ret, &message_array);
	// Clean up memory.
	api->godot_variant_destroy(&summary_variant);
	api->godot_variant_destroy(&body_variant);
	api->godot_array_destroy(&message_array);
	git_tree_free(index_tree);
	git_commit_free(head);
	git_index_free(index);
	return ret;
}

INSTANCE_METHOD(get_branch_list) {
	VERBOSE("get_branch_list");
	CHECK_ARG_COUNT(0);
	validate_git_repo_is_initialized();
	// Get the index of the repository.
	git_index *index;
	git_repository_index(&index, repo);
	// Create a dictionary to later return as a variant.
	godot_dictionary branch_dict;
	api->godot_dictionary_new(&branch_dict);
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
			api->godot_variant_new_int(&branch_type_variant, out_branch_type);
			// Branch name as variant.
			const char *branch_name_cp;
			git_branch_name(&branch_name_cp, out_branch_ref);
			godot_string branch_name_str = cptos(branch_name_cp);
			godot_variant branch_name_variant;
			api->godot_variant_new_string(&branch_name_variant, &branch_name_str);
			// Set the dictionary entry.
			api->godot_dictionary_set(&branch_dict, &branch_name_variant, &branch_type_variant);
			// Clean up.
			api->godot_string_destroy(&branch_name_str);
			api->godot_variant_destroy(&branch_name_variant);
			api->godot_variant_destroy(&branch_type_variant);
		}
		git_branch_iterator_free(iterator);
	}
	godot_variant ret;
	api->godot_variant_new_dictionary(&ret, &branch_dict);
	// Clean up memory.
	api->godot_dictionary_destroy(&branch_dict);
	git_index_free(index);
	return ret;
}

INSTANCE_METHOD(get_remote_list) {
	VERBOSE("get_remote_list");
	CHECK_ARG_COUNT(0);
	validate_git_repo_is_initialized();
	// Get a list of remotes.
	git_strarray remotes_git_strarray;
	git_remote_list(&remotes_git_strarray, repo);
	// Convert to an array of Godot strings.
	godot_array remotes_array;
	api->godot_array_new(&remotes_array);
	for (int i = 0; i < remotes_git_strarray.count; i++) {
		godot_string remote_string = cptos(remotes_git_strarray.strings[i]);
		godot_variant remote_string_variant;
		api->godot_variant_new_string(&remote_string_variant, &remote_string);
		api->godot_array_push_back(&remotes_array, &remote_string_variant);
		// Clean up memory.
		api->godot_string_destroy(&remote_string);
		api->godot_variant_destroy(&remote_string_variant);
	}
	godot_variant remotes_variant;
	api->godot_variant_new_array(&remotes_variant, &remotes_array);
	// Clean up memory.
	api->godot_array_destroy(&remotes_array);
	git_strarray_free(&remotes_git_strarray);
	return remotes_variant;
}

INSTANCE_METHOD(get_all_commits_dictionary) {
	VERBOSE("get_all_commits_dictionary");
	CHECK_ARG_COUNT(1);
	validate_git_repo_is_initialized();
	// Get the index of the repository.
	git_index *index;
	git_repository_index(&index, repo);
	// Parse the limit from the args.
	int64_t commit_limit = api->godot_variant_as_int(p_args[0]);
	// Set up the dictionary for the commit information. The hashes are keys.
	godot_dictionary commit_dictionary;
	api->godot_dictionary_new(&commit_dictionary);
	if (walker == NULL) {
		git_revwalk_new(&walker, repo);
	}
	// Scope for branch iteration. Everything is saved to commit_dictionary.
	{
		godot_dictionary branch_heads_dictionary;
		api->godot_dictionary_new(&branch_heads_dictionary);
		// Iterate over all branches.
		git_branch_iterator *iterator;
		git_branch_iterator_new(&iterator, repo, GIT_BRANCH_ALL);
		git_reference *out_branch_ref;
		git_branch_t out_branch_type;
		while (!git_branch_next(&out_branch_ref, &out_branch_type, iterator)) {
			// Branch name as variant.
			const char *branch_name_cp;
			git_branch_name(&branch_name_cp, out_branch_ref);
			godot_string branch_name_str = cptos(branch_name_cp);
			godot_variant branch_name_variant;
			api->godot_variant_new_string(&branch_name_variant, &branch_name_str);
			// Get the commit object from the branch.
			git_object *first_commit_object = NULL;
			git_reference_peel(&first_commit_object, out_branch_ref, GIT_OBJECT_COMMIT);
			const git_oid *first_commit_oid = git_object_id(first_commit_object);
			// Push the first commit to the revwalker and start walking.
			git_revwalk_push(walker, first_commit_oid);
			git_oid next_commit_oid;
			for (int64_t i = 0; i < commit_limit && !git_revwalk_next(&next_commit_oid, walker); i++) {
				// Look up the commit from its OID.
				git_commit *commit;
				git_commit_lookup(&commit, repo, &next_commit_oid);
				// Get the SHA1 hash of the commit as a string.
				char commit_hash[41] = "";
				git_oid_fmt(commit_hash, &next_commit_oid);
				godot_string commit_hash_string = cptos(commit_hash);
				godot_variant commit_hash_variant;
				api->godot_variant_new_string(&commit_hash_variant, &commit_hash_string);
				// If this commit is a branch head, save the hash there.
				if (git_oid_cmp(first_commit_oid, &next_commit_oid) == 0) {
					api->godot_dictionary_set(&branch_heads_dictionary, &branch_name_variant, &commit_hash_variant);
				}
				// Check if this hash is already in the dictionary.
				if (api->godot_dictionary_has(&commit_dictionary, &commit_hash_variant)) {
					// Hey, I've seen this one!
					i = commit_limit;
					// Clean up memory.
					api->godot_string_destroy(&commit_hash_string);
					api->godot_variant_destroy(&commit_hash_variant);
					continue;
				}
				// Convert the commit info to an array and add it to the dictionary.
				godot_array commit_array = git_commit_to_godot_array(commit);
				godot_variant commit_variant;
				api->godot_variant_new_array(&commit_variant, &commit_array);
				api->godot_dictionary_set(&commit_dictionary, &commit_hash_variant, &commit_variant);
				// Clean up memory (for this commit).
				api->godot_string_destroy(&commit_hash_string);
				api->godot_variant_destroy(&commit_hash_variant);
				api->godot_array_destroy(&commit_array);
				api->godot_variant_destroy(&commit_variant);
			}
			git_revwalk_reset(walker);
			// Clean up memory (for this branch revwalk).
			api->godot_string_destroy(&branch_name_str);
			api->godot_variant_destroy(&branch_name_variant);
			git_object_free(first_commit_object);
		}
		// Save the branch heads dictionary inside of commit_dictionary.
		godot_variant branch_heads_dictionary_variant;
		api->godot_variant_new_dictionary(&branch_heads_dictionary_variant, &branch_heads_dictionary);
		godot_string branch_heads_string = cptos("branch_heads_dictionary");
		godot_variant branch_heads_string_variant;
		api->godot_variant_new_string(&branch_heads_string_variant, &branch_heads_string);
		api->godot_dictionary_set(&commit_dictionary, &branch_heads_string_variant, &branch_heads_dictionary_variant);
		// Clean up memory (for this branch iterator).
		api->godot_dictionary_destroy(&branch_heads_dictionary);
		api->godot_variant_destroy(&branch_heads_dictionary_variant);
		api->godot_string_destroy(&branch_heads_string);
		api->godot_variant_destroy(&branch_heads_string_variant);
		git_branch_iterator_free(iterator);
		git_reference_free(out_branch_ref);
	}
	// Convert the dictionary to a variant to return.
	godot_variant ret;
	api->godot_variant_new_dictionary(&ret, &commit_dictionary);
	// Clean up memory.
	api->godot_dictionary_destroy(&commit_dictionary);
	git_index_free(index);
	return ret;
}

INSTANCE_METHOD(checkout_branch) {
	VERBOSE("checkout_branch");
	CHECK_ARG_COUNT(1);
	validate_git_repo_is_initialized();
	// Get the index of the repository.
	git_index *index;
	git_repository_index(&index, repo);
	// Parse the branch name.
	godot_string prefix = cptos("refs/heads/");
	godot_string branch_name_str = api->godot_variant_as_string(p_args[0]);
	godot_string branch_ref_str = api->godot_string_operator_plus(&prefix, &branch_name_str);
	godot_char_string branch_ref_cs = stocs(&branch_ref_str);
	const char *branch_ref_cp = api->godot_char_string_get_data(&branch_ref_cs);
	// Switch branches.
	git_repository_set_head(repo, branch_ref_cp);
	git_checkout_options opt = GIT_CHECKOUT_OPTIONS_INIT;
	opt.baseline_index = index;
	git_checkout_head(repo, &opt);
	// Write in-memory changes to disk and clean up.
	api->godot_string_destroy(&prefix);
	api->godot_string_destroy(&branch_name_str);
	api->godot_string_destroy(&branch_ref_str);
	api->godot_char_string_destroy(&branch_ref_cs);
	git_index_write(index);
	git_index_free(index);
}

INSTANCE_METHOD(create_branch) {
	VERBOSE("create_branch");
	CHECK_ARG_COUNT(1);
	validate_git_repo_is_initialized();
	// Parse the branch name.
	godot_string branch_str = api->godot_variant_as_string(p_args[0]);
	godot_char_string branch_cs = stocs(&branch_str);
	const char *branch_cp = api->godot_char_string_get_data(&branch_cs);
	// Get information for the current HEAD of the repository.
	git_oid head_oid;
	git_reference_name_to_id(&head_oid, repo, "HEAD");
	git_commit *head;
	git_commit_lookup(&head, repo, &head_oid);
	// Create the branch.
	git_reference *branch;
	git_branch_create(&branch, repo, branch_cp, head, 0);
	// Clean up memory.
	api->godot_string_destroy(&branch_str);
	api->godot_char_string_destroy(&branch_cs);
	git_commit_free(head);
	git_reference_free(branch);
}

INSTANCE_METHOD(rename_branch) {
	VERBOSE("rename_branch");
	CHECK_ARG_COUNT(2);
	validate_git_repo_is_initialized();
	// Parse the old branch name.
	godot_string old_branch_str = api->godot_variant_as_string(p_args[0]);
	godot_char_string old_branch_cs = stocs(&old_branch_str);
	const char *old_branch_cp = api->godot_char_string_get_data(&old_branch_cs);
	// Parse the new branch name.
	godot_string new_branch_str = api->godot_variant_as_string(p_args[1]);
	godot_char_string new_branch_cs = stocs(&new_branch_str);
	const char *new_branch_cp = api->godot_char_string_get_data(&new_branch_cs);
	// Get the branch.
	git_reference *old_branch;
	git_branch_lookup(&old_branch, repo, old_branch_cp, GIT_BRANCH_LOCAL);
	// Rename the branch.
	git_reference *new_branch;
	git_branch_move(&new_branch, old_branch, new_branch_cp, 0);
	// Clean up memory.
	api->godot_string_destroy(&old_branch_str);
	api->godot_string_destroy(&new_branch_str);
	api->godot_char_string_destroy(&old_branch_cs);
	api->godot_char_string_destroy(&new_branch_cs);
	git_reference_free(old_branch);
	git_reference_free(new_branch);
}

INSTANCE_METHOD(delete_branch) {
	VERBOSE("delete_branch");
	CHECK_ARG_COUNT(1);
	validate_git_repo_is_initialized();
	// Parse the branch name.
	godot_string branch_str = api->godot_variant_as_string(p_args[0]);
	godot_char_string branch_cs = stocs(&branch_str);
	const char *branch_cp = api->godot_char_string_get_data(&branch_cs);
	// Get the branch.
	git_reference *branch;
	print(itos(git_branch_lookup(&branch, repo, branch_cp, GIT_BRANCH_LOCAL)));
	// Delete the branch.
	git_branch_delete(branch);
	// Clean up memory.
	api->godot_string_destroy(&branch_str);
	api->godot_char_string_destroy(&branch_cs);
	git_reference_free(branch);
}

INSTANCE_METHOD(get_upstream_branch) {
	VERBOSE("get_upstream_branch");
	CHECK_ARG_COUNT(1);
	validate_git_repo_is_initialized();
	// Parse the branch name.
	godot_string branch_str = api->godot_variant_as_string(p_args[0]);
	godot_char_string branch_cs = stocs(&branch_str);
	const char *branch_cp = api->godot_char_string_get_data(&branch_cs);
	// Get the local branch.
	git_reference *local_branch = NULL;
	git_branch_lookup(&local_branch, repo, branch_cp, GIT_BRANCH_LOCAL);
	// Get the upstream of the branch.
	git_reference *upstream_branch = NULL;
	git_branch_upstream(&upstream_branch, local_branch);
	// Get the name of the upstream.
	const char *upstream_name_cp;
	if (upstream_branch == NULL) {
		upstream_name_cp = "";
	} else {
		git_branch_name(&upstream_name_cp, upstream_branch);
	}
	godot_string upstream_name_string = cptos(upstream_name_cp);
	godot_variant upstream_name_variant;
	api->godot_variant_new_string(&upstream_name_variant, &upstream_name_string);
	// Clean up memory.
	api->godot_string_destroy(&branch_str);
	api->godot_string_destroy(&upstream_name_string);
	api->godot_char_string_destroy(&branch_cs);
	git_reference_free(local_branch);
	git_reference_free(upstream_branch);
	return upstream_name_variant;
}

INSTANCE_METHOD(set_upstream_branch) {
	VERBOSE("set_upstream_branch");
	CHECK_ARG_COUNT(2);
	validate_git_repo_is_initialized();
	// Parse the local branch name.
	godot_string local_branch_str = api->godot_variant_as_string(p_args[0]);
	godot_char_string local_branch_cs = stocs(&local_branch_str);
	const char *local_branch_cp = api->godot_char_string_get_data(&local_branch_cs);
	// Parse the upstream branch name.
	godot_string upstream_branch_str = api->godot_variant_as_string(p_args[1]);
	godot_char_string upstream_branch_cs = stocs(&upstream_branch_str);
	const char *upstream_branch_cp = api->godot_char_string_get_data(&upstream_branch_cs);
	// Get the local branch.
	git_reference *local_branch;
	git_branch_lookup(&local_branch, repo, local_branch_cp, GIT_BRANCH_LOCAL);
	// Set the upstream branch. This will fail if there's no upstream branch with the same name.
	if (git_branch_set_upstream(local_branch, upstream_branch_cp)) {
		ERR("Can't set upstream branch, doesn't exist on remote.")
	}
	// Clean up memory.
	api->godot_string_destroy(&local_branch_str);
	api->godot_char_string_destroy(&local_branch_cs);
	api->godot_string_destroy(&upstream_branch_str);
	api->godot_char_string_destroy(&upstream_branch_cs);
	git_reference_free(local_branch);
}

INSTANCE_METHOD(add_remote) {
	VERBOSE("add_remote");
	CHECK_ARG_COUNT(2);
	validate_git_repo_is_initialized();
	// Parse the remote name an.
	godot_string remote_name_str = api->godot_variant_as_string(p_args[0]);
	godot_char_string remote_name_cs = stocs(&remote_name_str);
	const char *remote_name_cp = api->godot_char_string_get_data(&remote_name_cs);
	godot_string remote_url_str = api->godot_variant_as_string(p_args[1]);
	godot_char_string remote_url_cs = stocs(&remote_url_str);
	const char *remote_url_cp = api->godot_char_string_get_data(&remote_url_cs);
	// Create the remote.
	git_remote *remote;
	git_remote_create(&remote, repo, remote_name_cp, remote_url_cp);
	// Clean up memory.
	api->godot_string_destroy(&remote_name_str);
	api->godot_char_string_destroy(&remote_name_cs);
	api->godot_string_destroy(&remote_url_str);
	api->godot_char_string_destroy(&remote_url_cs);
	git_remote_free(remote);
}

INSTANCE_METHOD(delete_remote) {
	VERBOSE("delete_remote");
	CHECK_ARG_COUNT(1);
	validate_git_repo_is_initialized();
	// Parse the remote name.
	godot_string remote_name_str = api->godot_variant_as_string(p_args[0]);
	godot_char_string remote_name_cs = stocs(&remote_name_str);
	const char *remote_name_cp = api->godot_char_string_get_data(&remote_name_cs);
	// Delete the remote.
	git_remote_delete(repo, remote_name_cp);
	// Clean up memory.
	api->godot_string_destroy(&remote_name_str);
	api->godot_char_string_destroy(&remote_name_cs);
}

INSTANCE_METHOD(rename_remote) {
	VERBOSE("rename_remote");
	CHECK_ARG_COUNT(2);
	validate_git_repo_is_initialized();
	// Parse the remote names.
	godot_string old_remote_name_str = api->godot_variant_as_string(p_args[0]);
	godot_char_string old_remote_name_cs = stocs(&old_remote_name_str);
	const char *old_remote_name_cp = api->godot_char_string_get_data(&old_remote_name_cs);
	godot_string new_remote_name_str = api->godot_variant_as_string(p_args[1]);
	godot_char_string new_remote_name_cs = stocs(&new_remote_name_str);
	const char *new_remote_name_cp = api->godot_char_string_get_data(&new_remote_name_cs);
	// Rename the remote.
	git_strarray problems;
	git_remote_rename(&problems, repo, old_remote_name_cp, new_remote_name_cp);
	// Clean up memory.
	api->godot_string_destroy(&old_remote_name_str);
	api->godot_char_string_destroy(&old_remote_name_cs);
	api->godot_string_destroy(&new_remote_name_str);
	api->godot_char_string_destroy(&new_remote_name_cs);
	git_strarray_free(&problems);
}

INSTANCE_METHOD(fetch_all) {
	VERBOSE("fetch_all");
	CHECK_ARG_COUNT(0);
	validate_git_repo_is_initialized();
	// Get the remotes.
	godot_variant remote_list = INSTANCE_METHOD_CALL(get_remote_list, 0, NULL);
	godot_array remote_array = api->godot_variant_as_array(&remote_list);
	godot_int size = api->godot_array_size(&remote_array);
	// Fetch all of the remotes.
	for (godot_int i = 0; i < size; i++) {
		godot_variant remote_variant = api->godot_array_get(&remote_array, i);
		godot_variant *args[1] = { &remote_variant };
		INSTANCE_METHOD_CALL(fetch_one, 1, args);
		api->godot_variant_destroy(&remote_variant);
	}
	// Clean up memory.
	api->godot_variant_destroy(&remote_list);
	api->godot_array_destroy(&remote_array);
}

INSTANCE_METHOD(fetch_one) {
	VERBOSE("fetch_one");
	CHECK_ARG_COUNT(1);
	validate_git_repo_is_initialized();
	// Parse the remote name.
	godot_string remote_name_str = api->godot_variant_as_string(p_args[0]);
	godot_char_string remote_name_cs = stocs(&remote_name_str);
	const char *remote_name_cp = api->godot_char_string_get_data(&remote_name_cs);
	// Get the remote.
	git_remote *remote;
	git_remote_lookup(&remote, repo, remote_name_cp);
	// Fetch from the remote.
	git_remote_fetch(remote, NULL, NULL, NULL);
	// Clean up memory.
	api->godot_string_destroy(&remote_name_str);
	api->godot_char_string_destroy(&remote_name_cs);
	git_remote_free(remote);
}

INSTANCE_METHOD(pull) {
	VERBOSE("pull");
	CHECK_ARG_COUNT(0);
	validate_git_repo_is_initialized();
	//
	// Clean up memory.
}

INSTANCE_METHOD(push) {
	VERBOSE("push");
	CHECK_ARG_COUNT(0);
	validate_git_repo_is_initialized();
	// Get the currently checked out branch.
	git_branch_iterator *iterator;
	git_branch_iterator_new(&iterator, repo, GIT_BRANCH_LOCAL);
	git_reference *local_branch_ref;
	git_branch_t out_branch_type; // Unused.
	while (!git_branch_next(&local_branch_ref, &out_branch_type, iterator)) {
		if (git_branch_is_head(local_branch_ref) == 1) {
			// TODO: git_branch_is_head vs git_branch_is_checked_out?
			break;
		}
	}
	//git_reference *upstream_branch_reference;
	//git_branch_upstream(&upstream_branch_reference, local_branch_ref);
	//git_refspec *refspec;
	//git_refspec_parse(&refspec, "", 0);
	char *refspec_cp = "+refs/heads/master";
	const git_strarray refspec = { &refspec_cp, 1 };
	git_remote *remote;
	print(itos(git_remote_lookup(&remote, repo, "origin")));
	git_push_options opts; // = GIT_PUSH_OPTIONS_INIT;
	print(itos(git_push_options_init(&opts, GIT_PUSH_OPTIONS_VERSION)));
	print(itos(git_remote_push(remote, &refspec, &opts)));
	const git_error *error = git_error_last();
	print(cptos(error->message));
	// Clean up memory.
	git_branch_iterator_free(iterator);
	git_reference_free(local_branch_ref);
	//git_refspec_free(refspec);
	git_remote_free(remote);
}
