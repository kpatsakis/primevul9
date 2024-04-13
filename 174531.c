email_open_implementation(char *Mailer, const char * final_args[])
{
	priv_state priv;
	int prev_umask;
	FILE *mailerstream;

	/* Want the letter to come from "condor" if possible */
	priv = set_condor_priv();
	/* there are some oddities with how popen can open a pipe. In some
		arches, popen will create temp files for locking and they need to
		be of the correct perms in order to be deleted. So the umask is
		set to something useable for the open operation. -pete 9/11/99
	*/
	prev_umask = umask(022);
	mailerstream = my_popenv(final_args,EMAIL_POPEN_FLAGS,FALSE);
	umask(prev_umask);

	/* Set priv state back */
	set_priv(priv);

	if ( mailerstream == NULL ) {	
		dprintf(D_ALWAYS,"Failed to access email program \"%s\"\n",
			Mailer);
	}

	return mailerstream;
}