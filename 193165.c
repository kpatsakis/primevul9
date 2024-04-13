static int parse_fetch_recurse(const char *opt, const char *arg,
			       int die_on_error)
{
	switch (git_config_maybe_bool(opt, arg)) {
	case 1:
		return RECURSE_SUBMODULES_ON;
	case 0:
		return RECURSE_SUBMODULES_OFF;
	default:
		if (!strcmp(arg, "on-demand"))
			return RECURSE_SUBMODULES_ON_DEMAND;

		if (die_on_error)
			die("bad %s argument: %s", opt, arg);
		else
			return RECURSE_SUBMODULES_ERROR;
	}
}