static int parse_push_recurse(const char *opt, const char *arg,
			       int die_on_error)
{
	switch (git_config_maybe_bool(opt, arg)) {
	case 1:
		/* There's no simple "on" value when pushing */
		if (die_on_error)
			die("bad %s argument: %s", opt, arg);
		else
			return RECURSE_SUBMODULES_ERROR;
	case 0:
		return RECURSE_SUBMODULES_OFF;
	default:
		if (!strcmp(arg, "on-demand"))
			return RECURSE_SUBMODULES_ON_DEMAND;
		else if (!strcmp(arg, "check"))
			return RECURSE_SUBMODULES_CHECK;
		else if (!strcmp(arg, "only"))
			return RECURSE_SUBMODULES_ONLY;
		else if (die_on_error)
			die("bad %s argument: %s", opt, arg);
		else
			return RECURSE_SUBMODULES_ERROR;
	}
}