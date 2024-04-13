int git_config_perm(const char *var, const char *value)
{
	int i;
	char *endptr;

	if (value == NULL)
		return PERM_GROUP;

	if (!strcmp(value, "umask"))
		return PERM_UMASK;
	if (!strcmp(value, "group"))
		return PERM_GROUP;
	if (!strcmp(value, "all") ||
	    !strcmp(value, "world") ||
	    !strcmp(value, "everybody"))
		return PERM_EVERYBODY;

	/* Parse octal numbers */
	i = strtol(value, &endptr, 8);

	/* If not an octal number, maybe true/false? */
	if (*endptr != 0)
		return git_config_bool(var, value) ? PERM_GROUP : PERM_UMASK;

	/*
	 * Treat values 0, 1 and 2 as compatibility cases, otherwise it is
	 * a chmod value to restrict to.
	 */
	switch (i) {
	case PERM_UMASK:               /* 0 */
		return PERM_UMASK;
	case OLD_PERM_GROUP:           /* 1 */
		return PERM_GROUP;
	case OLD_PERM_EVERYBODY:       /* 2 */
		return PERM_EVERYBODY;
	}

	/* A filemode value was given: 0xxx */

	if ((i & 0600) != 0600)
		die("Problem with core.sharedRepository filemode value "
		    "(0%.3o).\nThe owner of files must always have "
		    "read and write permissions.", i);

	/*
	 * Mask filemode value. Others can not get write permission.
	 * x flags for directories are handled separately.
	 */
	return -(i & 0666);
}