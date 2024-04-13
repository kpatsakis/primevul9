switchUser(uid_t uid, const struct passwd *userInfo) {
	if (setuid(uid) == -1) {
		int e = errno;
		fprintf(stderr, "setuid(%d) failed: %s (errno=%d)\n",
			(int) uid, strerror(e), e);
		exit(1);
	}
	if (userInfo != NULL) {
		setenv("USER", userInfo->pw_name, 1);
		setenv("LOGNAME", userInfo->pw_name, 1);
		setenv("SHELL", userInfo->pw_shell, 1);
		setenv("HOME", userInfo->pw_dir, 1);
	} else {
		unsetenv("USER");
		unsetenv("LOGNAME");
		unsetenv("SHELL");
		unsetenv("HOME");
	}
}