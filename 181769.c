static void find_refs(RCore *core, const char *glob) {
	char cmd[128];
	ut64 curseek = core->offset;
	while (*glob == ' ') glob++;
	if (!*glob) {
		glob = "str.";
	}
	if (*glob == '?') {
		eprintf ("Usage: axF [flag-str-filter]\n");
		return;
	}
	eprintf ("Finding references of flags matching '%s'...\n", glob);
	snprintf (cmd, sizeof (cmd) - 1, ".(findstref) @@= `f~%s[0]`", glob);
	r_core_cmd0 (core, "(findstref;f here=$$;s entry0;/r here;f-here)");
	r_core_cmd0 (core, cmd);
	r_core_cmd0 (core, "(-findstref)");
	r_core_seek (core, curseek, 1);
}