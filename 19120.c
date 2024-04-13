static bool blacklisted_word(char* name) {
	const char * list[] = {
		"__stack_chk_guard",
		"__stderrp",
		"__stdinp",
		"__stdoutp",
		"_DefaultRuneLocale"
	};
	int i;
	for (i = 0; i < sizeof (list) / sizeof (list[0]); i++) {
		if (strstr (name, list[i])) { return true; }
	}
	return false;
}