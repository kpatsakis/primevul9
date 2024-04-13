_pam_parse (const pam_handle_t *pamh, int argc, const char **argv,
	    const char **conffile, const char **envfile, int *readenv,
	    const char **user_envfile, int *user_readenv)
{
    int ctrl=0;

    *user_envfile = DEFAULT_USER_ENVFILE;
    *envfile = DEFAULT_ETC_ENVFILE;
    *readenv = DEFAULT_READ_ENVFILE;
    *user_readenv = DEFAULT_USER_READ_ENVFILE;
    *conffile = DEFAULT_CONF_FILE;

    /* step through arguments */
    for (; argc-- > 0; ++argv) {

	/* generic options */

	if (!strcmp(*argv,"debug"))
	    ctrl |= PAM_DEBUG_ARG;
	else if (!strncmp(*argv,"conffile=",9)) {
	  if ((*argv)[9] == '\0') {
	    pam_syslog(pamh, LOG_ERR,
		       "conffile= specification missing argument - ignored");
	  } else {
	    *conffile = 9+*argv;
	    D(("new Configuration File: %s", *conffile));
	  }
	} else if (!strncmp(*argv,"envfile=",8)) {
	  if ((*argv)[8] == '\0') {
	    pam_syslog (pamh, LOG_ERR,
			"envfile= specification missing argument - ignored");
	  } else {
	    *envfile = 8+*argv;
	    D(("new Env File: %s", *envfile));
	  }
	} else if (!strncmp(*argv,"user_envfile=",13)) {
	  if ((*argv)[13] == '\0') {
	    pam_syslog (pamh, LOG_ERR,
			"user_envfile= specification missing argument - ignored");
	  } else {
	    *user_envfile = 13+*argv;
	    D(("new User Env File: %s", *user_envfile));
	  }
	} else if (!strncmp(*argv,"readenv=",8))
	  *readenv = atoi(8+*argv);
	else if (!strncmp(*argv,"user_readenv=",13))
	  *user_readenv = atoi(13+*argv);
	else
	  pam_syslog(pamh, LOG_ERR, "unknown option: %s", *argv);
    }

    return ctrl;
}