CWD_API char *virtual_getcwd_ex(size_t *length TSRMLS_DC) /* {{{ */
{
	cwd_state *state;

	state = &CWDG(cwd);

	if (state->cwd_length == 0) {
		char *retval;

		*length = 1;
		retval = (char *) malloc(2);
		if (retval == NULL) {
			return NULL;
		}
		retval[0] = DEFAULT_SLASH;
		retval[1] = '\0';
		return retval;
	}

#ifdef TSRM_WIN32
	/* If we have something like C: */
	if (state->cwd_length == 2 && state->cwd[state->cwd_length-1] == ':') {
		char *retval;

		*length = state->cwd_length+1;
		retval = (char *) malloc(*length+1);
		if (retval == NULL) {
			return NULL;
		}
		memcpy(retval, state->cwd, *length);
		retval[0] = toupper(retval[0]);
		retval[*length-1] = DEFAULT_SLASH;
		retval[*length] = '\0';
		return retval;
	}
#endif
	*length = state->cwd_length;
	return strdup(state->cwd);
}