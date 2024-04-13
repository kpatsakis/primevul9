int server_supports_feature(const char *c, const char *feature,
			    int die_on_error)
{
	int i;

	for (i = 0; i < server_capabilities_v2.argc; i++) {
		const char *out;
		if (skip_prefix(server_capabilities_v2.argv[i], c, &out) &&
		    (!*out || *(out++) == '=')) {
			if (parse_feature_request(out, feature))
				return 1;
			else
				break;
		}
	}

	if (die_on_error)
		die(_("server doesn't support feature '%s'"), feature);

	return 0;
}