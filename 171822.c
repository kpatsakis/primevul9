static void _pam_free_data_info3(pam_handle_t *pamh)
{
	pam_set_data(pamh, PAM_WINBIND_HOMEDIR, NULL, NULL);
	pam_set_data(pamh, PAM_WINBIND_LOGONSCRIPT, NULL, NULL);
	pam_set_data(pamh, PAM_WINBIND_LOGONSERVER, NULL, NULL);
	pam_set_data(pamh, PAM_WINBIND_PROFILEPATH, NULL, NULL);
}