static int _pam_parse(const pam_handle_t *pamh,
		      int flags,
		      int argc,
		      const char **argv,
		      dictionary **result_d)
{
	int ctrl = 0;
	const char *config_file = NULL;
	int i;
	const char **v;
	dictionary *d = NULL;

	if (flags & PAM_SILENT) {
		ctrl |= WINBIND_SILENT;
	}

	for (i=argc,v=argv; i-- > 0; ++v) {
		if (!strncasecmp(*v, "config", strlen("config"))) {
			ctrl |= WINBIND_CONFIG_FILE;
			config_file = v[i];
			break;
		}
	}

	if (config_file == NULL) {
		config_file = PAM_WINBIND_CONFIG_FILE;
	}

	d = iniparser_load(CONST_DISCARD(char *, config_file));
	if (d == NULL) {
		goto config_from_pam;
	}

	if (iniparser_getboolean(d, CONST_DISCARD(char *, "global:debug"), false)) {
		ctrl |= WINBIND_DEBUG_ARG;
	}

	if (iniparser_getboolean(d, CONST_DISCARD(char *, "global:debug_state"), false)) {
		ctrl |= WINBIND_DEBUG_STATE;
	}

	if (iniparser_getboolean(d, CONST_DISCARD(char *, "global:cached_login"), false)) {
		ctrl |= WINBIND_CACHED_LOGIN;
	}

	if (iniparser_getboolean(d, CONST_DISCARD(char *, "global:krb5_auth"), false)) {
		ctrl |= WINBIND_KRB5_AUTH;
	}

	if (iniparser_getboolean(d, CONST_DISCARD(char *, "global:silent"), false)) {
		ctrl |= WINBIND_SILENT;
	}

	if (iniparser_getstr(d, CONST_DISCARD(char *, "global:krb5_ccache_type")) != NULL) {
		ctrl |= WINBIND_KRB5_CCACHE_TYPE;
	}

	if ((iniparser_getstr(d, CONST_DISCARD(char *, "global:require-membership-of"))
	     != NULL) ||
	    (iniparser_getstr(d, CONST_DISCARD(char *, "global:require_membership_of"))
	     != NULL)) {
		ctrl |= WINBIND_REQUIRED_MEMBERSHIP;
	}

	if (iniparser_getboolean(d, CONST_DISCARD(char *, "global:try_first_pass"), false)) {
		ctrl |= WINBIND_TRY_FIRST_PASS_ARG;
	}

	if (iniparser_getint(d, CONST_DISCARD(char *, "global:warn_pwd_expire"), 0)) {
		ctrl |= WINBIND_WARN_PWD_EXPIRE;
	}

	if (iniparser_getboolean(d, CONST_DISCARD(char *, "global:mkhomedir"), false)) {
		ctrl |= WINBIND_MKHOMEDIR;
	}

config_from_pam:
	/* step through arguments */
	for (i=argc,v=argv; i-- > 0; ++v) {

		/* generic options */
		if (!strcmp(*v,"debug"))
			ctrl |= WINBIND_DEBUG_ARG;
		else if (!strcasecmp(*v, "debug_state"))
			ctrl |= WINBIND_DEBUG_STATE;
		else if (!strcasecmp(*v, "silent"))
			ctrl |= WINBIND_SILENT;
		else if (!strcasecmp(*v, "use_authtok"))
			ctrl |= WINBIND_USE_AUTHTOK_ARG;
		else if (!strcasecmp(*v, "use_first_pass"))
			ctrl |= WINBIND_USE_FIRST_PASS_ARG;
		else if (!strcasecmp(*v, "try_first_pass"))
			ctrl |= WINBIND_TRY_FIRST_PASS_ARG;
		else if (!strcasecmp(*v, "unknown_ok"))
			ctrl |= WINBIND_UNKNOWN_OK_ARG;
		else if (!strncasecmp(*v, "require_membership_of",
				      strlen("require_membership_of")))
			ctrl |= WINBIND_REQUIRED_MEMBERSHIP;
		else if (!strncasecmp(*v, "require-membership-of",
				      strlen("require-membership-of")))
			ctrl |= WINBIND_REQUIRED_MEMBERSHIP;
		else if (!strcasecmp(*v, "krb5_auth"))
			ctrl |= WINBIND_KRB5_AUTH;
		else if (!strncasecmp(*v, "krb5_ccache_type",
				      strlen("krb5_ccache_type")))
			ctrl |= WINBIND_KRB5_CCACHE_TYPE;
		else if (!strcasecmp(*v, "cached_login"))
			ctrl |= WINBIND_CACHED_LOGIN;
		else if (!strcasecmp(*v, "mkhomedir"))
			ctrl |= WINBIND_MKHOMEDIR;
		else {
			__pam_log(pamh, ctrl, LOG_ERR,
				 "pam_parse: unknown option: %s", *v);
			return -1;
		}

	}

	if (result_d) {
		*result_d = d;
	} else {
		if (d) {
			iniparser_freedict(d);
		}
	}

	return ctrl;
};