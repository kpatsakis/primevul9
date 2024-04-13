reportGetpwuidError(const string &user, int e) {
	if (e == 0) {
		fprintf(stderr,
			"ERROR: Cannot lookup up system user database entry for user '%s':"
			" user does not exist\n", user.c_str());
	} else {
		fprintf(stderr,
			"ERROR: Cannot lookup up system user database entry for user '%s':"
			" %s (errno=%d)\n",
			user.c_str(), strerror(e), e);
	}
}