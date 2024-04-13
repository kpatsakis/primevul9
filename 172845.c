static char *find_include(const char *prefix, const char *file) {
	char *pfx = NULL, *ret = NULL, *env = r_sys_getenv (R_EGG_INCDIR_ENV);
	if (!prefix) {
		prefix = "";
	}
	if (*prefix == '$') {
		char *out = r_sys_getenv (prefix + 1);
		pfx = out? out: strdup ("");
	} else {
		pfx = strdup (prefix);
		if (!pfx) {
			free (env);
			return NULL;
		}
	}

	if (env) {
		char *str, *ptr = strchr (env, ':');
		// eprintf ("MUST FIND IN PATH (%s)\n", env);
		str = env;
		while (str) {
			if (ptr) {
				*ptr = 0;
			}
			free (ret);
			ret = r_str_appendf (NULL, "%s/%s", pfx, file);
			{
				char *filepath = r_str_appendf (NULL, "%s/%s/%s", str, pfx, file);
				// eprintf ("try (%s)\n", filepath);
				if (r_file_exists (filepath)) {
					free (env);
					free (pfx);
					free (ret);
					return filepath;
				}
				free (filepath);
			}
			if (!ptr) {
				break;
			}
			str = ptr + 1;
			ptr = strchr (str, ':');
		}
		free (env);
	} else {
		ret = r_str_appendf (NULL, "%s/%s", pfx, file);
	}
	free (pfx);
	return ret;
}