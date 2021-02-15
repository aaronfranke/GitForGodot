#include <gdnative_api_struct.gen.h>
#include <string.h>

godot_string itos(const int64_t p_value) {
	return godot_string_num_int64(p_value, 10);
}

godot_string stos(const char *p_string) {
	godot_string s = godot_string_num_int64(0, 1); // Hack to get a mostly empty string.
	godot_string_parse_utf8(&s, p_string);
	return s;
}

void print(godot_string p_string) {
	godot_print(&p_string);
}

void print2(godot_string p_string1, godot_string p_string2) {
	godot_string s = godot_string_insert(&p_string1, godot_string_length(&p_string1), p_string2);
	godot_print(&s);
}

void print3(godot_string p_string1, godot_string p_string2, godot_string p_string3) {
	godot_string s = godot_string_insert(&p_string1, godot_string_length(&p_string1), p_string2);
	s = godot_string_insert(&s, godot_string_length(&s), p_string3);
	godot_print(&s);
}

void print4(godot_string p_string1, godot_string p_string2, godot_string p_string3, godot_string p_string4) {
	godot_string s = godot_string_insert(&p_string1, godot_string_length(&p_string1), p_string2);
	s = godot_string_insert(&s, godot_string_length(&s), p_string3);
	s = godot_string_insert(&s, godot_string_length(&s), p_string4);
	godot_print(&s);
}
