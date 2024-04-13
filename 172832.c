R_API void r_egg_lang_include_init(REgg *egg) {
	char *s = r_str_newf (".:%s/%s", r_sys_prefix (NULL), R_EGG_INCDIR_PATH);
	r_sys_setenv (R_EGG_INCDIR_ENV, s);
	free (s);
}