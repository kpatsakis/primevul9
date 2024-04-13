static void cmd_sdbk(Sdb *db, const char *input) {
	char *out = (input[0] == ' ')
		? sdb_querys (db, NULL, 0, input + 1)
		: sdb_querys (db, NULL, 0, "*");
	if (out) {
		r_cons_println (out);
		free (out);
	} else {
		eprintf ("|ERROR| Usage: ask [query]\n");
	}
}