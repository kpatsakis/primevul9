_parse_env_file(pam_handle_t *pamh, int ctrl, const char *file)
{
    int retval=PAM_SUCCESS, i, t;
    char buffer[BUF_SIZE], *key, *mark;
    FILE *conf;

    D(("Env file name is: %s", file));

    if ((conf = fopen(file,"r")) == NULL) {
      pam_syslog(pamh, LOG_ERR, "Unable to open env file: %s: %m", file);
      return PAM_IGNORE;
    }

    while (_assemble_line(conf, buffer, BUF_SIZE) > 0) {
	D(("Read line: %s", buffer));
	key = buffer;

	/* skip leading white space */
	key += strspn(key, " \n\t");

	/* skip blanks lines and comments */
	if (key[0] == '#')
	    continue;

	/* skip over "export " if present so we can be compat with
	   bash type declarations */
	if (strncmp(key, "export ", (size_t) 7) == 0)
	    key += 7;

	/* now find the end of value */
	mark = key;
	while(mark[0] != '\n' && mark[0] != '#' && mark[0] != '\0')
	    mark++;
	if (mark[0] != '\0')
	    mark[0] = '\0';

       /*
	* sanity check, the key must be alpha-numeric
	*/

	for ( i = 0 ; key[i] != '=' && key[i] != '\0' ; i++ )
	    if (!isalnum(key[i]) && key[i] != '_') {
		pam_syslog(pamh, LOG_ERR,
		           "non-alphanumeric key '%s' in %s', ignoring",
		           key, file);
		break;
	    }
	/* non-alphanumeric key, ignore this line */
	if (key[i] != '=' && key[i] != '\0')
	    continue;

	/* now we try to be smart about quotes around the value,
	   but not too smart, we can't get all fancy with escaped
	   values like bash */
	if (key[i] == '=' && (key[++i] == '\"' || key[i] == '\'')) {
	    for ( t = i+1 ; key[t] != '\0' ; t++)
		if (key[t] != '\"' && key[t] != '\'')
		    key[i++] = key[t];
		else if (key[t+1] != '\0')
		    key[i++] = key[t];
	    key[i] = '\0';
	}

	/* if this is a request to delete a variable, check that it's
	   actually set first, so we don't get a vague error back from
	   pam_putenv() */
	for (i = 0; key[i] != '=' && key[i] != '\0'; i++);

	if (key[i] == '\0' && !pam_getenv(pamh,key))
	    continue;

	/* set the env var, if it fails, we break out of the loop */
	retval = pam_putenv(pamh, key);
	if (retval != PAM_SUCCESS) {
	    D(("error setting env \"%s\"", key));
	    break;
	} else if (ctrl & PAM_DEBUG_ARG) {
	    pam_syslog(pamh, LOG_DEBUG,
		       "pam_putenv(\"%s\")", key);
	}
    }

    (void) fclose(conf);

    /* tidy up */
    D(("Exit."));
    return retval;
}