static void rcc_set_callname(REgg *egg, const char *s) {
	R_FREE (egg->lang.callname);
	egg->lang.nargs = 0;
	egg->lang.callname = trim (strdup (skipspaces (s)));
	egg->lang.pushargs = !((!strcmp (s, "goto")) || (!strcmp (s, "break")));
}