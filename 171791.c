static void _pam_set_data_info3(struct pwb_context *ctx,
				const struct wbcAuthUserInfo *info)
{
	_pam_set_data_string(ctx, PAM_WINBIND_HOMEDIR,
			     info->home_directory);
	_pam_set_data_string(ctx, PAM_WINBIND_LOGONSCRIPT,
			     info->logon_script);
	_pam_set_data_string(ctx, PAM_WINBIND_LOGONSERVER,
			     info->logon_server);
	_pam_set_data_string(ctx, PAM_WINBIND_PROFILEPATH,
			     info->profile_path);
}