int server_supports_v2(const char *c, int die_on_error)
{
	int i;

	for (i = 0; i < server_capabilities_v2.argc; i++) {
		const char *out;
		if (skip_prefix(server_capabilities_v2.argv[i], c, &out) &&
		    (!*out || *out == '='))
			return 1;
	}

	if (die_on_error)
		die(_("server doesn't support '%s'"), c);

	return 0;
}