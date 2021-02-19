#include <gdnative_api_struct.gen.h>
#include <string.h>

// Int to string.
godot_string itos(const int64_t p_value) {
	return godot_string_num_int64(p_value, 10);
}

// Char pointer to string.
godot_string cptos(const char *p_string) {
	godot_string s;
	godot_string_new(&s);
	godot_string_parse_utf8(&s, p_string);
	return s;
}

// String to char string.
godot_char_string stocs(godot_string *p_str) {
	godot_char_string char_string = api->godot_string_utf8(p_str);
	return char_string;
}

// Variant to char string.
godot_char_string vtocs(godot_variant *p_variant) {
	godot_string str = godot_variant_as_string(p_variant);
	godot_char_string char_string = api->godot_string_utf8(&str);
	godot_string_destroy(&str);
	return char_string;
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
