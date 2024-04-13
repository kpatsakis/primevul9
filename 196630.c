lookupUserGroup(const string &user, uid_t *uid, struct passwd **userInfo, gid_t *gid) {
	errno = 0;
	*userInfo = getpwnam(user.c_str());
	if (*userInfo == NULL) {
		if (looksLikePositiveNumber(user)) {
			int e = errno;
			fprintf(stderr,
				"Warning: error looking up system user database"
				" entry for user '%s': %s (errno=%d)\n",
				user.c_str(), strerror(e), e);
			*uid = (uid_t) atoi(user.c_str());
			*userInfo = getpwuid(*uid);
			if (*userInfo == NULL) {
				reportGetpwuidError(user, errno);
				exit(1);
			} else {
				*gid = (*userInfo)->pw_gid;
			}
		} else {
			reportGetpwuidError(user, errno);
			exit(1);
		}
	} else {
		*uid = (*userInfo)->pw_uid;
		*gid = (*userInfo)->pw_gid;
	}
}