R_API void r_egg_lang_include_path(REgg *egg, const char *path) {
	char *tmp_ptr = NULL;
	char *env = r_sys_getenv (R_EGG_INCDIR_ENV);
	if (!env || !*env) {
		r_egg_lang_include_init (egg);
		free (env);
		env = r_sys_getenv (R_EGG_INCDIR_ENV);
		tmp_ptr = env;
	}
	env = r_str_appendf (NULL, "%s:%s", path, env);
	free (tmp_ptr);
	r_sys_setenv (R_EGG_INCDIR_ENV, env);
	free (env);
}