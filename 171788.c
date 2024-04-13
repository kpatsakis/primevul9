static int _pam_chown_homedir(struct pwb_context *ctx,
			      const char *dirname,
			      uid_t uid,
			      gid_t gid)
{
	if (chown(dirname, uid, gid) != 0) {
		_pam_log(ctx, LOG_ERR, "failed to chown user homedir: %s (%s)",
			 dirname, strerror(errno));
		return PAM_PERM_DENIED;
	}

	return PAM_SUCCESS;
}