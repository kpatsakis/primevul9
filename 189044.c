handle_env (pam_handle_t *pamh, int argc, const char **argv)
{
  int retval, ctrl, readenv=DEFAULT_READ_ENVFILE;
  int user_readenv = DEFAULT_USER_READ_ENVFILE;
  const char *conf_file = NULL, *env_file = NULL, *user_env_file = NULL;

  /*
   * this module sets environment variables read in from a file
   */

  D(("Called."));
  ctrl = _pam_parse(pamh, argc, argv, &conf_file, &env_file,
		    &readenv, &user_env_file, &user_readenv);

  retval = _parse_config_file(pamh, ctrl, conf_file);

  if(readenv && retval == PAM_SUCCESS) {
    retval = _parse_env_file(pamh, ctrl, env_file);
    if (retval == PAM_IGNORE)
      retval = PAM_SUCCESS;
  }

  if(user_readenv && retval == PAM_SUCCESS) {
    char *envpath = NULL;
    struct passwd *user_entry = NULL;
    const char *username;
    struct stat statbuf;

    username = _pam_get_item_byname(pamh, "PAM_USER");

    if (username)
      user_entry = pam_modutil_getpwnam (pamh, username);
    if (!user_entry) {
      pam_syslog(pamh, LOG_ERR, "No such user!?");
    }
    else {
      if (asprintf(&envpath, "%s/%s", user_entry->pw_dir, user_env_file) < 0)
	{
	  pam_syslog(pamh, LOG_ERR, "Out of memory");
	  return PAM_BUF_ERR;
	}
      if (stat(envpath, &statbuf) == 0) {
	PAM_MODUTIL_DEF_PRIVS(privs);

	if (pam_modutil_drop_priv(pamh, &privs, user_entry)) {
	  retval = PAM_SESSION_ERR;
	} else {
	  retval = _parse_config_file(pamh, ctrl, envpath);
	  if (pam_modutil_regain_priv(pamh, &privs))
	    retval = PAM_SESSION_ERR;
	}
        if (retval == PAM_IGNORE)
          retval = PAM_SUCCESS;
      }
      free(envpath);
    }
  }

  /* indicate success or failure */
  D(("Exit."));
  return retval;
}