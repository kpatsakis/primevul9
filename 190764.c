GetHomeDir(uid_t uid)
{
	struct passwd *pwEnt = NULL;
	char *homedir = NULL;

	pwEnt = getpwuid (uid);
	if (pwEnt == NULL)
		return NULL;

	if(pwEnt->pw_dir != NULL) {
		homedir = strdup (pwEnt->pw_dir);
	}

	return homedir;
}