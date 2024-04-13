static int _define_var(pam_handle_t *pamh, int ctrl, VAR *var)
{
  /* We have a variable to define, this is a simple function */

  char *envvar;
  int retval = PAM_SUCCESS;

  D(("Called."));
  if (asprintf(&envvar, "%s=%s", var->name, var->value) < 0) {
    pam_syslog(pamh, LOG_ERR, "out of memory");
    return PAM_BUF_ERR;
  }

  retval = pam_putenv(pamh, envvar);
  if (ctrl & PAM_DEBUG_ARG) {
    pam_syslog(pamh, LOG_DEBUG, "pam_putenv(\"%s\")", envvar);
  }
  _pam_drop(envvar);
  D(("Exit."));
  return retval;
}