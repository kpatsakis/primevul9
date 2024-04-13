env_insert(struct environment *e, char *pair)
{
    debug_decl(env_insert, SUDO_DEBUG_ARGS);

    /* Make sure we have at least two slots free (one for NULL). */
    if (e->env_len + 1 >= e->env_size) {
	char **tmp;

	if (e->env_size == 0)
	    e->env_size = 16;
	tmp = reallocarray(e->envp, e->env_size, 2 * sizeof(char *));
	if (tmp == NULL)
	    sudo_fatalx(U_("%s: %s"), __func__, U_("unable to allocate memory"));
	e->envp = tmp;
	e->env_size *= 2;
    }
    e->envp[e->env_len++] = pair;
    e->envp[e->env_len] = NULL;

    debug_return;
}