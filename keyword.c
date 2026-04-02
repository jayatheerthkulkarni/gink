#include "keyword.h"

/* Array of Gink language keywords */
static const char* gink_keywords[] = {
	/* Control Flow & Logic */
	"if", "else", "while", "for", "in", "break", "continue", "return", "true", "false",
	/* Variables & Data Structures */
	"auto", "const", "type", "struct",
	/* Functions & Modifiers */
	"fun", "inline",
	/* Memory & Resource Management */
	"unsafe", "ensure", "defer", "new",
	/* Modules & Visibility */
	"package", "import", "use", "as", "pub", "pri",
	/* Built-in Primitive Types */
	"i8", "u8", "i16", "u16", "i32", "u32", "i64", "u64", "i128", "u128", "int", "uint",
	"f16", "f32", "f64", "f128", "float", "bool", "char", "string"
};

size_t keyword_count = sizeof(gink_keywords) / sizeof(gink_keywords[0]);

bool is_keyword(const char *val) {
	for (size_t i = 0; i < keyword_count; i++) {
		if (strcmp(val, gink_keywords[i]) == 0) {
			return true;
		}
	}
	return false;
}

int keyword_total_count() {
	return keyword_count;
}
