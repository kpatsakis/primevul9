parse_env_list(struct environment *e, char *list)
{
    char *cp, *last, *val;
    debug_decl(parse_env_list, SUDO_DEBUG_ARGS);

    for ((cp = strtok_r(list, ",", &last)); cp != NULL;
	(cp = strtok_r(NULL, ",", &last))) {
	if (strchr(cp, '=') != NULL) {
	    sudo_warnx(U_("invalid environment variable name: %s"), cp);
	    usage();
	}
	if ((val = getenv(cp)) != NULL)
	    env_set(e, cp, val);
    }
    debug_return;
}