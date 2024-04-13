static int _pam_winbind_init_context(pam_handle_t *pamh,
				     int flags,
				     int argc,
				     const char **argv,
				     struct pwb_context **ctx_p)
{
	struct pwb_context *r = NULL;

#ifdef HAVE_GETTEXT
	textdomain_init();
#endif

	r = TALLOC_ZERO_P(NULL, struct pwb_context);
	if (!r) {
		return PAM_BUF_ERR;
	}

	talloc_set_destructor(r, _pam_winbind_free_context);

	r->pamh = pamh;
	r->flags = flags;
	r->argc = argc;
	r->argv = argv;
	r->ctrl = _pam_parse(pamh, flags, argc, argv, &r->dict);
	if (r->ctrl == -1) {
		TALLOC_FREE(r);
		return PAM_SYSTEM_ERR;
	}

	*ctx_p = r;

	return PAM_SUCCESS;
}