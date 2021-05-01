#include <gdnative_api_struct.gen.h>
#include <string.h>

// Int to string.
godot_string itos(const int64_t p_value) {
	return api->godot_string_num_int64(p_value, 10);
}

// Char pointer to string.
godot_string cptos(const char *p_string) {
	godot_string s;
	api->godot_string_new(&s);
	api->godot_string_parse_utf8(&s, p_string);
	return s;
}

// String to char string.
godot_char_string stocs(godot_string *p_str) {
	godot_char_string char_string = api->godot_string_utf8(p_str);
	return char_string;
}

// Variant to char string.
godot_char_string vtocs(godot_variant *p_variant) {
	godot_string str = api->godot_variant_as_string(p_variant);
	godot_char_string char_string = api->godot_string_utf8(&str);
	api->godot_string_destroy(&str);
	return char_string;
}

void print(godot_string p_string) {
	api->godot_print(&p_string);
}

void print2(godot_string p_string1, godot_string p_string2) {
	godot_string s = api->godot_string_insert(&p_string1, api->godot_string_length(&p_string1), p_string2);
	api->godot_print(&s);
}

void print3(godot_string p_string1, godot_string p_string2, godot_string p_string3) {
	godot_string s = api->godot_string_insert(&p_string1, api->godot_string_length(&p_string1), p_string2);
	s = api->godot_string_insert(&s, api->godot_string_length(&s), p_string3);
	api->godot_print(&s);
}

void print4(godot_string p_string1, godot_string p_string2, godot_string p_string3, godot_string p_string4) {
	godot_string s = api->godot_string_insert(&p_string1, api->godot_string_length(&p_string1), p_string2);
	s = api->godot_string_insert(&s, api->godot_string_length(&s), p_string3);
	s = api->godot_string_insert(&s, api->godot_string_length(&s), p_string4);
	api->godot_print(&s);
}

// Error macros.
#define ERR(m_err) api->godot_print_error(m_err, __FUNCTION__, __FILE__, __LINE__);
#define CHECK_ARG_COUNT(m_num_args)                                    \
	if (p_num_args != m_num_args) {                                    \
		ERR("Invalid number of arguments, expected " #m_num_args "."); \
	}

#define VERBOSE(m_text) //print(cptos(m_text));
