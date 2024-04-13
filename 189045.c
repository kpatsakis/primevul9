static int _undefine_var(pam_handle_t *pamh, int ctrl, VAR *var)
{
  /* We have a variable to undefine, this is a simple function */

  D(("Called and exit."));
  if (ctrl & PAM_DEBUG_ARG) {
    pam_syslog(pamh, LOG_DEBUG, "remove variable \"%s\"", var->name);
  }
  return pam_putenv(pamh, var->name);
}