static void _pam_setup_unix_username(struct pwb_context *ctx,
				     char **user_ret,
				     struct wbcLogonUserInfo *info)
{
	const char *unix_username = NULL;
	uint32_t i;

	if (!user_ret || !info) {
		return;
	}

	for (i=0; i < info->num_blobs; i++) {
		if (strcasecmp(info->blobs[i].name, "unix_username") == 0) {
			unix_username = (const char *)info->blobs[i].blob.data;
			break;
		}
	}

	if (!unix_username || !unix_username[0]) {
		return;
	}

	*user_ret = strdup(unix_username);
}