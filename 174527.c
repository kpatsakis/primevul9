email_developers_open(const char *subject)
{
	char *tmp;
	FILE *mailer;

	/* 
	** According to the docs, if CONDOR_DEVELOPERS is not
	** in the config file, it defaults to UW.  If it is "NONE", 
	** nothing should be emailed.
	*/
    tmp = param ("CONDOR_DEVELOPERS");
    if (tmp == NULL) {
		/* we strdup here since we always call free below */
#ifdef NO_PHONE_HOME
		tmp = strdup("NONE");
#else
        tmp = strdup("condor-admin@cs.wisc.edu");
#endif
    }

    if (strcasecmp (tmp, "NONE") == 0) {
        free (tmp);
        return NULL;
    }

	mailer = email_open(tmp,subject);		

	/* Don't forget to free tmp! */
	free(tmp);
	return mailer;
}