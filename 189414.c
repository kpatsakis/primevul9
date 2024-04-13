env_set(struct environment *e, char *var, char *val)
{
    char *pair;
    debug_decl(env_set, SUDO_DEBUG_ARGS);

    pair = sudo_new_key_val(var, val);
    if (pair == NULL) {
	sudo_fatalx(U_("%s: %s"),
	    __func__, U_("unable to allocate memory"));
    }
    env_insert(e, pair);

    debug_return;
}