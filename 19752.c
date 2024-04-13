static int parse_opt_token(const char *token)
{
	if (token == NULL)
		return OPT_ERROR;

	/*
	 * token is NULL terminated and contains exactly the
	 * keyword so we can match exactly
	 */
	if (strcmp(token, "users") == 0)
		return OPT_USERS;
	if (strcmp(token, "user_xattr") == 0)
		return OPT_USER_XATTR;
	if (strcmp(token, "user") == 0 ||
		strcmp(token, "username") == 0)
		return OPT_USER;
	if (strcmp(token, "pass") == 0 ||
		strcmp(token, "password") == 0)
		return OPT_PASS;
	if (strcmp(token, "sec") == 0)
		return OPT_SEC;
	if (strcmp(token, "ip") == 0 ||
		strcmp(token, "addr") == 0)
		return OPT_IP;
	if (strcmp(token, "unc") == 0 ||
		strcmp(token, "target") == 0 ||
		strcmp(token, "path") == 0)
		return OPT_UNC;
	if (strcmp(token, "dom") == 0 ||
		strcmp(token, "domain") == 0 ||
		strcmp(token, "workgroup") == 0)
		return OPT_DOM;
	if (strcmp(token, "cred") == 0 || /* undocumented */
		strcmp(token, "credentials") == 0)
		return OPT_CRED;
	if (strcmp(token, "uid") == 0)
		return OPT_UID;
	if (strcmp(token, "cruid") == 0)
		return OPT_CRUID;
	if (strcmp(token, "gid") == 0)
		return OPT_GID;
	if (strcmp(token, "fmask") == 0)
		return OPT_FMASK;
	if (strcmp(token, "file_mode") == 0)
		return OPT_FILE_MODE;
	if (strcmp(token, "dmask") == 0)
		return OPT_DMASK;
	if (strcmp(token, "dir_mode") == 0 ||
		strcmp(token, "dirm") == 0)
		return OPT_DIR_MODE;
	if (strcmp(token, "nosuid") == 0)
		return OPT_NO_SUID;
	if (strcmp(token, "suid") == 0)
		return OPT_SUID;
	if (strcmp(token, "nodev") == 0)
		return OPT_NO_DEV;
	if (strcmp(token, "nobrl") == 0 ||
		strcmp(token, "nolock") == 0)
		return OPT_NO_LOCK;
	if (strcmp(token, "mand") == 0)
		return OPT_MAND;
	if (strcmp(token, "nomand") == 0)
		return OPT_NOMAND;
	if (strcmp(token, "dev") == 0)
		return OPT_DEV;
	if (strcmp(token, "noexec") == 0)
		return OPT_NO_EXEC;
	if (strcmp(token, "exec") == 0)
		return OPT_EXEC;
	if (strcmp(token, "guest") == 0)
		return OPT_GUEST;
	if (strcmp(token, "ro") == 0)
		return OPT_RO;
	if (strcmp(token, "rw") == 0)
		return OPT_RW;
	if (strcmp(token, "remount") == 0)
		return OPT_REMOUNT;
	if (strcmp(token, "_netdev") == 0)
		return OPT_IGNORE;
	if (strcmp(token, "backupuid") == 0)
		return OPT_BKUPUID;
	if (strcmp(token, "backupgid") == 0)
		return OPT_BKUPGID;
	if (strcmp(token, "nofail") == 0)
		return OPT_NOFAIL;
	if (strncmp(token, "x-", 2) == 0)
		return OPT_IGNORE;
	if (strncmp(token, "snapshot", 8) == 0)
		return OPT_SNAPSHOT;

	return OPT_ERROR;
}