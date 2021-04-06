#include <gdnative_api_struct.gen.h>
#include <libgit2/include/git2.h>

const int GIT_STATUS_INDEX_CHANGES = GIT_STATUS_INDEX_NEW | GIT_STATUS_INDEX_MODIFIED | GIT_STATUS_INDEX_DELETED | GIT_STATUS_INDEX_RENAMED | GIT_STATUS_INDEX_TYPECHANGE;
const int GIT_STATUS_WT_CHANGES = GIT_STATUS_WT_NEW | GIT_STATUS_WT_MODIFIED | GIT_STATUS_WT_DELETED | GIT_STATUS_WT_RENAMED | GIT_STATUS_WT_TYPECHANGE | GIT_STATUS_WT_UNREADABLE;

const godot_gdnative_core_api_struct *api = NULL;
const godot_gdnative_ext_nativescript_api_struct *nativescript_api = NULL;

GDCALLINGCONV void *simple_constructor(godot_object *p_instance, void *p_method_data);
GDCALLINGCONV void simple_destructor(godot_object *p_instance, void *p_method_data, void *p_user_data);

#define INSTANCE_METHOD(m_name) godot_variant simple_##m_name(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args)

#define INSTANCE_METHOD_CALL(m_name, m_num_args, m_args) simple_##m_name(p_instance, p_method_data, p_user_data, m_num_args, m_args)

INSTANCE_METHOD(get_data);
INSTANCE_METHOD(get_status);
INSTANCE_METHOD(stage_one);
INSTANCE_METHOD(stage_all);
INSTANCE_METHOD(unstage_all);
INSTANCE_METHOD(discard_unstaged);
INSTANCE_METHOD(commit);
INSTANCE_METHOD(get_head_message);
INSTANCE_METHOD(get_branch_list);
INSTANCE_METHOD(get_remote_list);
INSTANCE_METHOD(checkout_branch);
INSTANCE_METHOD(create_branch);
INSTANCE_METHOD(rename_branch);
INSTANCE_METHOD(delete_branch);
INSTANCE_METHOD(get_upstream_branch);
INSTANCE_METHOD(set_upstream_branch);
INSTANCE_METHOD(fetch_all);
INSTANCE_METHOD(fetch_one);
INSTANCE_METHOD(pull);
INSTANCE_METHOD(push);

void GDN_EXPORT godot_gdnative_init(godot_gdnative_init_options *p_options) {
	api = p_options->api_struct;

	for (int i = 0; i < api->num_extensions; i++) {
		switch (api->extensions[i]->type) {
			case GDNATIVE_EXT_NATIVESCRIPT: {
				nativescript_api = (godot_gdnative_ext_nativescript_api_struct *)api->extensions[i];
			};
					break;
			default:
				break;
		};
	};
}

void GDN_EXPORT godot_gdnative_terminate(godot_gdnative_terminate_options *p_options) {
	api = NULL;
	nativescript_api = NULL;
}
