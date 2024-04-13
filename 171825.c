static int _pam_mkhomedir(struct pwb_context *ctx)
{
	struct passwd *pwd = NULL;
	char *token = NULL;
	char *create_dir = NULL;
	char *user_dir = NULL;
	int ret;
	const char *username;
	mode_t mode = 0700;
	char *safe_ptr = NULL;
	char *p = NULL;

	/* Get the username */
	ret = pam_get_user(ctx->pamh, &username, NULL);
	if ((ret != PAM_SUCCESS) || (!username)) {
		_pam_log_debug(ctx, LOG_DEBUG, "can not get the username");
		return PAM_SERVICE_ERR;
	}

	pwd = getpwnam(username);
	if (pwd == NULL) {
		_pam_log_debug(ctx, LOG_DEBUG, "can not get the username");
		return PAM_USER_UNKNOWN;
	}
	_pam_log_debug(ctx, LOG_DEBUG, "homedir is: %s", pwd->pw_dir);

	ret = _pam_create_homedir(ctx, pwd->pw_dir, 0700);
	if (ret == PAM_SUCCESS) {
		ret = _pam_chown_homedir(ctx, pwd->pw_dir,
					 pwd->pw_uid,
					 pwd->pw_gid);
	}

	if (ret == PAM_SUCCESS) {
		return ret;
	}

	/* maybe we need to create parent dirs */
	create_dir = talloc_strdup(ctx, "/");
	if (!create_dir) {
		return PAM_BUF_ERR;
	}

	/* find final directory */
	user_dir = strrchr(pwd->pw_dir, '/');
	if (!user_dir) {
		return PAM_BUF_ERR;
	}
	user_dir++;

	_pam_log(ctx, LOG_DEBUG, "final directory: %s", user_dir);

	p = pwd->pw_dir;

	while ((token = strtok_r(p, "/", &safe_ptr)) != NULL) {

		mode = 0755;

		p = NULL;

		_pam_log_debug(ctx, LOG_DEBUG, "token is %s", token);

		create_dir = talloc_asprintf_append(create_dir, "%s/", token);
		if (!create_dir) {
			return PAM_BUF_ERR;
		}
		_pam_log_debug(ctx, LOG_DEBUG, "current_dir is %s", create_dir);

		if (strcmp(token, user_dir) == 0) {
			_pam_log_debug(ctx, LOG_DEBUG, "assuming last directory: %s", token);
			mode = 0700;
		}

		ret = _pam_create_homedir(ctx, create_dir, mode);
		if (ret) {
			return ret;
		}
	}

	return _pam_chown_homedir(ctx, create_dir,
				  pwd->pw_uid,
				  pwd->pw_gid);
}