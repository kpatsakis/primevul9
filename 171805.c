static int _pam_create_homedir(struct pwb_context *ctx,
			       const char *dirname,
			       mode_t mode)
{
	struct stat sbuf;

	if (stat(dirname, &sbuf) == 0) {
		return PAM_SUCCESS;
	}

	if (mkdir(dirname, mode) != 0) {

		_make_remark_format(ctx, PAM_TEXT_INFO,
				    _("Creating directory: %s failed: %s"),
				    dirname, strerror(errno));
		_pam_log(ctx, LOG_ERR, "could not create dir: %s (%s)",
		 dirname, strerror(errno));
		 return PAM_PERM_DENIED;
	}

	return PAM_SUCCESS;
}