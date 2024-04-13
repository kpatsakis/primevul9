static int groupcmp(void *instance, REQUEST *req, UNUSED VALUE_PAIR *request,
		    VALUE_PAIR *check, VALUE_PAIR *check_pairs,
		    VALUE_PAIR **reply_pairs)
{
	struct passwd	*pwd;
	struct group	*grp;
	char		**member;
	int		retval;

	instance = instance;
	check_pairs = check_pairs;
	reply_pairs = reply_pairs;

	/*
	 *	No user name, doesn't compare.
	 */
	if (!req->username) {
		return -1;
	}

	pwd = getpwnam(req->username->vp_strvalue);
	if (pwd == NULL)
		return -1;

	grp = getgrnam(check->vp_strvalue);
	if (grp == NULL)
		return -1;
	
	retval = (pwd->pw_gid == grp->gr_gid) ? 0 : -1;
	if (retval < 0) {
		for (member = grp->gr_mem; *member && retval; member++) {
			if (strcmp(*member, pwd->pw_name) == 0)
				retval = 0;
		}
	}
	return retval;
}