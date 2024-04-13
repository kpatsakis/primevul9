_parse_config_file(pam_handle_t *pamh, int ctrl, const char *file)
{
    int retval;
    char buffer[BUF_SIZE];
    FILE *conf;
    VAR Var, *var=&Var;

    D(("Called."));

    var->name=NULL; var->defval=NULL; var->override=NULL;

    D(("Config file name is: %s", file));

    /*
     * Lets try to open the config file, parse it and process
     * any variables found.
     */

    if ((conf = fopen(file,"r")) == NULL) {
      pam_syslog(pamh, LOG_ERR, "Unable to open config file: %s: %m", file);
      return PAM_IGNORE;
    }

    /* _pam_assemble_line will provide a complete line from the config file,
     * with all comments removed and any escaped newlines fixed up
     */

    while (( retval = _assemble_line(conf, buffer, BUF_SIZE)) > 0) {
      D(("Read line: %s", buffer));

      if ((retval = _parse_line(pamh, buffer, var)) == GOOD_LINE) {
	retval = _check_var(pamh, var);

	if (DEFINE_VAR == retval) {
	  retval = _define_var(pamh, ctrl, var);

	} else if (UNDEFINE_VAR == retval) {
	  retval = _undefine_var(pamh, ctrl, var);
	}
      }
      if (PAM_SUCCESS != retval && ILLEGAL_VAR != retval
	  && BAD_LINE != retval && PAM_BAD_ITEM != retval) break;

      _clean_var(var);

    }  /* while */

    (void) fclose(conf);

    /* tidy up */
    _clean_var(var);        /* We could have got here prematurely,
			     * this is safe though */
    D(("Exit."));
    return (retval != 0 ? PAM_ABORT : PAM_SUCCESS);
}